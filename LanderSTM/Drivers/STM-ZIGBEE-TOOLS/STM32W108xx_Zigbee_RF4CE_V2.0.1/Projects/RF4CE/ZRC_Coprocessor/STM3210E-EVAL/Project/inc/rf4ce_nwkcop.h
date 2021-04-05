/**
  ******************************************************************************
  * @file    rf4ce_nwkcop.h
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   rf4ce_nwkcop header file
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

#ifndef RF4CE_NWKCOP_H
#define RF4CE_NWKCOP_H

/**
  * @addtogroup rf4ce_nwkcop RF4CE network coprocessor function and error codes
  *
  * See rf4ce_nwkcop.h for source code.
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include  "rf4ce_type.h"
#include  "nib.h"

/* Private define ------------------------------------------------------------*/ 

/**
  * @addtogroup RF4CE Network Coprocessor version defines
  * @{
  */

/**
  * @brief Version major number
  */
#define NWKCOP_VERSION_MAJOR 1

/**
  * @brief Version minor number
  */
#define NWKCOP_VERSION_MINOR 0

/**
  * @brief Version patch number
  */
#define NWKCOP_VERSION_PATCH 0
/**
  * @brief Version qualifier
  */
#define NWKCOP_VERSION_QUAL  ""

/**
  * @}
  */ /* END RF4CE Network Coprocessor version defines */

/**
  * @addtogroup RF4CE NWK coprocessor function define
  * @{
  */

/**
  * @brief Nop instruction
  */
#define NWKCOP_NOP                  	        0x00

/**
  * @brief Data request function serial code
  */
#define NWKCOP_NLDE_DATA_REQUEST	        0x01	

/**
  * @brief Data indication function serial code
  */
#define NWKCOP_NLDE_DATA_INDICATION             0x02	

/**
  * @brief Data confirm function serial code
  */
#define NWKCOP_NLDE_DATA_CONFIRM	        0x03

/**
  * @brief Auto discovery request function serial code
  */
#define NWKCOP_NLME_AUTO_DISCOVERY_REQUEST	0x04	

/**
  * @brief Auto discovery confirm function serial code
  */
#define NWKCOP_NLME_AUTO_DISCOVERY_CONFIRM      0x05

/**
  * @brief Comm status indication function serial code
  */
#define NWKCOP_NLME_COMM_STATUS_INDICATION      0x06

/**
  * @brief Discovery request function serial code
  */
#define NWKCOP_NLME_DISCOVERY_REQUEST	        0x07

/**
  * @brief Discovery indication function serial code
  */
#define NWKCOP_NLME_DISCOVERY_INDICATION	0x08

/**
  * @brief Discovery response function serial code
  */
#define NWKCOP_NLME_DISCOVERY_RESPONSE	        0x09

/**
  * @brief Discovery confirm function serial code
  */
#define NWKCOP_NLME_DISCOVERY_CONFIRM	        0x0A

/**
  * @brief Get request function serial code
  */
#define NWKCOP_NLME_GET	                        0x0B

/**
  * @brief Pair request function serial code
  */
#define NWKCOP_NLME_PAIR_REQUEST	        0x0C

/**
  * @brief Pair indication function serial code
  */
#define NWKCOP_NLME_PAIR_INDICATION	        0x0D

/**
  * @brief Pair response function serial code
  */
#define NWKCOP_NLME_PAIR_RESPONSE	        0x0E

/**
  * @brief Pair confirm function serial code
  */
#define NWKCOP_NLME_PAIR_CONFIRM	        0x0F

/**
  * @brief Reset request function serial code
  */
#define NWKCOP_NLME_RESET	                0x10

/**
  * @brief RX enable request function serial code
  */
#define NWKCOP_NLME_RX_ENABLE	                0x11

/**
  * @brief Set request function serial code
  */
#define NWKCOP_NLME_SET 	                0x12

/**
  * @brief Start request function serial code
  */
#define NWKCOP_NLME_START_REQUEST	        0x13

/**
  * @brief Start confirm function serial code
  */
#define NWKCOP_NLME_START_CONFIRM	        0x14

/**
  * @brief Unpair request function serial code
  */
#define NWKCOP_NLME_UNPAIR_REQUEST	        0x15

/**
  * @brief Unpair indication function serial code
  */
#define NWKCOP_NLME_UNPAIR_INDICATION	        0x16

/**
  * @brief Unpair response function serial code
  */
