/**
  ******************************************************************************
  * @file    zid_zrc_transport.h
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   ZID ZRC transport header file
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

/**
  * @addtogroup ZID ZRC USB Transport API
  * See zid_transport.h for source code.
  * @{
  */
#ifndef ZID_ZRC_TRANSPORT_H
#define ZID_ZRC_TRANSPORT_H

/**
  * @addtogroup HID TRANSPORT constant
  * @{
  */

/**
  * @brief HID Reset transport command Code
  */
#define RESET_TRANS_CODE 0x01

/**
  * @brief HID get number of report descriptor transport command Code
  */
#define GET_NUMBER_REPORT_DESC_TRANS_CODE 0x02

/**
  * @brief HID get number of report descriptor response transport command Code
  */
#define GET_NUMBER_REPORT_DESC_RSP_TRANS_CODE 0x03

/**
  * @brief HID Add device transport command Code
  */
#define ADD_HID_DEVICE_TRANS_CODE 0x04

/**
  * @brief HID get number interface transport command Code
  */
#define GET_NUMBER_INTERFACE_TRANS_CODE 0x05

/**
  * @brief HID get number itnerface response transport command Code
  */
#define GET_NUMBER_INTERFACE_RSP_TRANS_CODE 0x06

/**
  * @brief HID Host device full transport command Code
  */
#define HOST_DEVICE_FULL_TRANS_CODE 0x07

/**
  * @brief HID get descriptor transport command Code
  */
#define GET_DESCRIPTOR_TRANS_CODE 0x10

/**
  * @brief HID Response to a get descriptor transport command Code
  */
#define GET_DESCRIPTOR_RSP_TRANS_CODE 0x11

/**
  * @brief HID get report transport command Code
  */
#define GET_REPORT_TRANS_CODE 0x12

/**
  * @brief HID Response to a get report transport command Code
  */
#define GET_REPORT_RSP_TRANS_CODE 0x13

/**
  * @brief HID set report transport command Code
  */
#define SET_REPORT_TRANS_CODE 0x14

/**
  * @brief HID get idle transport command Code
  */
#define GET_IDLE_TRANS_CODE 0x16

/**
  * @brief HID Reposnse to a get idle transport command Code
  */
#define GET_IDLE_RSP_TRANS_CODE 0x17

/**
  * @brief HID set idle transport command Code
  */
#define SET_IDLE_TRANS_CODE 0x18

/**
  * @brief HID get protocol transport command Code
  */
#define GET_PROTOCOL_TRANS_CODE 0x1A

/**
  * @brief HID Response to a get protocol transport command Code
  */
#define GET_PROTOCOL_RSP_TRANS_CODE 0x1B

/**
  * @brief HID set protocol transport command Code
  */
#define SET_PROTOCOL_TRANS_CODE 0x1C

/**
  * @brief HID Configuration complete transport command Code
  */
#define CONFIGURATION_COMPLETE_TRANS_CODE 0x1E

/**
  * @brief HID Report Data transport command Code
  */
#define REPORT_DATA_TRANS_CODE 0x1F

/**
  * @brief HID transport packet start byte
  */
#define START_PACKET 0xFC

/**
  * @brief HID transport packet end byte
  */
#define END_PACKET 0xCC

/**
  * @brief Error serial timeout
  */
#define SERIAL_TX_TIMEOUT 0xFC10

/**
  * @}
  */  /* END HID TRANSPORT constant */

/**@brief  This function initialize the UART communication
  *@param  None
  *@retval None
  */
void transportInit(void);

/**@brief  This function Send the packet over UART to the HID Class driver
  *@param  packet Packet to send over UART
  *@param  length Packet length
  *@retval Send status. Valid value are:
  *        - SUCCESS
  *        - ???
  */
uint32_t sendTransportPacket(uint8_t cmdTrans, uint8_t *packet, uint16_t length);

/**@brief  This function manage the packet received over UART and 
  *        call the appropriate HID instruction
  *@param  None
  *@retval None
  */
void transportInterpreter(void);

#endif /*  ZID_ZRC_TRANSPORT_H */

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
