/**
  ******************************************************************************
  * @file    stm32_sc.h
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   This file provides the RF4CE API definition for nwk coprocessor serial implemenation 
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

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef STM32_SC_H
#define STM32_SC_H

/* Includes ------------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Functions Prototypes ------------------------------------------------------*/

/**
  * @addtogroup SAP Types
  * @{
  */

/**
  * @brief Serial communication error
  * timout during serial transmission
  * 
  * 
  */
#define NWKCOP_SERIAL_TX_TIMEOUT  0xE0

/**
  * @brief Serial communication error
  * serial reception timeout
  * 
  */
#define NWKCOP_SERIAL_RX_TIMEOUT   0xE1

/**
  * @brief Serial communication error
  * host busy
  * 
  */
#define NWKCOP_HOST_BUSY  0xE2

/**
  * @brief Serial communication error
  * cmd received too long
  * 
  */
#define NWKCOP_SERIAL_RX_TOO_LONG 0xE3

/* STM32 SPI signal */
#define SPIm                   SPI2
#define SPIm_CLK               RCC_APB1Periph_SPI2
#define SPIm_GPIO              GPIOB
#define SPIm_GPIO_CLK          RCC_APB2Periph_GPIOB 
#define SPIm_PIN_SCK           GPIO_Pin_13
#define SPIm_PIN_MISO          GPIO_Pin_14
#define SPIm_PIN_MOSI          GPIO_Pin_15

/* STM32 wake signal */
#define Wake_Signal            GPIOE
#define Wake_GPIO_CLK          RCC_APB2Periph_GPIOE
#define HWAKE                  GPIO_Pin_10
#define SWAKE                  GPIO_Pin_11
#define STOP                   GPIO_Pin_12
#define SSEL                   GPIO_Pin_13

/* STM32 UART signal */
#define USARTm                   USART2
#define USARTm_GPIO              GPIOD
#define USARTm_CLK               RCC_APB1Periph_USART2
#define USARTm_GPIO_CLK          RCC_APB2Periph_GPIOD
#define USARTm_RxPin             GPIO_Pin_6
#define USARTm_TxPin             GPIO_Pin_5

/* Function prototype */
void hostSerialInit(bool isSPI);
bool isHwakeHigh(void);
uint32_t receiveHostCmd(uint8_t *cmd);
uint32_t waitRspCmd(uint8_t *rsp);
uint32_t sendHostCmd(uint8_t *cmd);

#endif 

/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
