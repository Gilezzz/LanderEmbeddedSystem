/**
  ******************************************************************************
  * @file    stm32_sc.c
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   This file provides the RF4CE API for nwk coprocessor serial implemenation
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/  
#include "stm32f10x.h"
#include "rf4ce.h"
#include "rf4ce_nwkcop.h"
#include "stm32_sc.h"
#include <string.h>
#include <stdio.h>

#define TIME_OUT_HOST 300 // 300 msec

/* Data received from the nwk cop */
uint8_t hostRx[200];
uint8_t hostRxTop;
uint8_t hostRxBottom;
/* Flag to signal byte command end */
__IO uint8_t hostCmdEnd;
__IO int timeTick_ms;

/* Private variable ----------------------------------------------------------*/
/* Flag to signal the communication type  */
static bool sc_isSPI;

/* Private function prototypes -----------------------------------------------*/
static void serialQueueInit(void);
static void timeOutStart(void);
static int timeElapsed(void);
static void RCC_Configuration(bool isSPI);
static void GPIO_Configuration(bool isSPI);
static void SPI_Configuration(void);
static void UART_Configuration(void);
static bool isStopHigh(void);
static uint32_t sendHostSPICmd(uint8_t *cmd);
static uint32_t sendHostUartCmd(uint8_t *cmd);
static uint32_t waitRspSpiCmd(uint8_t *rsp);
static uint32_t waitRspUartCmd(uint8_t *rsp);
static uint32_t receiveHostSpiCmd(uint8_t *cmd);
static uint32_t receiveHostUartCmd(uint8_t *cmd);
static void nwkCopSpiInit(bool isSPI);
static void nwkCopUartInit(bool isSPI);

/* Private function ----------------------------------------------------------*/

/**@brief Serial Queue init
  *@param None  
  *@retval None
  */
static void serialQueueInit(void)
{
  hostRxTop = 0;
  hostRxBottom = 0;
}

/**@brief Time Out Start
  *@param None  
  *@retval None
  */
static void timeOutStart(void)
{
  timeTick_ms = 0;
}

/**@brief Time Elapsed
  *@param None  
  *@retval None
  */
static int timeElapsed(void)
{
  return timeTick_ms;
}

/**@brief RCC Configuration
  *@param isSPI: SPI or UART coprocessor  
  *@retval None
  */
static void RCC_Configuration(bool isSPI)
{
  if (isSPI) {
    /* Enable SPIm Periph clock */
    RCC_APB1PeriphClockCmd(SPIm_CLK, ENABLE);
    /* Enable SPIm clock and GPIO clock for wake signal */
    RCC_APB2PeriphClockCmd(SPIm_GPIO_CLK | Wake_GPIO_CLK, ENABLE);
  } else {   
    /* Enable USARTm clock and GPIO clock for wake signal */
    RCC_APB2PeriphClockCmd(USARTm_GPIO_CLK | Wake_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE);
    /* Enable USARTm Clock */
    RCC_APB1PeriphClockCmd(USARTm_CLK, ENABLE);  
  }
}

/**@brief GPIO Configuration
  *@param isSPI: SPI or UART coprocessor  
  *@retval None
  */
static void GPIO_Configuration(bool isSPI)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  if (isSPI) {
    /* Configure SPIm pins: SCK, MISO and MOSI */
    GPIO_InitStructure.GPIO_Pin = SPIm_PIN_SCK | SPIm_PIN_MISO | SPIm_PIN_MOSI;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(SPIm_GPIO, &GPIO_InitStructure);
  } else {
    GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE); 
    /* Configure USARTm Rx as input floating */
    GPIO_InitStructure.GPIO_Pin = USARTm_RxPin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(USARTm_GPIO, &GPIO_InitStructure);  
    /* Configure USARTm Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = USARTm_TxPin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(USARTm_GPIO, &GPIO_InitStructure); 
  }
  
  /* Configure nHwake nSwake Stop pins */
  GPIO_InitStructure.GPIO_Pin = HWAKE | STOP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(Wake_Signal, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = SWAKE | SSEL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(Wake_Signal, &GPIO_InitStructure);
 
  /* Set the wake GPIO to the right value */
  GPIO_SetBits(Wake_Signal, SSEL); 
  GPIO_SetBits(Wake_Signal, SWAKE); 

}

