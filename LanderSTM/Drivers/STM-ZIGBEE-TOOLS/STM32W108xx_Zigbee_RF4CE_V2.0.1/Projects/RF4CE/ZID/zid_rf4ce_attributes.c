/**
  ******************************************************************************
  * @file    zid_rf4ce_attributes.c 
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   ZID attributes management
  * @verbatim
  *
 ===============================================================================
                             ##### Note #####
 ===============================================================================

* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.

    @endverbatim
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
#include "hal_flash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "zid_rf4ce.h"
#include "zid_rf4ce_sm.h"
#include "zid_rf4ce_attributes.h"

/* Private define ------------------------------------------------------------*/

/* RF4CE 1.1.0 uses 252 bytes, so if using RF4CE the data storage starts at 0x140 */
/* We allocate 6 HID groups of profile attributes because we have 5 paring table entry + the Device profile attributes */
#define NVM_HID_DEV_ATTR_SIZE      126
#define NVM_ADAPTER_ATTR_SIZE      14

#define NVM_HID_DEV_ATTR_OFFSET    (NVM_MGMT_SIZE_B+0x0140)
#ifdef ZID_FULL
#define NVM_HID_PROXY_0_OFFSET     (NVM_HID_DEV_ATTR_OFFSET+NVM_ADAPTER_ATTR_SIZE) 
#define NVM_HID_PROXY_1_OFFSET     (NVM_HID_PROXY_0_OFFSET+NVM_HID_DEV_ATTR_SIZE) 
#define NVM_HID_PROXY_2_OFFSET     (NVM_HID_PROXY_1_OFFSET+NVM_HID_DEV_ATTR_SIZE) 
#define NVM_HID_PROXY_3_OFFSET     (NVM_HID_PROXY_2_OFFSET+NVM_HID_DEV_ATTR_SIZE) 
#define NVM_HID_PROXY_4_OFFSET     (NVM_HID_PROXY_3_OFFSET+NVM_HID_DEV_ATTR_SIZE) 
#define NUMB_PROXY_TABLE_ENTRY     5
#else
#define NUMB_PROXY_TABLE_ENTRY     1
#endif
#define FLASH_PAGE_SIZE            MFB_PAGE_SIZE_B

#define MIN_NON_STD_REPORT_ID      0x80

/* Private typedef -----------------------------------------------------------*/

#pragma pack(1)
typedef struct headerNonStdDescS {
  uint8_t hidDescType;
  uint8_t hidCompId;
  uint16_t hidDescSize;
}headerNonStdDesc_Type;

typedef struct hidAttributeS {
  uint8_t  hidDeviceType;
  uint8_t  aplKeyExchangeTransferCount;
  uint16_t aplZIDProfileVersion;
  uint16_t aplHIDParserVersion;
  uint8_t  aplHIDCountryCode;
  uint16_t aplHIDDeviceReleaseNumber;
  uint16_t aplHIDVendorId;
  uint16_t aplHIDProductId;
  uint8_t  aplPowerStatus;
  uint16_t aplIntPipeUnsafeTxWindowTime;
  uint8_t  aplReportRepeatInterval;
  uint8_t  aplHIDDeviceSubclass;
  uint8_t  aplHIDProtocolCode;
  uint8_t  aplHIDNumEndpoints;
  uint8_t  aplHIDPollInterval;
  uint8_t  aplHIDNumStdDescComps;
  uint8_t  aplHIDStdDescCompsList[aplcMaxStdDescCompsPerHID]; // 12 bytes
  uint8_t  aplHIDNumNullReports;
  uint8_t  aplHIDNumNonStdDescComps;
  headerNonStdDesc_Type headerNonStdDesc[aplcMaxNonStdDescCompsPerHID]; // struct = 4 bytes * 4 entry = 16 bytes
  uint8_t  nullReports[aplcMaxNonStdDescCompsPerHID][aplcMaxNullReportSize+2]; // array 4 entry * 18 bytes = 72 bytes
  uint8_t  aplDeviceIdleRate;
  uint8_t  aplCurrentProtocol;
} hidAttribute_Type;
#pragma pack()

typedef struct attrFlushInfoS {
  bool needFlush;
  uint8_t attrRef;
} attrFlushInfo_Type;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static hidAttribute_Type hidAttribute;
static attrFlushInfo_Type flushInfo;

#if defined (__ICCARM__) || defined (__GNUC__)
NO_STRIPPING(NO_INIT(VAR_AT_SEGMENT (const uint8_t proxyTable[NUMB_PROXY_TABLE_ENTRY*FLASH_PAGE_SIZE], __NVM__)));
#elif defined __CC_ARM
uint8_t proxyTable[NUMB_PROXY_TABLE_ENTRY*FLASH_PAGE_SIZE] __attribute__((section("__NVM__"), zero_init));
#endif

/* Private function prototypes -----------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
uint8_t HIDDeviceType=HID_CLASS_DEVICE;

/* Private functions ---------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/

/**@brief  This function checks if the device is a HID adapter
  *@param  None
  *@retval TRUE if the device is a HID Adapter, FALSE otherwise
  */
bool isHIDAdapter(void)
{
  if (HIDDeviceType == HID_ADAPTER)
    return TRUE;
  else
    return FALSE;
}

/**@brief  This function erase the proxy table entry
  *@param  pairRef is the pairing refernece which associated the proxy entry
  *@retval None
  */
