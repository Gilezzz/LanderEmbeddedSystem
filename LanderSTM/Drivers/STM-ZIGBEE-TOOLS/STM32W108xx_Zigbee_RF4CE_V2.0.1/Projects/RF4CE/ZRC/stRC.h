/**
  ******************************************************************************
  * @file    stRC.h
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   This file contains all the functions prototypes for the stRC 
  *          firmware library.
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
#ifndef stRC_H
#define stRC_H

/* Includes ------------------------------------------------------------------*/
#include "stm32w108xx.h"

/* Private typedef -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void powerSaveManagement(void);
void enablePowerSaveAction(void *val);
void setPairRef (uint8_t index);
int8_t firstValidPairingTableEntry (void);
void eraseNVMAction(void *none);
void cmdMenuAction(void *none);
void cfgDeviceAction (void *voidParams);
uint32_t cfgDevice(bool target, bool forceColdStart);
void dscPairAction(void *none);
void sendDataAction(void *voidParams);
uint32_t sendData(uint8_t cmdId, uint8_t *cmdKey);
void dumpPairingTableAction (void *none);
void printPrompt(void);
void rebootAction(void *none);
void warmStart(void);
void erasePairingTableAction(void *none);
void cloneImageButtonAction(void *none);
void irCmdInterpreter(uint8_t *cmd);
void dscPairTTAction(void *none);
void helpAction(void);
void interactiveAction(void);

#endif /* stRC_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