/**@brief SPI Configuration
  *@param None
  *@retval None
  */
static void SPI_Configuration(void)
{
  SPI_InitTypeDef SPI_InitStructure;
  
  SPI_I2S_DeInit(SPIm);
  
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_Init(SPIm, &SPI_InitStructure);
  
  /* Enable SPIy */
  SPI_Cmd(SPIm, ENABLE);
  
  return;
}

/**@brief UART Configuration
  *@param None
  *@retval None
  */
static void UART_Configuration(void)
{
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Init the queue to store the data received from the nwk corpocessor */
  serialQueueInit();
      
  /* Enable the USARTm Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  /* Configure USARTm */
  USART_Init(USARTm, &USART_InitStructure);
  /* Enable the USARTm */
  USART_Cmd(USARTm, ENABLE);
  
  /* The two while below are necessary to avoid the timeout signaling sent from
     the nwk coprocessor after the init */
  timeOutStart();
  while((USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET) && (timeElapsed() < TIME_OUT_HOST));
  while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET)
  {
    /* Read bytes from the receive data register */
    hostRx[0] = USART_ReceiveData(USART2);
  }
  /* Enable the Rx interrupt */
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}

/**@brief nwk Coprocessor SPI Init
  *@param isSPI SPI or UART coprocessor  
  *@retval None
  */
static void nwkCopSpiInit(bool isSPI)
{
  RCC_Configuration(isSPI);
  GPIO_Configuration(isSPI);
  SPI_Configuration();
}

/**@brief nwk Coprocessor UART Init
  *@param isSPI SPI or UART coprocessor  
  *@retval None
  */
static void nwkCopUartInit(bool isSPI)
{
  RCC_Configuration(isSPI);
  GPIO_Configuration(isSPI);
  UART_Configuration();
}

/**@brief is Stop High
  *@param None 
  *@retval bool
  */
static bool isStopHigh(void)
{
  if (GPIO_ReadInputDataBit(Wake_Signal, STOP))
    return TRUE;
  else
    return FALSE;
}

/**@brief Send Host SPI Cmd
  *@param *cmd pointer on the command to be send 
  *@retval operation status
  */
static uint32_t sendHostSPICmd(uint8_t *cmd)
{
  uint8_t i=0, length;
  
  timeOutStart();
  while(isStopHigh() && (timeElapsed() < TIME_OUT_HOST));
  timeOutStart();
  if (isStopHigh() || !isHwakeHigh())
    return NWKCOP_BUSY;
  GPIO_ResetBits(Wake_Signal, SWAKE); 
  /* Added to avoid to send a command when the coprocessor want to start a communication */
  if (!isHwakeHigh())
    return NWKCOP_BUSY;
  while(!isStopHigh() && (timeElapsed() < TIME_OUT_HOST));
  if (!isStopHigh())
    return NWKCOP_BUSY;
  GPIO_SetBits(Wake_Signal, SWAKE); 
  GPIO_ResetBits(Wake_Signal, SSEL); 

  while (SPI_I2S_GetFlagStatus(SPIm, SPI_I2S_FLAG_TXE) == RESET);
    
  length = cmd[1] + 3;
  for (i = 0; i<length; i++) {
    SPI_I2S_SendData(SPIm, cmd[i]);
    while (SPI_I2S_GetFlagStatus(SPIm, SPI_I2S_FLAG_TXE) == RESET);
  }
  return SUCCESS;
}

/**@brief Send Host UART Cmd
  *@param *cmd pointer on the command to be send 
  *@retval operation status
  */