void ZID_EraseProxyEntry(uint8_t pairRef)
{
  uint32_t address;

  if ((!isHIDAdapter() && pairRef > 0) || 
      (pairRef > (NUMB_PROXY_TABLE_ENTRY-1)))
    return;

  if (!isHIDAdapter())
    pairRef = 0;
  
  address = (uint32_t)(proxyTable+(pairRef*FLASH_PAGE_SIZE));
  if (halInternalFlashErase(MFB_PAGE_ERASE, address) != 0) {
    return;
  }
}

/**@brief  Init the Device Attributes with the default value
  *@param  coldStart if the device has executed a cold start 
  *        the function init the default parameters. Otherwise recall the user 
  *        modified device parameter
  *@param  hidDeviceType device type:
  *        - 1 = HID Adapter
  *        - 0 = HID Device
  *@retval None
  */
void ZID_DeviceAttrInit(bool coldStart, uint8_t hidDeviceType)
{
  uint8_t i;

  if (coldStart) {
    // Attributes managed by HID device
    hidAttribute.aplKeyExchangeTransferCount = aplKeyExchangeTransferCount_Default;
    hidAttribute.aplPowerStatus = aplPowerStatus_Default;
    hidAttribute.aplZIDProfileVersion = aplZIDProfileVersion_Default;
    hidAttribute.aplIntPipeUnsafeTxWindowTime = aplIntPipeUnsafeTxWindowTime_Default;
    hidAttribute.aplReportRepeatInterval = aplReportRepeatInterval_Default;
    hidAttribute.aplHIDParserVersion = aplHIDParserVersion_Default;
    hidAttribute.aplHIDDeviceSubclass = aplHIDDeviceSubclass_Default;
    hidAttribute.aplHIDProtocolCode = aplHIDProtocolCode_Default;
    hidAttribute.aplHIDCountryCode = aplHIDCountryCode_Default;
    hidAttribute.aplHIDDeviceReleaseNumber = aplHIDDeviceReleaseNumber_Default;
    hidAttribute.aplHIDVendorId = aplHIDVendorId_Default;
    hidAttribute.aplHIDProductId = aplHIDProductId_Default;
    hidAttribute.aplHIDNumEndpoints = aplHIDNumEndpoints_Default;
    hidAttribute.aplHIDPollInterval = aplHIDPollInterval_Default;
    hidAttribute.aplHIDNumStdDescComps = aplHIDNumStdDescComps_Default;
    memset(hidAttribute.aplHIDStdDescCompsList, 0, aplcMaxStdDescCompsPerHID);
    hidAttribute.aplHIDNumNonStdDescComps = aplHIDNumNonStdDescComps_Default;
    memset(hidAttribute.headerNonStdDesc, 0, sizeof(hidAttribute.headerNonStdDesc));
    hidAttribute.aplHIDNumNullReports = aplHIDNumNullReports_Default;
    memset(hidAttribute.nullReports, 0, sizeof(hidAttribute.nullReports));
    hidAttribute.aplDeviceIdleRate = 0;
    hidAttribute.aplCurrentProtocol = 1; //Report Protocol
    hidAttribute.hidDeviceType = hidDeviceType;
    HIDDeviceType = hidDeviceType;
    if (HIDDeviceType == HID_CLASS_DEVICE) {
      ATOMIC(halCommonWriteToNvm((uint8_t*)&hidAttribute, NVM_HID_DEV_ATTR_OFFSET, NVM_HID_DEV_ATTR_SIZE););
    } else {
      ATOMIC(halCommonWriteToNvm((uint8_t*)&hidAttribute, NVM_HID_DEV_ATTR_OFFSET, NVM_ADAPTER_ATTR_SIZE););
    }
    for (i=0; i<NUMB_PROXY_TABLE_ENTRY; i++)
      ZID_EraseProxyEntry(i);
  } else {
    if (HIDDeviceType == HID_CLASS_DEVICE)
      halCommonReadFromNvm((uint8_t*)&hidAttribute, NVM_HID_DEV_ATTR_OFFSET, NVM_HID_DEV_ATTR_SIZE);
    else
      halCommonReadFromNvm((uint8_t*)&hidAttribute, NVM_HID_DEV_ATTR_OFFSET, NVM_ADAPTER_ATTR_SIZE);
    HIDDeviceType = hidAttribute.hidDeviceType;
  }
  flushInfo.attrRef = 0;
  flushInfo.needFlush = FALSE;
}

/**@brief  Get the HID device attribute information
  *@param  value contains the attribute information
  *@retval operation result. Valid value are:
  *        - HID_ATTR_SUCCESS
  *        - HID_ATTR_UNSUPPRTED 
  *        - HID_ATTR_ILLEGAL_REQ
  */
