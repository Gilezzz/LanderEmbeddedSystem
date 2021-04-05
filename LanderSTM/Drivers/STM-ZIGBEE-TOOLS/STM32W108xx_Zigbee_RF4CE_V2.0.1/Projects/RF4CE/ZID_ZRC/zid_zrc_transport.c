/**
  ******************************************************************************
  * @file    zid_zrc_transport.c
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   This file provides the HID ZRC usb transport firmware 
  *            package to manage the communication with HID class driver
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
#include PLATFORM_HEADER
#include "hal.h"
#include "error.h"

#include <stdio.h>
#include <string.h>

#include "zid_zrc_usb.h"
#include "zid_zrc_transport.h"

/* Private define ------------------------------------------------------------*/
/* This is a timeout for the UART communication */
#define UART_TIMEOUT 6000 /* 6 sec */

/* UART statistics */
#define UART_TXIDLE 0x06
#define UART_RXOVF  0x03
#define UART_TXFREE 0x02
#define UART_RXVAL  0x01

#define START_RECEIVE_BYTE   0x00
#define LENGTH_RECEIVE_BYTE  0x01
#define CMD_RECEIVE_BYTE     0x02
#define PAYLOAD_RECEIVE_BYTE 0x03
#define END_RECEIVE_BYTE     0x04

/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static uint32_t currentMilliSecondTime(void);
static uint32_t timeMilliSecondElapsed(uint32_t startTime);

/* Public variables ----------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static uint32_t currentMilliSecondTime(void)
{
  return halCommonGetInt32uMillisecondTick ();
}

static uint32_t timeMilliSecondElapsed(uint32_t startTime)
{
  uint32_t now = halCommonGetInt32uMillisecondTick();
  
  return (now - startTime);
}

/* Public functions ----------------------------------------------------------*/

/**@brief  This function initialize the UART communication
  *@param  None
  *@retval None
  */
void transportInit(void) // ??? To add
{
  /* UART configured as:
      RTS:  disabled
      #BIT: 8
      STOP: 1 Stop bit
      PAR:  Parity error None
      FLOW: Flow Control disabled
      AUTO: automatic nRTS disabled
   */
  SC1_UART->UARTCR = 0x02;

  /* Set the boaud rate period and fractional period 
     register to obtain 115200 */
  SC1_UART->UARTBRR1 = 104;
  SC1_UART->UARTBRR2 = 0;

  /* SC configured as UART mode */
  SC1_UART->CR = 1;

  halGpioConfig(PORTB_PIN(1),GPIO_Mode_AF_PP);
  halGpioConfig(PORTB_PIN(2),GPIO_Mode_IN);

}

/**@brief Packet format to send will be: 
  *
  *           START_BYTE  | PACKET_LENGTH | PACKET | END_BYTE 
  *
  * Note the packet length not include the START_BYTE, itself and the END_BYTE 
  * The first byte of the packet is the USB transport command code 
  */
 
/**@brief  allows sending packet
  *@param  packet to be sent
  *@param  packet lenght
  *@retval unsigned value
  */   
uint32_t sendPacket(uint8_t *packet, uint16_t length)
{
  uint16_t i;

  for (i=0; i<length; i++)
  {
    __io_putchar(packet[i]);
  }
  
  return SUCCESS;
}

/**@brief  This function Send the packet over UART to the HID Class driver
  *@param  packet Packet to send over UART
  *@param  length Packet length
  *@retval Send status. Valid value are:
  *        - SUCCESS
  *        - ???
  */
uint32_t sendTransportPacket(uint8_t cmdTrans, uint8_t *packet, uint16_t length)
{
  uint8_t buff[261];
  uint32_t status;

  buff[0] = START_PACKET;
  buff[1] = (length+1) >> 8;
  buff[2] = (length+1) & 0x00ff;
  buff[3] = cmdTrans;
  memcpy(&buff[4], packet, length);
  buff[length+4] = END_PACKET;

  status = sendPacket(buff, length+5);
  
  return status;
}

/**@brief  This function manage the packet received over UART and 
  *        call the appropriate HID instruction
  *@param  None
  *@retval None
  */
void transportInterpreter(void) 
{
  uint8_t data;
  static uint8_t byte_length=0;
  static uint16_t length;
  static uint8_t packet[256];
  static uint8_t state=START_RECEIVE_BYTE;
  static uint8_t index=0;
  static boolean flushData = FALSE;
  static uint32_t now;

  if (__io_getcharNonBlocking(&data)) {
    now = currentMilliSecondTime();
    switch(state) {
    case START_RECEIVE_BYTE:
      packet[index++] = data;
      if (data == START_PACKET) {
	byte_length = 0;
	state = LENGTH_RECEIVE_BYTE;
      } else {
        flushData = TRUE;
      }
      break;
    case LENGTH_RECEIVE_BYTE:
      if (byte_length == 0) {
	length = data << 8;
      } else {
	length += data;
      }
      if (length > (sizeof(packet)-3)) {
	state = START_RECEIVE_BYTE;
	flushData = TRUE;
      } else {
	if (byte_length == 1) {
	  state = PAYLOAD_RECEIVE_BYTE;
	}
        packet[index++] = data;
	byte_length++;
      }
      break;
    case PAYLOAD_RECEIVE_BYTE:
      packet[index++] = data;
      if ((index-3) == length)
	state = END_RECEIVE_BYTE;
      break;
    case END_RECEIVE_BYTE:
      state = START_RECEIVE_BYTE;
      packet[index] = data;
      if (data == END_PACKET) {
	now = currentMilliSecondTime();
	hidCommandInterpreter(&packet[3], length); 
        index = 0;
        flushData = FALSE;
      } else {
        index++;
        flushData = TRUE;
      } 
      break;
    }
  }
  if (flushData || (timeMilliSecondElapsed(now) > 2000)) { /* Timeout 2 sec */
    hidSerialInputFlush(packet, index);  
    index = 0;
    flushData = FALSE;
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
