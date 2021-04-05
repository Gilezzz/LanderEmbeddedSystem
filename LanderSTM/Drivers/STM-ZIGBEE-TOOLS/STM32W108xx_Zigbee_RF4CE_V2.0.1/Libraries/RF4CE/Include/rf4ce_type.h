/**
  ******************************************************************************
  * @file    rf4ce_type.h
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   rf4ce_type Header file 
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
 * @addtogroup rf4ce_type
 *
 * See rf4ce_type.h for source code.
 * @{
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef RF4CE_TYPE_H
#define RF4CE_TYPE_H

/* Includes ------------------------------------------------------------------*/

#include PLATFORM_HEADER

/* Private define ------------------------------------------------------------*/  

#ifndef TRUE
/**
  * @brief An alias for one, used for clarity.
  */
#define TRUE  1
#endif
#ifndef FALSE
/**
  * @brief An alias for zero, used for clarity.
  */
#define FALSE 0
#endif

/* Private typedef -----------------------------------------------------------*/

typedef uint8_t bool;

typedef uint8_t IEEEAddr[8];

typedef uint8_t linkKeyType[16];

#endif /* RF4CE_TYPE_H */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