#define NWKCOP_NLME_UNPAIR_RESPONSE	        0x17

/**
  * @brief Unpair confirm function serial code
  */
#define NWKCOP_NLME_UNPAIR_CONFIRM	        0x18

/**
  * @brief Update key request function serial code
  */
#define NWKCOP_NLME_UPDATE_KEY	                0x19

/**
  * @brief Init the RF4CE network
  */
#define NWKCOP_NWK_INIT                         0x1A

/**
  * @brief Get the IEEE Address 
  */
#define NWKCOP_MAC_GET_EUI64                    0x1B

/**
  * @brief Get the firmware version, the length information is 2 bytes
  * 0xyz 0kji where where x, y and z are the major, minor and patch
  * version for the network coprocessor firmware. The k, j, and i are the major, minor, patch version for
  * RF4CE library.
  */
#define NWKCOP_FIRMWARE_VERSION                 0x1C

/**
  * @brief Power down request for STM32W coprocessor
  */
#define NWKCOP_POWER_DOWN                       0x1D

/**
  * @brief Power up request for STM32W coprocessor
  */
#define NWKCOP_POWER_UP                         0x1E

/**
  * @brief Helper function request to configure 
  * RF4CE network coprocessor layer prior 
  * to cold start initialization
  */
#define NWKCOP_NWK_CONFIG                       0x1F

/**
  * @brief Support function call to erase the 
  * RF4CE NVM network coprocessor memory 
  */
#define NWKCOP_NVM_ERASE                        0x20

/**
  * @brief Set the IEEE Address only for test
  * not available for normal use
  */
#define NWKCOP_MAC_SET_EUI64                    0x21

/**
  * @}
  */ /* END RF4CE NWK coprocessor function define */


/**
  * @addtogroup RF4CE NWK coprocessor serial communication code
  * @{
  */

/**
  * @brief Serial communication error 
  * packet too long.
  */
#define NWKCOP_PACKET_TOO_LONG 0x100000DA

/**
  * @brief Serial communication error 
  * packet damaged.
  */
#define NWKCOP_PACKET_DAMAGED 0x100000DB

/**
  * @brief Serial communication error 
  * STM32W is busy form more than a timeout.
  */
#define NWKCOP_BUSY 0x100000DE

/**
  * @brief Serial communication error
  * unknown command.
  * 
  */
#define NWKCOP_UNKNOWN_COMMAND 0x100000CB

/**
  * @brief Serial communication error
  * unknown packet type (start byte unknown).
  * 
  */
#define NWKCOP_UNKNOWN_PACKET_TYPE 0x100000CA

/**
  * @brief Serial communication error
  * wrong parameters length
  * 
  */
#define NWKCOP_WRONG_PARAM_LENGTH 0x100000C9

/**
  * @brief Serial communication error
  * wrong parameters lengthend byte missing
  * 
  */
#define NWKCOP_END_PACKET_MISSING 0x100000C8

/**
  * @}
  */ /* END RF4CE NWK coprocessor serial communication code */


/**
  * @addtogroup RF4CE NWK coprocessor packet code
  * @{
  */

/**
  * @brief Packet start byte.
  * 
  */
#define NWKCOP_COMMAND_PACKET 0xC0

/**
  * @brief Packet start byte. At the command end
  * the STM32W goes in power save.
  * 
  */
#define NWKCOP_COMMAND_SLEEP_PACKET 0xD0

/**
  * @brief Response packet start byte.
  * 
  */
#define NWKCOP_RSP_PACKET 0xC1

/**
  * @brief Bootloader packet start byte.
  * 
  */
#define NWKCOP_BOOTLOADER_PACKET 0xC2

/**
  * @brief End packet.
  * 
  */
#define NWKCOP_END_PACKET 0xFC

/**
  * @}
  */  /* END RF4CE NWK coprocessor packet code */

/**
  * @addtogroup RF4CE NWK coprocessor serial communication type
  * @{
  */

/**
  * @brief SPI serial communication.
  * 
  */
#define NWKCOP_SPI 0xCC

/**
  * @brief UART serial communication
  * 
  */
#define NWKCOP_UART 0xCD

/**
  * @}
  */ /* END RF4CE NWK coprocessor serial communication type */

/* Private function prototypes -----------------------------------------------*/
void NWKCOP_Tick(void);
void nwkCop_debugMessage (char *fmt, ...);

#endif /* RF4CE_NWKCOP_H */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
