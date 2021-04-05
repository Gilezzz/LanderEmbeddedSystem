/**
  ******************************************************************************
  * @file    bootloader.h
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   bootloader header file
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
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/  
#define BOOTLOADER_COMMANDS \
  { "enableBootloader", enableBootloaderAction, "u", "\r\nEnable the RF4CE bootloader mode"}, \
  { "sendRawPacket", sendRawPacketAction, "uubvub", "\r\nchannel\r\ndstAddrMode\r\ndstAddr\r\ndstPANId\r\npayloadLength\r\npayload"}, \
  { "getEUI64", getEUI64Action, "", ""}, \
  { "bl_setDestEui64", setDestEui64Action, "b", "eui64"}, \
  { "bl_getDestEui64", getDestEui64Action, "", ""}, \
  { "cloneImage", cloneImageAction, "u", "\r\npairing table entry (0xFF for recovery mode)"}, \
  { "bl_modeuart", blModeUartAction, "", "Enter bootloader mode via UART"},  \
  { "bl_modeota", blModeOTAAction, "u", "Enter bootloader mode RF the parameter is the pairing table entry (0xFF for recovery mode}"},  \
  { "bl_get", getCommandAction, "", ""}, \
  { "bl_getid", getIdCommandAction, "", ""}, \
  { "bl_getversion", getVersionCommandAction, "", ""}, \
  { "bl_read", readCommandAction, "wu", "address bytes"}, \
  { "bl_write", writeCommandAction, "wub", "address bytes data"}, \
  { "bl_erase", eraseCommandAction, "ub", "pages page_list"}, \
  { "bl_go", goAddressCommandAction, "w", "address"}
/* Private macro -------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void blDemoInit(void);
void enableBootloaderAction(void);
void sendRawPacketAction(void);
void getEUI64Action(void);
void setDestEui64Action (void);
void getDestEui64Action (void);
void getCommandAction(void);
void getIdCommandAction(void);
void getVersionCommandAction(void);
void readCommandAction(void);
void writeCommandAction(void);
void eraseCommandAction(void);
void goAddressCommandAction(void);
void blModeUartAction(void);
void blModeOTAAction(void);
void cloneImageAction(void);
void cloneImage(int8_t pairRef, uint8_t isTarget);

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