static uint32_t sendHostUartCmd(uint8_t *cmd)
{
  uint8_t i=0, length;
  
  hostCmdEnd = 0;
  timeOutStart();
  while(isStopHigh() && (timeElapsed() < TIME_OUT_HOST));
  timeOutStart();
  if (isStopHigh() || !isHwakeHigh())
    return NWKCOP_BUSY;
  GPIO_ResetBits(Wake_Signal, SWAKE);
  /*Added to avoid to send a command when the coprocessor want to start a communication */
  if (!isHwakeHigh())
    return NWKCOP_BUSY;
  while(!isStopHigh() && (timeElapsed() < TIME_OUT_HOST));
  if (!isStopHigh())
    return NWKCOP_BUSY;
  GPIO_SetBits(Wake_Signal, SWAKE); 
  while (USART_GetFlagStatus(USARTm, USART_FLAG_TXE) == RESET);
  
  length = cmd[1] + 3;
  for (i = 0; i<length; i++) {
    USART_SendData(USARTm, cmd[i]);
    while (USART_GetFlagStatus(USARTm, USART_FLAG_TXE) == RESET);
  }
  return SUCCESS;
}

/**@brief Wait Rsp SPI Cmd
  *@param *rsp pointer to receive the response 
  *@retval operation status
  */