ATTR_Status ZID_GetDeviceAttr(deviceAttributeInfo_Type *attr)
{
  ATTR_Status status = HID_ATTR_SUCCESS;
  uint32_t offset;
  uint8_t indexRef, attrSize;
  bool isAdapter;

  isAdapter = isHIDAdapter();
#ifdef ZID_FULL
  if (attr->proxyAttr) {
    indexRef = attr->proxyRef + 1;
    attrSize = NVM_HID_DEV_ATTR_SIZE;
    offset = NVM_HID_PROXY_0_OFFSET  + (attr->proxyRef * NVM_HID_DEV_ATTR_SIZE);
  } else {
#endif
    indexRef = 0;
    offset = NVM_HID_DEV_ATTR_OFFSET;
#ifdef ZID_FULL
   if (isAdapter) 
      attrSize = NVM_ADAPTER_ATTR_SIZE;
   else
#endif
      attrSize = NVM_HID_DEV_ATTR_SIZE;
#ifdef ZID_FULL
  }
#endif
  if ((indexRef > nwkcMaxPairingTableEntries) ||
      ((indexRef > 0) && !isAdapter))
    return HID_ATTR_ILLEGAL_REQ;

#ifdef TEST
  if (indexRef != flushInfo.attrRef) {
    flushInfo.needFlush = TRUE;
#else
  if (flushInfo.needFlush && (indexRef != flushInfo.attrRef)) {
#endif
    ZID_AttrFlush();
  }

  if (indexRef != flushInfo.attrRef) {
    halCommonReadFromNvm((uint8_t*)&hidAttribute, offset, attrSize);
    flushInfo.attrRef = indexRef;
  }

  switch(attr->id) {
  case aplKeyExchangeTransferCount_ID:
    {
      attr->value[0] = hidAttribute.aplKeyExchangeTransferCount;
      attr->length = 1;
    }
    break;
  case aplPowerStatus_ID:
    {
      status = HID_ATTR_UNSUPPORTED; /* ??? */
    }
    break;
  case aplZIDProfileVersion_ID:
    {
      uint16_t value;
      value = hidAttribute.aplZIDProfileVersion;
      memcpy(attr->value, &value, 2);
      attr->length = 2;
    }
    break;
  case aplIntPipeUnsafeTxWindowTime_ID:
    {
      uint16_t value;
#ifdef ZID_FULL
      if (!isAdapter || attr->proxyAttr) {
#endif
	value = hidAttribute.aplIntPipeUnsafeTxWindowTime;
	memcpy(attr->value, &value, 2);
	attr->length = 2;
#ifdef ZID_FULL
      } else {
	status = HID_ATTR_UNSUPPORTED;
      }
#endif
    }
    break;
  case aplReportRepeatInterval_ID:
    {
#ifdef ZID_FULL
      if (!isAdapter || attr->proxyAttr) {
#endif
	attr->value[0] = hidAttribute.aplReportRepeatInterval;
	attr->length = 1;
#ifdef ZID_FULL
      } else {
	status = HID_ATTR_UNSUPPORTED;
      }      
#endif
    }
    break;
  case aplHIDParserVersion_ID:
    {
      uint16_t value;
      value = hidAttribute.aplHIDParserVersion;
      memcpy(attr->value, &value, 2);
      attr->length = 2;
    }
    break;
  case aplHIDDeviceSubclass_ID:
    {
#ifdef ZID_FULL
      if (!isAdapter || attr->proxyAttr) {
#endif
	attr->value[0] = hidAttribute.aplHIDDeviceSubclass;
	attr->length = 1;
#ifdef ZID_FULL
      } else {
	status = HID_ATTR_UNSUPPORTED;
      }
#endif
    }
    break;
  case aplHIDProtocolCode_ID:
    {
#ifdef ZID_FULL
      if (!isAdapter || attr->proxyAttr) {
#endif
	attr->value[0] = hidAttribute.aplHIDProtocolCode;
	attr->length = 1;
#ifdef ZID_FULL
      } else {
	status = HID_ATTR_UNSUPPORTED;
      }
#endif
    }
    break;
  case aplHIDCountryCode_ID:
    {
      attr->value[0] = hidAttribute.aplHIDCountryCode;
      attr->length = 1;
    }
    break;
  case aplHIDDeviceReleaseNumber_ID:
    {
      uint16_t value;
      value = hidAttribute.aplHIDDeviceReleaseNumber;
      memcpy(attr->value, &value, 2);
      attr->length = 2;
    }
    break;
  case aplHIDVendorId_ID:
    {
      uint16_t value;
      value = hidAttribute.aplHIDVendorId;
      memcpy(attr->value, &value, 2);
      attr->length = 2;
    }
    break;
  case aplHIDProductId_ID:
    {
      uint16_t value;
      value = hidAttribute.aplHIDProductId;
      memcpy(attr->value, &value, 2);
      attr->length = 2;
    }
    break;
  case aplHIDNumEndpoints_ID:
    {
#ifdef ZID_FULL
      if (!isAdapter || attr->proxyAttr) {
#endif
	attr->value[0] = hidAttribute.aplHIDNumEndpoints;
	attr->length = 1;
#ifdef ZID_FULL
      } else {
	status = HID_ATTR_UNSUPPORTED;
      }
#endif
    }
    break;
  case aplHIDPollInterval_ID:
    {
#ifdef ZID_FULL
      if (!isAdapter || attr->proxyAttr) {
#endif
	attr->value[0] = hidAttribute.aplHIDPollInterval;
	attr->length = 1;
#ifdef ZID_FULL
      } else {
	status = HID_ATTR_UNSUPPORTED;
      }
#endif
    }
    break;
  case aplHIDNumStdDescComps_ID:
    {
#ifdef ZID_FULL
      if (!isAdapter || attr->proxyAttr) {
#endif
	attr->value[0] = hidAttribute.aplHIDNumStdDescComps;
	attr->length = 1;
#ifdef ZID_FULL
      } else {
	status = HID_ATTR_UNSUPPORTED;
      }
#endif
    }
    break;
  case aplHIDStdDescCompsList_ID:
    {
#ifdef ZID_FULL
      if (!isAdapter || attr->proxyAttr) {
#endif
	attr->length = hidAttribute.aplHIDNumStdDescComps;
	memcpy(attr->value, hidAttribute.aplHIDStdDescCompsList, attr->length);
#ifdef ZID_FULL
      } else {
	status = HID_ATTR_UNSUPPORTED;
      }
#endif
    }
    break;
  case aplHIDNumNullReports_ID:
    {
#ifdef ZID_FULL
      if (!isAdapter || attr->proxyAttr) {
#endif
	attr->value[0] = hidAttribute.aplHIDNumNullReports;
	attr->length = 1;
#ifdef ZID_FULL
      } else {
	status = HID_ATTR_UNSUPPORTED;
      }
#endif
    }
    break;
  case aplHIDNumNonStdDescComps_ID:
    {
#ifdef ZID_FULL
      if (!isAdapter || attr->proxyAttr) {
#endif
	attr->value[0] = hidAttribute.aplHIDNumNonStdDescComps;
	attr->length = 1;
#ifdef ZID_FULL
      } else {
	status = HID_ATTR_UNSUPPORTED;
      }
#endif
    }
    break;
  default: 
    { 
      if ((attr->id < aplHIDNonStdDescCompSpec_MIN_ID) || 
	  (attr->id > aplHIDNonStdDescCompSpec_MAX_ID) ||
	  (isAdapter && !attr->proxyAttr)) {
	status = HID_ATTR_UNSUPPORTED;
      } else {
	uint8_t index, *base_addr;
	uint16_t descSize;
	if (!isAdapter)
	  attr->proxyRef = 0;
	index = attr->id - aplHIDNonStdDescCompSpec_MIN_ID;
	base_addr = (uint8_t*)(proxyTable + (attr->proxyRef*FLASH_PAGE_SIZE) + index * aplcMaxNonStdDescCompSize);
	attr->length = (hidAttribute.headerNonStdDesc[index]).hidDescSize + 4;
	attr->value[0] = (hidAttribute.headerNonStdDesc[index]).hidDescType;
	descSize = (hidAttribute.headerNonStdDesc[index]).hidDescSize; 
	attr->value[1] = descSize >> 8;
        attr->value[2] = descSize & 0x00FF;
	attr->value[3] = (hidAttribute.headerNonStdDesc[index]).hidCompId;
	memcpy(&attr->value[4], base_addr, (hidAttribute.headerNonStdDesc[index]).hidDescSize);
      }
    }
    break;
  }   
  return status;
}

/**@brief  Set the HID device attribute information
  *@param  value is an input variable with the attribute value to set
  *@retval operation result. Valid value are:
  *        - HID_ATTR_SUCCESS
  *        - HID_ATTR_UNSUPPRTED 
  *        - HID_ATTR_ILLEGAL_REQ
  */
ATTR_Status ZID_SetDeviceAttr(deviceAttributeInfo_Type *attr)
{
  ATTR_Status status = HID_ATTR_SUCCESS;
  uint32_t offset;
  uint8_t indexRef, attrSize;
  bool isAdapter;

  isAdapter = isHIDAdapter();

#ifdef ZID_FULL
  if (attr->proxyAttr) {
    indexRef = attr->proxyRef + 1;
    attrSize = NVM_HID_DEV_ATTR_SIZE;
    offset = NVM_HID_PROXY_0_OFFSET  + (attr->proxyRef * NVM_HID_DEV_ATTR_SIZE);
  } else {
#endif
    indexRef = 0;
    offset = NVM_HID_DEV_ATTR_OFFSET;
#ifdef ZID_FULL
   if (isAdapter) 
      attrSize = NVM_ADAPTER_ATTR_SIZE;
   else
#endif
      attrSize = NVM_HID_DEV_ATTR_SIZE;
#ifdef ZID_FULL
  }
#endif
   
  if ((indexRef > nwkcMaxPairingTableEntries) ||
      ((indexRef > 0) && !isAdapter))
    return HID_ATTR_ILLEGAL_REQ;

#ifdef TEST
  if (indexRef != flushInfo.attrRef) {
    flushInfo.needFlush = TRUE;
#else
  if (flushInfo.needFlush && (indexRef != flushInfo.attrRef)) {
#endif
    ZID_AttrFlush();
  }

  if (indexRef != flushInfo.attrRef) {
    halCommonReadFromNvm((uint8_t*)&hidAttribute, offset, attrSize);
    flushInfo.attrRef = indexRef;
  }

  switch(attr->id) {
  case aplKeyExchangeTransferCount_ID:
    {
      if ((attr->value[0] >= aplcMinKeyExchangeTransferCount) && 
	  (attr->length == 1)) {
	hidAttribute.aplKeyExchangeTransferCount = attr->value[0];
      } else {
	status = HID_ATTR_ILLEGAL_REQ;
      }
    }
    break;
    case aplPowerStatus_ID:
      {
	status = HID_ATTR_ILLEGAL_REQ; // ???
      }
      break;
  case aplZIDProfileVersion_ID:
    {
      uint16_t value;
      if (attr->length == 2) {
	memcpy(&value, attr->value, attr->length);
	hidAttribute.aplZIDProfileVersion = value;
      } else {
	status = HID_ATTR_ILLEGAL_REQ;
      }
    }
    break;
  case aplIntPipeUnsafeTxWindowTime_ID:
    {
#ifdef ZID_FULL
      if (isAdapter && !attr->proxyAttr) {
	status = HID_ATTR_UNSUPPORTED;
      } else{
#endif
	uint16_t val; 
	memcpy(&val, attr->value, 2);
	if ((attr->length == 2) &&
	    (val >= aplcMinIntPipeUnsafeTxWindowTime)) {
	  hidAttribute.aplIntPipeUnsafeTxWindowTime = val;
	} else {
	  status = HID_ATTR_ILLEGAL_REQ;
	}
#ifdef ZID_FULL
      }
#endif
    }
    break;
  case aplReportRepeatInterval_ID:
    {
#ifdef ZID_FULL
      if (isAdapter && !attr->proxyAttr) {
	status = HID_ATTR_UNSUPPORTED;
      } else{
#endif
	if ((attr->length == 1) &&
	    (attr->value[0] <= aplcMaxReportRepeatInterval)) {
	  hidAttribute.aplReportRepeatInterval = attr->value[0];
	} else {
	  status = HID_ATTR_ILLEGAL_REQ;
	}
#ifdef ZID_FULL
      }
#endif
    }
    break;
  case aplHIDParserVersion_ID:
    {
      uint16_t value;
      if (attr->length == 2) {
	memcpy(&value, attr->value, 2);
	hidAttribute.aplHIDParserVersion = value;
      } else {
	status = HID_ATTR_ILLEGAL_REQ;
      }
    }
    break;
  case aplHIDDeviceSubclass_ID:
    {
#ifdef ZID_FULL
      if (isAdapter && !attr->proxyAttr) {
	status = HID_ATTR_UNSUPPORTED;
      } else {
#endif
	if (attr->length == 1)
	  hidAttribute.aplHIDDeviceSubclass = attr->value[0];
	else 
	  status = HID_ATTR_ILLEGAL_REQ;
#ifdef ZID_FULL
      }
#endif
    }
    break;
  case aplHIDProtocolCode_ID:
    {
#ifdef ZID_FULL
      if (isAdapter && !attr->proxyAttr) {
	status = HID_ATTR_UNSUPPORTED;
      } else {
#endif
	if (attr->length == 1)
	  hidAttribute.aplHIDProtocolCode = attr->value[0];
	else 
	  status = HID_ATTR_ILLEGAL_REQ;
#ifdef ZID_FULL
      }
#endif
    }
    break;
  case aplHIDCountryCode_ID:
    {
      if (attr->length == 1)
	hidAttribute.aplHIDCountryCode = attr->value[0];
      else
	status = HID_ATTR_ILLEGAL_REQ;
    }
    break;
  case aplHIDDeviceReleaseNumber_ID:
    {
      uint16_t value;
      if (attr->length == 2) {
	memcpy(&value, attr->value, 2);
	hidAttribute.aplHIDDeviceReleaseNumber = value;
      } else {
	status = HID_ATTR_ILLEGAL_REQ;
      }
    }
    break;
  case aplHIDVendorId_ID:
    {
      uint16_t value;
      if (attr->length == 2) {
	memcpy(&value, attr->value, 2);
	hidAttribute.aplHIDVendorId = value;
      } else {
	status = HID_ATTR_ILLEGAL_REQ;
      }
    }
    break;
  case aplHIDProductId_ID:
    {
      uint16_t value;
      if (attr->length == 2) {
	memcpy(&value, attr->value, 2);
	hidAttribute.aplHIDProductId = value;
      } else {
	status = HID_ATTR_ILLEGAL_REQ;
      }
    }
    break;
  case aplHIDNumEndpoints_ID:
    {
#ifdef ZID_FULL
      if (isAdapter && !attr->proxyAttr) {
	status = HID_ATTR_UNSUPPORTED;
      } else {
#endif
	if (attr->length == 1)
	  hidAttribute.aplHIDNumEndpoints = attr->value[0];
	else
	  status = HID_ATTR_ILLEGAL_REQ;
#ifdef ZID_FULL
      }
#endif
    }
    break;
  case aplHIDPollInterval_ID:
    {
#ifdef ZID_FULL
      if (isAdapter && !attr->proxyAttr) {
	status = HID_ATTR_UNSUPPORTED;
      } else {
#endif
	if ((attr->value[0] >= aplHIDPollInterval_MIN) && 
	    (attr->value[0] <= aplHIDPollInterval_MAX) && 
	    (attr->length == 1))
	  hidAttribute.aplHIDPollInterval = attr->value[0];
	else
	  status = HID_ATTR_ILLEGAL_REQ;
#ifdef ZID_FULL
      }
#endif
    }
    break;
  case aplHIDNumStdDescComps_ID:
    {
#ifdef ZID_FULL
      if (isAdapter && !attr->proxyAttr) {
	status = HID_ATTR_UNSUPPORTED;
      } else {
#endif
	if ((attr->value[0] > aplcMaxStdDescCompsPerHID) ||
	    (attr->length != 1))
	  status = HID_ATTR_ILLEGAL_REQ;
	else
	  hidAttribute.aplHIDNumStdDescComps = attr->value[0];
#ifdef ZID_FULL
      }
#endif
    }
    break;
  case aplHIDStdDescCompsList_ID:
    {
#ifdef ZID_FULL
      if (isAdapter && !attr->proxyAttr) {
	status = HID_ATTR_UNSUPPORTED;
      } else {
#endif
	if (attr->length > aplcMaxStdDescCompsPerHID)
	  status = HID_ATTR_ILLEGAL_REQ;
	else
	  memcpy(hidAttribute.aplHIDStdDescCompsList, attr->value, attr->length);
#ifdef ZID_FULL
      }
#endif
    }
    break;
  case aplHIDNumNullReports_ID:
    {
#ifdef ZID_FULL
      if (isAdapter && !attr->proxyAttr) {
	status = HID_ATTR_UNSUPPORTED;
      } else {
#endif
	if ((attr->length == 1) && 
	    (attr->value[0] <= aplcMaxNonStdDescCompsPerHID))
	  hidAttribute.aplHIDNumNullReports = attr->value[0];
	else
	  status = HID_ATTR_ILLEGAL_REQ;
#ifdef ZID_FULL
      }
#endif
    }
    break;
  case aplHIDNumNonStdDescComps_ID:
    {
#ifdef ZID_FULL
      if (isAdapter && !attr->proxyAttr) {
	status = HID_ATTR_UNSUPPORTED;
      } else {
#endif
	if ((attr->length != 1) || 
	    (attr->value[0] > aplcMaxNonStdDescCompsPerHID)) 
	  status = HID_ATTR_ILLEGAL_REQ;
	else 
	  hidAttribute.aplHIDNumNonStdDescComps = attr->value[0];
#ifdef ZID_FULL
      }
#endif
    }
    break;
  default:
    { 
      if ((attr->id < aplHIDNonStdDescCompSpec_MIN_ID) || 
	  (attr->id > aplHIDNonStdDescCompSpec_MAX_ID) ||
	  (isAdapter && !attr->proxyAttr) ||
	  (attr->value[3] < MIN_NON_STD_REPORT_ID)) {
	status = HID_ATTR_UNSUPPORTED;
      } else { 
	uint8_t index;
	uint32_t base_addr;
	uint16_t length;

	if (!isAdapter)
	  attr->proxyRef = 0;

	index = attr->id - aplHIDNonStdDescCompSpec_MIN_ID;
	(hidAttribute.headerNonStdDesc[index]).hidDescType = attr->value[0];
	length = attr->value[1] << 8 | attr->value[2];
	(hidAttribute.headerNonStdDesc[index]).hidDescSize = length;
	(hidAttribute.headerNonStdDesc[index]).hidCompId = attr->value[3];
	if ((hidAttribute.headerNonStdDesc[index]).hidDescSize <= aplcMaxNonStdDescCompSize) {
	  length = (((hidAttribute.headerNonStdDesc[index]).hidDescSize % 2) + (hidAttribute.headerNonStdDesc[index]).hidDescSize)/2;
	  base_addr = (uint32_t)(proxyTable + (attr->proxyRef*FLASH_PAGE_SIZE) + index * aplcMaxNonStdDescCompSize); 
	  if (halInternalFlashWrite(base_addr, (uint16_t*)&attr->value[4], length) != 0) {
	    status = HID_ATTR_ILLEGAL_REQ;
	  }
	} else {
	  status = HID_ATTR_ILLEGAL_REQ;
	}
      }
    }
    break;
  }   

#ifndef TEST
  if (status == HID_ATTR_SUCCESS) {
      flushInfo.needFlush = TRUE;
  }
#endif

  return status;
}

/**@brief  Executes a flush to store the HID device attributes inside 
  *        the non __IO memory. 
  *@NOTE:  This function will be executed only when the HID host/device
  *        is in IDLE state.
  *@param  None
  *@retval None
  */
void ZID_AttrFlush(void)
{
  uint32_t offset;
  uint8_t attrSize;
  
  if (flushInfo.needFlush) {
    if (flushInfo.attrRef == 0) {
      offset = NVM_HID_DEV_ATTR_OFFSET;
      if (!isHIDAdapter())
	attrSize = NVM_HID_DEV_ATTR_SIZE;
      else 
	attrSize = NVM_ADAPTER_ATTR_SIZE;
    } else {
      if (!isHIDAdapter())
	return;
#ifdef ZID_FULL
      offset = NVM_HID_PROXY_0_OFFSET  + ((flushInfo.attrRef-1)* NVM_HID_DEV_ATTR_SIZE); 
      attrSize = NVM_HID_DEV_ATTR_SIZE;
#endif
    }
    ATOMIC(halCommonWriteToNvm((uint8_t*)&hidAttribute, offset, attrSize););
    flushInfo.needFlush = FALSE;
  }
}

/**@brief  This function sets the NULL report for the non std descriptor
  *@param  pairRef If the device is an adapter, this parameter is the pairing reference.
  *        If it is a hid class device the paramter shall be 0
  *@param  nullDesc returns the null descriptor
  *@param  descLen returns the null descriptor length
  *@retval operation result. Valid value are:
  *        - HID_ATTR_SUCCESS
  *        - HID_ATTR_UNSUPPRTED 
  *        - HID_ATTR_ILLEGAL_REQ
  */
ATTR_Status ZID_SetNullReport(uint8_t pairRef, uint8_t *nullDesc, uint8_t descLen)
{
  uint8_t index, attrSize, i;
  uint32_t offset;
  bool idOk;

  if ((descLen > (aplcMaxNullReportSize+2)) ||
      (pairRef >= nwkcMaxPairingTableEntries) ||
      (nullDesc[0] < MIN_NON_STD_REPORT_ID)) 
    return HID_ATTR_ILLEGAL_REQ;

#ifdef ZID_FULL
  if (isHIDAdapter())
    pairRef++;
#endif
  else 
    pairRef = 0;

  if (flushInfo.needFlush && (pairRef != flushInfo.attrRef))
    ZID_AttrFlush();

  attrSize = NVM_HID_DEV_ATTR_SIZE;
  if (!isHIDAdapter()) {
    offset = NVM_HID_DEV_ATTR_OFFSET;
  } else {
#ifdef ZID_FULL
    offset = NVM_HID_PROXY_0_OFFSET  + ((pairRef-1) * NVM_HID_DEV_ATTR_SIZE); 
#endif
  }

  if (pairRef != flushInfo.attrRef) {
    halCommonReadFromNvm((uint8_t*)&hidAttribute, offset, attrSize);
    flushInfo.attrRef = pairRef;
  }

  idOk = FALSE;
  for (i=0; i<aplcMaxNonStdDescCompsPerHID; i++) {
    if ((hidAttribute.headerNonStdDesc[i]).hidCompId == nullDesc[0]) {
      index = i;
      idOk = TRUE;
      break;
    }
  }

  if (idOk) {
    memcpy(hidAttribute.nullReports[index], nullDesc, descLen); 
    flushInfo.needFlush = TRUE;
    return HID_ATTR_SUCCESS;
  }

  return HID_ATTR_ILLEGAL_REQ;
}

/**@brief  This function gets the NULL report for the non std descriptor
  *@param  pairRef If the device is an adapter, this parameter is the pairing reference.
  *        If it is a hid class device the paramter shall be 0
  *@param  reportId is the report identifier for the asscoiated null report
  *@param  nullDesc returns the null descriptor
  *@param  descLen returns the null descriptor length
  *@retval operation result. Valid value are:
  *        - HID_ATTR_SUCCESS
  *        - HID_ATTR_UNSUPPRTED 
  *        - HID_ATTR_ILLEGAL_REQ
  */
ATTR_Status ZID_GetNullReport(uint8_t pairRef, uint8_t reportId, uint8_t *nullDesc, uint8_t *descLen)
{
  uint8_t attrSize, i;
  uint32_t offset;

  if (pairRef >= nwkcMaxPairingTableEntries)
    return HID_ATTR_ILLEGAL_REQ;
#ifdef ZID_FULL
  if (isHIDAdapter())
    pairRef++;
#endif
  else
    pairRef = 0;

  if (flushInfo.needFlush && (pairRef != flushInfo.attrRef))
    ZID_AttrFlush();
  
  attrSize = NVM_HID_DEV_ATTR_SIZE;
  if (!isHIDAdapter()) {
    offset = NVM_HID_DEV_ATTR_OFFSET;
  } else {
#ifdef ZID_FULL
    offset = NVM_HID_PROXY_0_OFFSET  + ((pairRef-1) * NVM_HID_DEV_ATTR_SIZE); 
#endif
  }
  
  if (pairRef != flushInfo.attrRef) {
    halCommonReadFromNvm((uint8_t*)&hidAttribute, offset, attrSize);
    flushInfo.attrRef = pairRef;
  }
  
  for (i=0; i<aplcMaxNonStdDescCompsPerHID; i++) {
    if (hidAttribute.nullReports[i][0] == reportId) {
      *descLen = hidAttribute.nullReports[i][1] + 2;
      memcpy(nullDesc, hidAttribute.nullReports[i], *descLen);
      return HID_ATTR_SUCCESS;
    }
  }

  return HID_ATTR_ILLEGAL_REQ;
}

#ifdef ZID_FULL
/**@brief  This function return the flash address for the non standard report descriptor
  *@param  pairRef pairing reference index
  *@param  reportId is the report identifier for the asscoiated null report
  *@param  reportLen is the length of the non standard report descriptor
  *@retval The start flash address for the non standard report descriptor
  */
uint32_t ZID_GetNonStdReportAddr(uint8_t pairRef, uint8_t reportId, uint16_t *rptLen)
{
  uint8_t indexRef;
  uint32_t base_addr, offset, attrSize;
  uint16_t length;

  indexRef = pairRef + 1;
  offset = NVM_HID_PROXY_0_OFFSET  + (pairRef * NVM_HID_DEV_ATTR_SIZE);
  attrSize = NVM_HID_DEV_ATTR_SIZE;

  if (flushInfo.needFlush && (indexRef != flushInfo.attrRef))
    ZID_AttrFlush();

  if (indexRef != flushInfo.attrRef) {
    halCommonReadFromNvm((uint8_t*)&hidAttribute, offset, attrSize);
    flushInfo.attrRef = indexRef;
  }

  indexRef = reportId - aplHIDNonStdDescCompSpec_MIN_ID;
  length = (hidAttribute.headerNonStdDesc[indexRef]).hidDescSize;
  base_addr = (uint32_t)(proxyTable + (pairRef*FLASH_PAGE_SIZE) + indexRef * aplcMaxNonStdDescCompSize); 
  *rptLen = length;

  return base_addr;
}

/**@brief  This function returns the device current protocol
  *@param  pairRef pairing reference index
  *@param  protocol contain the protocol value:
  *        - 0 Boot
  *        - 1 Report
  *@retval HID_ATTR_ILLEGAL_REQ or HID_ATTR_SUCCESS;
  */
uint32_t ZID_GetCurrentProtocol(uint8_t pairRef, uint8_t *protocol)
{
  uint8_t attrSize;
  uint32_t offset;

  if ((pairRef >= nwkcMaxPairingTableEntries) || !isHIDAdapter())
    return HID_ATTR_ILLEGAL_REQ;

  pairRef++;

  if (flushInfo.needFlush && (pairRef != flushInfo.attrRef))
    ZID_AttrFlush();
  
  attrSize = NVM_HID_DEV_ATTR_SIZE;
  offset = NVM_HID_PROXY_0_OFFSET  + ((pairRef-1) * NVM_HID_DEV_ATTR_SIZE); 
  
  if (pairRef != flushInfo.attrRef) {
    halCommonReadFromNvm((uint8_t*)&hidAttribute, offset, attrSize);
    flushInfo.attrRef = pairRef;
  }
  
  *protocol = hidAttribute.aplCurrentProtocol;

  return HID_ATTR_SUCCESS;
}

/**@brief  This function sets the device current protocol
  *@param  pairRef pairing reference index
  *@param  protocol contain the protocol value:
  *        - 0 Boot
  *        - 1 Report
  *@retval HID_ATTR_ILLEGAL_REQ or HID_ATTR_SUCCESS;
  */
uint32_t ZID_SetCurrentProtocol(uint8_t pairRef, uint8_t protocol)
{
  uint8_t attrSize;
  uint32_t offset;
  
  if ((pairRef >= nwkcMaxPairingTableEntries) || !isHIDAdapter())
    return HID_ATTR_ILLEGAL_REQ;

  pairRef++;

  if (flushInfo.needFlush && (pairRef != flushInfo.attrRef))
    ZID_AttrFlush();
  
  attrSize = NVM_HID_DEV_ATTR_SIZE;
  offset = NVM_HID_PROXY_0_OFFSET  + ((pairRef-1) * NVM_HID_DEV_ATTR_SIZE); 
  
  if (pairRef != flushInfo.attrRef) {
    halCommonReadFromNvm((uint8_t*)&hidAttribute, offset, attrSize);
    flushInfo.attrRef = pairRef;
  }
  
  /* Device support the boot interface */
  if (hidAttribute.aplHIDDeviceSubclass == 0x01) {
    hidAttribute.aplCurrentProtocol = protocol;
    flushInfo.needFlush = TRUE;
  }

  return HID_ATTR_SUCCESS;
}

/**@brief  This function returns the device idle rate
  *@param  pairRef pairing reference index
  *@param  idleRate contains the device idle rate
  *        - 0x00 = 0.004s
  *        - 0x01 = 0.008s
  *        - ....
  *        - 0xff = 1.020s
  *@retval HID_ATTR_ILLEGAL_REQ or HID_ATTR_SUCCESS;
  */
uint32_t ZID_GetIdleRate(uint8_t pairRef, uint8_t *idleRate)
{
  uint8_t attrSize;
  uint32_t offset;
  
  if ((pairRef >= nwkcMaxPairingTableEntries) || !isHIDAdapter())
    return HID_ATTR_ILLEGAL_REQ;

  pairRef++;

  if (flushInfo.needFlush && (pairRef != flushInfo.attrRef))
    ZID_AttrFlush();
  
  attrSize = NVM_HID_DEV_ATTR_SIZE;
  offset = NVM_HID_PROXY_0_OFFSET  + ((pairRef-1) * NVM_HID_DEV_ATTR_SIZE); 
  
  if (pairRef != flushInfo.attrRef) {
    halCommonReadFromNvm((uint8_t*)&hidAttribute, offset, attrSize);
    flushInfo.attrRef = pairRef;
  }

  *idleRate = hidAttribute.aplDeviceIdleRate;

  return HID_ATTR_SUCCESS;
}

/**@brief  This function sets the device idle rate
  *@param  pairRef pairing reference index
  *@param  idleRate contains the device idle rate
  *        - 0x00 = 0.004s
  *        - 0x01 = 0.008s
  *        - ....
  *        - 0xff = 1.020s
  *@retval HID_ATTR_ILLEGAL_REQ or HID_ATTR_SUCCESS;
  */
uint32_t ZID_SetIdleRate(uint8_t pairRef, uint8_t idleRate)
{
  uint8_t attrSize;
  uint32_t offset;
  
  if ((pairRef >= nwkcMaxPairingTableEntries) || !isHIDAdapter())
    return HID_ATTR_ILLEGAL_REQ;

  pairRef++;

  if (flushInfo.needFlush && (pairRef != flushInfo.attrRef))
    ZID_AttrFlush();
  
  attrSize = NVM_HID_DEV_ATTR_SIZE;
  offset = NVM_HID_PROXY_0_OFFSET  + ((pairRef-1) * NVM_HID_DEV_ATTR_SIZE); 
  
  if (pairRef != flushInfo.attrRef) {
    halCommonReadFromNvm((uint8_t*)&hidAttribute, offset, attrSize);
    flushInfo.attrRef = pairRef;
  }

  hidAttribute.aplDeviceIdleRate = idleRate;
  flushInfo.needFlush = TRUE;

  return HID_ATTR_SUCCESS;
}
#endif


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