static uint32_t waitRspSpiCmd(uint8_t *rsp)
{
  uint8_t i=0;

  timeOutStart();
  while(timeElapsed() < TIME_OUT_HOST) {
    while (SPI_I2S_GetFlagStatus(SPIm, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPIm, 0xff);
    /* while (SPI_I2S_GetFlagStatus(SPIm, SPI_I2S_FLAG_BSY) != RESET); */
    while (SPI_I2S_GetFlagStatus(SPIm, SPI_I2S_FLAG_RXNE) == RESET);
    rsp[i] = SPI_I2S_ReceiveData(SPIm);
    if (rsp[0] == NWKCOP_RSP_PACKET)
      i++;
    if ((i > 1) && (i > (rsp[1]+3))) {
      GPIO_SetBits(Wake_Signal, SSEL);
      return SUCCESS;
    }
  }
  return NWKCOP_SERIAL_RX_TIMEOUT;
}

/**@brief Wait Rsp UART Cmd
  *@param *rsp pointer to receive the response 
  *@retval operation status
  */
static uint32_t waitRspUartCmd(uint8_t *rsp)
{
  uint8_t i=0;

  timeOutStart();
  while((hostCmdEnd == 0) && (timeElapsed() < TIME_OUT_HOST));
  if (hostCmdEnd == 0)
    return NWKCOP_SERIAL_RX_TIMEOUT;
  timeOutStart();
  while(timeElapsed() < TIME_OUT_HOST) {
    while (hostRxTop != hostRxBottom) {
        rsp[i] = hostRx[hostRxBottom];
        hostRxBottom = (hostRxBottom+1) % sizeof(hostRx);
        if (rsp[0] == NWKCOP_RSP_PACKET)
          i++;
    }
    if ((i > 1) && (i == (rsp[1]+3)))
      return SUCCESS;
  }
  return NWKCOP_SERIAL_RX_TIMEOUT;
}

/**@brief Receive Host SPI Cmd
  *@param *cmd pointer to command received
  *@retval operation status
  */
static uint32_t receiveHostSpiCmd(uint8_t *cmd)
{
  uint8_t i=0;

  timeOutStart();
  while(isHwakeHigh() && (timeElapsed() < TIME_OUT_HOST));
  if (isHwakeHigh())
    return NWKCOP_SERIAL_RX_TIMEOUT;
  GPIO_ResetBits(Wake_Signal, SWAKE);
  timeOutStart();
  while(!isStopHigh() && (timeElapsed() < TIME_OUT_HOST));
  if (!isStopHigh())
    return NWKCOP_SERIAL_RX_TIMEOUT;
  GPIO_SetBits(Wake_Signal, SWAKE);

  GPIO_ResetBits(Wake_Signal, SSEL);

  timeOutStart();
  while(timeElapsed() < TIME_OUT_HOST) {    
    while (SPI_I2S_GetFlagStatus(SPIm, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPIm, 0xFF);
    while (SPI_I2S_GetFlagStatus(SPIm, SPI_I2S_FLAG_RXNE) == RESET);
    cmd[i] = SPI_I2S_ReceiveData(SPIm);
    if (cmd[0] == NWKCOP_COMMAND_PACKET)
      i++;
    if ((i > 1) && (i > (cmd[1]+3))) {
      GPIO_SetBits(Wake_Signal, SSEL);
      return SUCCESS;
    }
  }
  return NWKCOP_SERIAL_RX_TIMEOUT;
}

/**@brief Receive Host UART Cmd
  *@param *cmd pointer to command received
  *@retval operation status
  */
static uint32_t receiveHostUartCmd(uint8_t *cmd)
{
  uint8_t i=0;

  hostCmdEnd = 0;
  timeOutStart();
  while(isHwakeHigh() && (timeElapsed() < TIME_OUT_HOST));
  if (isHwakeHigh())
    return NWKCOP_SERIAL_RX_TIMEOUT;
  GPIO_ResetBits(Wake_Signal, SWAKE);
  timeOutStart();
  while(!isStopHigh() && (timeElapsed() < TIME_OUT_HOST));
  if (!isStopHigh())
    return NWKCOP_SERIAL_RX_TIMEOUT;
  GPIO_SetBits(Wake_Signal, SWAKE);
  
  timeOutStart();
  while ((hostCmdEnd == 0) && (timeElapsed() < TIME_OUT_HOST));
  if (hostCmdEnd == 0)
    return NWKCOP_SERIAL_RX_TIMEOUT;
  timeOutStart();
  while (timeElapsed() < TIME_OUT_HOST) {
    while (hostRxTop != hostRxBottom) {
        cmd[i] = hostRx[hostRxBottom];
        hostRxBottom = (hostRxBottom+1) % sizeof(hostRx);
        if (cmd[0] == NWKCOP_COMMAND_PACKET) 
          i++;
    }
    if ((i > 1) && (i == (cmd[1]+3)))
      return SUCCESS;
  }
  return NWKCOP_SERIAL_RX_TIMEOUT;
}

/* Public function -----------------------------------------------------------*/

/**@brief Check the Hardware wake status high
  *@param None
  *@retval bool
  */
bool isHwakeHigh(void)
{
  if (GPIO_ReadInputDataBit(Wake_Signal, HWAKE))
    return TRUE;
  else
    return FALSE;
}

/**@brief Send Host Cmd
  *@param *cmd pointer on command to be send
  *@retval operation status
  */
uint32_t sendHostCmd(uint8_t *cmd)
{  
  if (sc_isSPI)
    return sendHostSPICmd(cmd);
  else
    return sendHostUartCmd(cmd);     
}

/**@brief Wait response Cmd
  *@param *rsp pointer to receive the response 
  *@retval operation status
  */
uint32_t waitRspCmd(uint8_t *rsp)
{
  if (sc_isSPI)
    return waitRspSpiCmd(rsp);
  else
    return waitRspUartCmd(rsp);     
}

/**@brief Receive Host Cmd
  *@param *cmd pointer on command to be received
  *@retval operation status
  */
uint32_t receiveHostCmd(uint8_t *cmd)
{
  if (sc_isSPI)
    return receiveHostSpiCmd(cmd);
  else
    return receiveHostUartCmd(cmd);     
}

/**@brief Host serial interface init
  *@param isSPI: SPI or UART coprocessor 
  *@retval None
  */
void hostSerialInit(bool isSPI)
{
  /* Setup SysTick Timer for 1 msec interrupts  */
  SysTick_Config(SystemCoreClock / 1000);

  sc_isSPI = isSPI;
  if (isSPI) {
    nwkCopSpiInit(isSPI);
  } else {
    nwkCopUartInit(isSPI);
  }  
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
