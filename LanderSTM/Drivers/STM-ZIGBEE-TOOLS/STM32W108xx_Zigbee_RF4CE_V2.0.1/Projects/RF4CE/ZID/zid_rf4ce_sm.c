/**
  ******************************************************************************
  * @file    zid_rf4ce_sm.c 
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   zid_rf4ce state machine program body
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
#include <stdio.h>
#include <string.h>
#include "zid_rf4ce.h"
#include "zid_rf4ce_sm.h"
#include "zid_rf4ce_usb.h"
#include "timer.h"


/* Private typedef -----------------------------------------------------------*/
typedef struct queueCmdS {
  /* HID Command code sent*/
  uint8_t cmd;
  /* Start time of the HID command */
  uint32_t startTime;
} queueCmdType;

typedef struct idleContextS {
  /** Flag to signal if the device must stay active
   */
  bool hostDP;
  /** Start Time in active mode
   */
  uint32_t startTimeActiveMode;
  /** Flag to signal that the power save can be enabled
   */
  bool enablePowerSave;
  /** Flag to signal if puts the radio in sleep mode
   *  when the HID Class Device is in Idle state
   */
  bool radioSleepFlag;
  /** Number of message sent pending */
  uint8_t msgSentPending;
} idleContextType;

#ifdef ZID_FULL
typedef struct repeatReportS {
  /* Report Identifier */
  uint8_t id;
  /* Report Data Length */
  uint8_t length;
  /* Report value*/
  uint8_t data[64];
  /* Start time of the report */
  uint32_t startTime;
  /* Device Idle Rate */
  uint8_t idleRate;
  /* Idle Start Time */
  uint32_t startIdleTime;
} repeatReportType;
#endif

/* Private define ------------------------------------------------------------*/
#define ZID_START_TIMEOUT        8000000
#define ZID_COMMAND_CODE         0x0F /* Bits 0-3 Command code */
#define ZID_DATA_PENDING         0x80 /* HID data pending field  enabled */
#define ZID_GDP_COMMAND_FRAME    0x40 /* GDP Command Frame */
#define ZID_VENDOR_SPECIFIC_DATA 0x04 /* Vendor Specific Data */

#define ZID_DEV_ATTR_COLLECTION                    0x00
#define ZID_DEV_NON_STD_RPT_COLLECTION             0x01
#define ZID_DEV_NULL_REPORT_COLLECTION             0x02
#define ZID_SEND_CONFIGURATION_COMPLETE_COLLECTION 0x03
#define ZID_CONFIG_DONE_COLLECTION                 0x04            


#ifdef ZID_FULL
#define aplNullReportSpecList_ID 0xff
#define NUMBER_PROXY_TABLE_ENTRY 15
#define HID_PROXY_RESET_TIMEOUT  3000000 /* 3 sec */
#endif

/* Private macro -------------------------------------------------------------*/
#ifdef TRACE
#define DEBUG_MESSAGE(a) hidDebugMessage a
#else
#define DEBUG_MESSAGE(a)
#endif

#define SUBSTATE_TRANSITION(prefix, prevState, nextState)  \
  { if  (prefix##_##prevState != prefix##_##nextState)	   \
      { \
	DEBUG_MESSAGE(("ZID RF4CE State transition from %08x to %08x\r\n", prefix##_##prevState, prefix##_##nextState)); \
      } \
    newState = prefix##_##nextState; \
  }

#define STATE_TRANSITION(prefix, prevState, nextState) \
  { if  (prefix##_##prevState != prefix##_##nextState)			\
      {									\
	DEBUG_MESSAGE(("ZID RF4CE State transition from %08x to %08x\r\n", prefix##_##prevState, prefix##_##nextState)); \
      }									\
    newState.state = prefix##_##nextState;				\
    if (prefix##_##prevState != prefix##_##nextState) {			\
      newState.subState = prefix##_##nextState | 0x0001;		\
    }									\
  }

extern HID_StateType HID_State;
#define GET_CURRENT_STATE() HID_State.state
#define GET_CURRENT_SUBSTATE() HID_State.subState

/* Private variables ---------------------------------------------------------*/
static hidGlobalContextType globalContextMemory;
hidGlobalContextType *hidGlobalContext = &globalContextMemory;
static idleContextType idleContext;
static queueCmdType cmdQ[nwkcMaxPairingTableEntries]; /* Command queue to take the timeout */
#ifdef ZID_FULL
static repeatReportType rptToRepeat[nwkcMaxPairingTableEntries]; /* Report to repeat @ idle rate */
#endif

/* Private function prototypes -----------------------------------------------*/
#ifdef TRACE
static void hidDebugMessage(char *fmt, ...);
static char debugMessageBuffer[128];
static void printDebugInfo(NLDE_DATA_INDICATION_Type *param);
#endif
static bool devIsPaired(void);
static void callbackSchedule(NLDE_DATA_INDICATION_Type *param);
static void createCfgAttr(uint8_t zidAttrType, uint8_t *msg, uint8_t *msgLen);
static bool checkCompatibility(uint8_t pairRef, uint8_t *msg, uint8_t msgLen);
static bool cmdRspPending(void); 
static void verifyCmdResponse(uint8_t pairRef, uint8_t cmdCode);
#ifdef ZID_FULL
static void checkRptToRepeat(void);
static bool isValidPairRef(uint8_t pairRef);
static void initProxyTable (hidInfoType *proxyTableAttr);
static GRSP_Status extractHIDAttributes(void);
static bool checkAttr(uint8_t pairRef, hidInfoType *proxyTable);
static void buildAttributeRsp(uint8_t *req, uint8_t reqLen, uint8_t *rsp, uint8_t *rpsLen);
#endif

/* Public variables ----------------------------------------------------------*/
#ifdef TEST
extern bool reportData;
#endif

/* Private functions ---------------------------------------------------------*/

/**@brief  hid debug message.
  *@param  fmt
  *@retval None
  */
#ifdef TRACE
static void hidDebugMessage (char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vsnprintf((char *)&debugMessageBuffer, 112, fmt, ap);
  va_end(ap);
  printf (debugMessageBuffer);
}
#endif

/**@brief  Pends response commande.
  *@param  None
  *@retval boolean value
  */
static bool cmdRspPending(void)
{
  uint8_t i;

  for (i=0; i<nwkcMaxPairingTableEntries; i++) {
    if ((cmdQ[i].cmd != 0) && (TIME_ELAPSED(cmdQ[i].startTime) > aplcMaxResponseWaitTime)) {
      cmdQ[i].cmd = 0;
    }    
  }
  for (i=0; i<nwkcMaxPairingTableEntries; i++) {
    if (cmdQ[i].cmd != 0)
      return TRUE;
  }  

  return FALSE;
}

/**@brief  verifies response command.
  *@param  pairing reference
  *@param  cmdCode
  *@retval None
  */
static void verifyCmdResponse(uint8_t pairRef, uint8_t cmdCode)
{
  uint8_t cmdRsp;
  
  cmdRsp = cmdCode & (ZID_COMMAND_CODE | ZID_GDP_COMMAND_FRAME);
  switch (cmdQ[pairRef].cmd) {
  case HEARTBEAT_CODE | ZID_GDP_COMMAND_FRAME:
    {
      if (((cmdRsp >= (GENERIC_RESPONSE_CODE|ZID_GDP_COMMAND_FRAME)) && 
	   (cmdRsp <= (PUSH_ATTRIBUTES_CODE|ZID_GDP_COMMAND_FRAME))) ||
	  ((cmdRsp >= GET_REPORT_CODE) && (cmdRsp <= SET_REPORT_CODE)))
	cmdQ[pairRef].cmd = 0; /* No Command waiting */
    }
    break;
  case PUSH_ATTRIBUTES_CODE | ZID_GDP_COMMAND_FRAME:
  case SET_REPORT_CODE:
    {
      if (cmdRsp == (GENERIC_RESPONSE_CODE | ZID_GDP_COMMAND_FRAME))
	cmdQ[pairRef].cmd = 0; /* No Command waiting */
    }
    break;
  case GET_ATTRIBUTES_CODE | ZID_GDP_COMMAND_FRAME:
    {
      if (cmdRsp == (GET_ATTRIBUTES_RESPONSE_CODE | ZID_GDP_COMMAND_FRAME))
	cmdQ[pairRef].cmd = 0; /* No Command waiting */
    }
    break;
  case GET_REPORT_CODE:
    {
      if ((cmdRsp == REPORT_DATA_CODE) || (cmdRsp == (GENERIC_RESPONSE_CODE | ZID_GDP_COMMAND_FRAME)))
	cmdQ[pairRef].cmd = 0; /* No Command waiting */
    }
    break;
  }
}

/**@brief  device pairing.
  *@param  None
  *@retval None
  */
static bool devIsPaired(void)
{
  bool res=FALSE;
  PairingTableEntryType pairT;
  NLME_GET_Type par;
  uint8_t i;

  par.NIBAttribute = nwkPairingTable_ID;
  par.NIBAttributeValue = (uint8_t*)&pairT;
  for (i=0; i < nwkcMaxPairingTableEntries; i++) {
    par.NIBAttributeIndex = i;
    NLME_Get(&par);
    if (pairT.status == 0x02) {
      res = TRUE;
      break;
    }
  }
  return res;
}

#ifdef ZID_FULL
/**@brief  initialize proxy table.
  *@param  Proxy table attribute
  *@retval None
  */
static void initProxyTable(hidInfoType *proxyTableAttr)
{
   proxyTableAttr[0].id = aplHIDVendorId_ID;
   proxyTableAttr[0].received = 0;
   proxyTableAttr[1].id = aplHIDProductId_ID;
   proxyTableAttr[1].received = 0;
   proxyTableAttr[2].id = aplHIDDeviceReleaseNumber_ID;
   proxyTableAttr[2].received = 0;
   proxyTableAttr[3].id = aplHIDNumEndpoints_ID;
   proxyTableAttr[3].received = 0;
   proxyTableAttr[4].id = aplHIDDeviceSubclass_ID;
   proxyTableAttr[4].received = 0;
   proxyTableAttr[5].id = aplHIDProtocolCode_ID;
   proxyTableAttr[5].received = 0;
   proxyTableAttr[6].id = aplHIDPollInterval_ID;
   proxyTableAttr[6].received = 0;
   proxyTableAttr[7].id = aplHIDParserVersion_ID;
   proxyTableAttr[7].received = 0;
   proxyTableAttr[8].id = aplHIDCountryCode_ID;
   proxyTableAttr[8].received = 0;
   proxyTableAttr[9].id = aplHIDNumStdDescComps_ID;
   proxyTableAttr[9].received = 0;
   proxyTableAttr[10].id = aplHIDNumNonStdDescComps_ID;
   proxyTableAttr[10].received = 0;
   proxyTableAttr[11].id = aplHIDStdDescCompsList_ID;
   proxyTableAttr[11].received = 0;
   proxyTableAttr[12].id = aplHIDNonStdDescCompSpec_MIN_ID;
   proxyTableAttr[12].received = 0;
   proxyTableAttr[13].id = aplHIDNumNullReports_ID;
   proxyTableAttr[13].received = 0;
   proxyTableAttr[14].id = aplNullReportSpecList_ID;
   proxyTableAttr[14].received = 0;
}

/**@brief  verifies response command.
  *@param  None
  *@retval GRSP Status
  */
static GRSP_Status extractHIDAttributes(void)
{
  configurationContextType *context =  (configurationContextType *) hidGlobalContext;
  uint8_t index=0, i, returnValue=0, *rpt;
  deviceAttributeInfo_Type attr;
  uint8_t length, pairRef, *param, fragment, fragmentLen;
  hidInfoType *proxyTableAttr;
  uint16_t descSize;
  bool attrOk;
  
  attrOk = TRUE;
  returnValue = ZID_SUCCESS;
  index = 0;
  length = context->msgLen -1;
  param = &context->msg[1];
  pairRef = context->pairRef;
  proxyTableAttr = context->proxyTableAttr;

  attr.proxyAttr = TRUE;
  attr.proxyRef = pairRef;
  while(index < length) {
    attr.id = param[index];
    index++;
    attr.length = param[index];
    index++;
    attr.value = &param[index];
    
    /* Check if I receive a non standard descriptor plus other attributes */
    if ((attr.id >= aplHIDNonStdDescCompSpec_MIN_ID) &&
	((attr.length+2) > length)) {
      returnValue = ZID_INVALID_PARAMETER;
      attrOk = FALSE;
    } 
    /* Check If I receive a non std desc with the fragmentation mechanism */
    if ((attr.id >= aplHIDNonStdDescCompSpec_MIN_ID) && attrOk) {
      descSize = (context->msg[4] << 8) + context->msg[5];
      /* memcpy(&descSize, &context->msg[4], 2); ??? To be removed */
      if (descSize > aplcMaxNonStdDescFragmentSize) {
	fragmentLen = attr.length - 5;
	fragment = context->msg[7];
	rpt = &context->msg[8];
      } else {
	fragmentLen = attr.length - 4;
	fragment = 0;
	rpt = &context->msg[7];
      }

      if (fragment == 0) {
	context->descSizeRcv = 4;
	context->fragment = 0;
	context->nonStdDesc[0] = context->msg[3];
	context->nonStdDesc[1] = descSize >> 8;
	context->nonStdDesc[2] = descSize && 0x00ff;
	/* memcpy(&context->nonStdDesc[1], &descSize, 2); ??? to be removed */
	context->nonStdDesc[3] = context->msg[6];
      } 

      if (context->fragment != fragment) {
	returnValue = ZID_MISSING_FRAGMENT;
	attrOk = FALSE;
	context->fragment = 0;
      } else {
	memcpy(&context->nonStdDesc[context->descSizeRcv], rpt, fragmentLen);
	context->descSizeRcv += fragmentLen;
	context->fragment++;
      }
      if ((context->descSizeRcv-4) != descSize) 
	attrOk = FALSE;
      else
	attr.value = context->nonStdDesc;
    }
    
    if (attrOk) {
      if (ZID_SetDeviceAttr(&attr) != HID_ATTR_SUCCESS) {
	returnValue = ZID_INVALID_PARAMETER;
	attrOk = FALSE;
      }
    }

    if (attr.id > aplHIDNonStdDescCompSpec_MIN_ID) {
      proxyTableAttr[12].received++;
    } else {
      for (i=0; ((i<NUMBER_PROXY_TABLE_ENTRY) && attrOk); i++) {
	if (proxyTableAttr[i].id == attr.id) {
	  if (attr.id == aplHIDStdDescCompsList_ID)
	    proxyTableAttr[i].received += attr.length;
	  else 
	    proxyTableAttr[i].received++;
	}
      }
    }
    index += attr.length;
  }

  for (i = 0; i<NUMBER_PROXY_TABLE_ENTRY; i++)
    DEBUG_MESSAGE(("ID = 0x%02x value = %d\r\n", proxyTableAttr[i].id, proxyTableAttr[i].received));
  
  return (GRSP_Status)returnValue;
}

/**@brief  verifies response command.
  *@param  pairing reference
  *@param  hid info type
  *@param  pairing referenceproxy table
  *@retval boolean value
  */
static bool checkAttr(uint8_t pairRef, hidInfoType *proxyTable)
{
  uint8_t numDesc, i, numNullRpt;
  deviceAttributeInfo_Type attr;

  /* Check for the HID attr */
  for (i=0; i<9; i++) {
    if (proxyTable[i].received == 0)
      return FALSE;
  }

  /* Check for std desc */
  attr.id = aplHIDNumStdDescComps_ID;
  attr.proxyAttr = TRUE;
  attr.proxyRef = pairRef;
  attr.value = &numDesc;
  ZID_GetDeviceAttr(&attr);
  if ((numDesc != 0) && (proxyTable[11].received != numDesc)) {
    return FALSE;
  }

  /* Check for non std desc */
  attr.id = aplHIDNumNonStdDescComps_ID;
  attr.proxyAttr = TRUE;
  attr.proxyRef = pairRef;
  attr.value = &numDesc;
  ZID_GetDeviceAttr(&attr);
  if ((numDesc != 0) && (proxyTable[12].received != numDesc)) {
    return FALSE;
  }

  attr.id = aplHIDNumNullReports_ID;
  attr.proxyAttr = TRUE;
  attr.proxyRef = pairRef;
  attr.value = &numNullRpt;
  ZID_GetDeviceAttr(&attr);
  if ((numNullRpt != 0) && (proxyTable[14].received != numNullRpt)) {
    return FALSE;
  }
  

  return TRUE;
}

/**@brief  verifies response command.
  *@param  request
  *@param  request lenght
  *@param  response
  *@param  response lenght
  *@retval None
  */  
static void buildAttributeRsp(uint8_t *req, uint8_t reqLen, uint8_t *rsp, uint8_t *rspLen)
{
  uint8_t i, index;
  deviceAttributeInfo_Type attr;
  uint16_t attrValue;
  ATTR_Status status;

  index = 0;
  attr.proxyAttr = FALSE;
  attr.value = (uint8_t*)&attrValue;
  for (i=0; i<reqLen; i++) {
    attr.id = req[i];
    status = ZID_GetDeviceAttr(&attr);
    rsp[index++] = attr.id;
    rsp[index++] = status;
    if(status == HID_ATTR_SUCCESS) {
      rsp[index++] = attr.length;
      memcpy(&rsp[index], (uint8_t*)&attrValue, attr.length);
      index += attr.length;
    }
  }
  *rspLen = index;
}

/**@brief  validating pairing reference.
  *@param  pairing reference
  *@retval boolean value
  */
static bool isValidPairRef(uint8_t pairRef)
{
  PairingTableEntryType pairT;
  NLME_GET_Type par;

  par.NIBAttribute = nwkPairingTable_ID;
  par.NIBAttributeValue = (uint8_t*)&pairT;
  par.NIBAttributeIndex = pairRef;
  NLME_Get(&par);
  if (pairT.status == 0x02) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**@brief  check the report to repeat.
  *@param  None
  *@retval None
  */
static void checkRptToRepeat(void)
{
  uint8_t i;
  ATTR_Status status;
  
  for (i=0; i<nwkcMaxPairingTableEntries; i++) {
    if (rptToRepeat[i].id != 0) {
      uint8_t nullRpt[16];
      uint8_t len;
      if ((rptToRepeat[i].idleRate != 0) &&
	  (TIME_ELAPSED(rptToRepeat[i].startIdleTime) >= (4000*rptToRepeat[i].idleRate))) {
	HID_ClassDriver_USBRptDataNotify(i, rptToRepeat[i].id, rptToRepeat[i].data, rptToRepeat[i].length);
	rptToRepeat[i].startIdleTime = TIME_CurrentTime();
      }
      if (TIME_ELAPSED(rptToRepeat[i].startTime) >= aplcIdleRateGuardTime) {
	memset(nullRpt, 0, sizeof(nullRpt));
	status = HID_ATTR_SUCCESS;
	switch (rptToRepeat[i].id) {
	case HID_MOUSE_RPTDESC:
	  len = 3;
	  break;
	case HID_KEYBOARD_RPTDESC:
	  len = 8;
	  break;
	default:
	  status = ZID_GetNullReport(i, rptToRepeat[i].id, nullRpt, &len);
	}
	if (status == HID_ATTR_SUCCESS)
	  HID_ClassDriver_USBRptDataNotify(i, rptToRepeat[i].id, nullRpt, len);
	rptToRepeat[i].id = 0;
      }
    }
  }
}
#endif

/**@brief  creates configuration attribute.
  *@param  zid attribute type
  *@param  message
  *@param  message lenght
  *@retval None
  */
static void createCfgAttr(uint8_t zidAttrType, uint8_t *msg, uint8_t *msgLen)
{
  configurationContextType *context =  (configurationContextType *) hidGlobalContext;
  uint8_t attr[] = { aplHIDVendorId_ID,            aplHIDProductId_ID,
		aplHIDDeviceReleaseNumber_ID, aplHIDNumEndpoints_ID,
		aplHIDDeviceSubclass_ID,      aplHIDProtocolCode_ID,
		aplHIDPollInterval_ID,        aplHIDParserVersion_ID,
		aplHIDCountryCode_ID,         aplHIDNumStdDescComps_ID,
		aplHIDNumNonStdDescComps_ID,  aplHIDNumNullReports_ID,
		aplHIDStdDescCompsList_ID};
  deviceAttributeInfo_Type deviceAttr;
  uint8_t i, index, attrLen, numDesc;

  /* Collect all the attributes information */
  if (zidAttrType == ZID_DEV_ATTR_COLLECTION) {
    index = 0;
    deviceAttr.id = aplHIDNumStdDescComps_ID;
    deviceAttr.proxyAttr = FALSE;
    deviceAttr.value = &numDesc;
    ZID_GetDeviceAttr(&deviceAttr);
    if (numDesc == 0)
      attrLen = sizeof(attr) - 1;
    else 
      attrLen = sizeof(attr);
    for (i = 0; i < attrLen; i++) {
      deviceAttr.id = attr[i];
      deviceAttr.proxyAttr = FALSE;
      deviceAttr.value = &msg[index+2];
      if (ZID_GetDeviceAttr(&deviceAttr) == HID_ATTR_SUCCESS) {
	msg[index++] = deviceAttr.id;
	msg[index++] = deviceAttr.length;
	index += deviceAttr.length;
      }
    } /* end for */
    
    *msgLen = index;
  }

  /* Collect non std desc */
  if (zidAttrType == ZID_DEV_NON_STD_RPT_COLLECTION) {
    index = 0;
    deviceAttr.id = aplHIDNonStdDescCompSpec_MIN_ID + context->nonStdDescSent;
    deviceAttr.proxyAttr = FALSE;
    deviceAttr.value = context->nonStdDesc;
    ZID_GetDeviceAttr(&deviceAttr);
    msg[0] = deviceAttr.id;
    msg[2] = context->nonStdDesc[0];
    memcpy(&msg[3], &context->nonStdDesc[1], 2);
    msg[5] = context->nonStdDesc[3];
    if (context->descSizeRcv == 0) {
      context->descSizeRcv += 4;
    }
    if((deviceAttr.length - 4)> aplcMaxNonStdDescFragmentSize) {
      msg[6] = context->fragment;
      if ((deviceAttr.length-context->descSizeRcv) > aplcMaxNonStdDescFragmentSize)
	index = aplcMaxNonStdDescFragmentSize;
      else
	index = deviceAttr.length - context->descSizeRcv;
      memcpy(&msg[7], &context->nonStdDesc[context->descSizeRcv], index);
      msg[1] = index + 5;
      context->descSizeRcv += index;
#ifdef TEST
      if ((context->fragment == 1) && (msg[5] == 0x81))
	context->fragment = 0x10;
      else
	context->fragment++;
#else
      context->fragment++;
#endif
    } else {
      msg[1] = deviceAttr.length;
      index = deviceAttr.length - 4;
      memcpy(&msg[6], &context->nonStdDesc[context->descSizeRcv], index);
      context->descSizeRcv += index;
    }
    if (context->descSizeRcv == deviceAttr.length) {
      context->rptId[context->nonStdDescSent] =  msg[5];
      context->nonStdDescSent++;
      context->descSizeRcv = 0;
    }
    *msgLen = msg[1] + 2;
  }

  /* Collect null rpt */
  if (zidAttrType == ZID_DEV_NULL_REPORT_COLLECTION) {
    ATTR_Status res;
    *msgLen = 0;
    for (i=0; i<aplcMaxNonStdDescCompsPerHID; i++) {
      if (context->rptId[i] != 0) {
	res = ZID_GetNullReport(context->pairRef, context->rptId[i], msg, &index);
	context->rptId[i] = 0;
	if (res == HID_ATTR_SUCCESS) {
	  context->numNullRptSent++;
	  *msgLen = index;
	  break;
	}
      }
    }
  }
}

/**@brief  checks for the compatibility.
  *@param  pairing reference
  *@param  message
  *@param  message lenght
  *@retval boolean value
  */
static bool checkCompatibility(uint8_t pairRef, uint8_t *msg, uint8_t msgLen)
{
  /* ??? In this version I check only the ZID Profile Version and the HID Parser Version */
  uint8_t index, len;
  deviceAttributeInfo_Type attr;
  uint16_t hostValue, deviceValue;

  index = 0;
  attr.proxyAttr = FALSE;
  attr.value = (uint8_t*)&deviceValue;
  while (index < msgLen) {
    attr.id = msg[index];
    ZID_GetDeviceAttr(&attr);
    index++;
    if (msg[index] == HID_ATTR_SUCCESS) {
      index++;
      len = msg[index];
      index++;
      hostValue = 0;
      memcpy(&hostValue, &msg[index], len);
      index += len;
      if ((attr.id == aplZIDProfileVersion_ID) || (attr.id == aplHIDParserVersion_ID)) {
	if (hostValue != deviceValue) {
	  return FALSE;
	}
      }
    } else {
      return FALSE;
    }
  }
  return TRUE;
}

/**@brief  schedule call back.
  *@param  NLDE data indication type
  *@retval None
  */
static void callbackSchedule(NLDE_DATA_INDICATION_Type *param)
{
  uint8_t commandCode;
  bool enableRcv, isGDPCmd, security;

  commandCode = param->nsdu[0] & ZID_COMMAND_CODE;
  enableRcv = param->nsdu[0] & ZID_DATA_PENDING;
  isGDPCmd = param->nsdu[0] & ZID_GDP_COMMAND_FRAME;
  security = param->RxFlags & 0x02;

  if (isGDPCmd) {
    switch(commandCode) {
    case GENERIC_RESPONSE_CODE:
      {
	GenericResponse_Type genRsp;
	
	if (!security)
	  return;

	genRsp.pairRef = param->PairingRef;
	genRsp.dataPending = enableRcv;
	genRsp.rspCode = (GRSP_Status) param->nsdu[1];
	GDP_GenericResponseHandler(&genRsp);
      }
      break;
#ifdef ZID_FULL
    case CONFIGURATION_COMPLETE_CODE:
      {
	ConfigurationComplete_Type cfgEnd;

	if (!security)
	  return;
	
	cfgEnd.pairRef = param->PairingRef;
	cfgEnd.status = ZID_SUCCESS;
	
	GDP_ConfigurationCompleteHandler(&cfgEnd);
      }
      break;
    case HEARTBEAT_CODE:
      {
	HeartBeat_Type attr;

	if (!security)
	  return;
	if (idleContext.hostDP) {
	  idleContext.hostDP = FALSE;
	  attr.pairRef = param->PairingRef;
	  GDP_HeartBeatHandler(&attr);
	} else {
	  GenericResponse_Type genRsp;
	  genRsp.pairRef = param->PairingRef;
	  genRsp.dataPending = FALSE;
	  genRsp.rspCode = ZID_SUCCESS;
	  GDP_GenericResponse(&genRsp);
	}
      }
      break;
#endif
    case GET_ATTRIBUTES_CODE:
      {
	GetAttributes_Type attr;

	if (!security)
	  return;
	
	attr.pairRef = param->PairingRef;
	attr.dataPending = enableRcv;
	attr.attrListLength = param->nsduLength - 1;
	attr.attrId = &param->nsdu[1];
	GDP_GetAttributesHandler(&attr);
      }
      break;
    case GET_ATTRIBUTES_RESPONSE_CODE:
      {
	GetAttributesResponse_Type attr;

	if (!security)
	  return;
	
	attr.pairRef = param->PairingRef;
	attr.dataPending = enableRcv;
	attr.statusRecLength = param->nsduLength - 1;
	attr.statusRec = &param->nsdu[1];
	GDP_GetAttributesResponseHandler(&attr);
      }
      break;
#ifdef ZID_FULL
    case PUSH_ATTRIBUTES_CODE:
      {
	PushAttributes_Type attr;

	if (!security)
	  return;
	
	attr.pairRef = param->PairingRef;
	attr.attributesRecLength = param->nsduLength - 1;
	attr.attributesRec = &param->nsdu[1];
	GDP_PushAttributesHandler(&attr);
      }
      break;
#endif
    default:
      // Error Command Code not supported
      DEBUG_MESSAGE(("callbackSchedule() error: GDP Command Code 0x%02x not supported\r\n", commandCode));
      break;
    }
  } else {
    switch(commandCode) {
    case GET_REPORT_CODE:
      {
	GetReport_Type rpt;
	
#ifdef ZID_FULL
	if (isHIDAdapter())
	  return;
#endif	

	if (!security)
	  return;

	rpt.pairRef = param->PairingRef;
	rpt.dataPending = enableRcv;
	rpt.reportType = param->nsdu[1];
	rpt.reportId = param->nsdu[2];
	ZID_GetReportHandler(&rpt);
      }
      break;
    case REPORT_DATA_CODE:
      {
	ReportData_Type rpt;
	
	rpt.txOptions = 0;
	if (security)
	  rpt.txOptions = TX_SECURITY_ENABLED;
	rpt.pairRef = param->PairingRef;
	rpt.dataPending = enableRcv;
	rpt.reportPayloadLength = param->nsduLength - 1;
	rpt.reportDataRecords = &param->nsdu[1];
	ZID_ReportDataHandler(&rpt);
      }
      break;
    case SET_REPORT_CODE:
      {
	SetReport_Type rptParam;
	
#ifdef ZID_FULL
	if (isHIDAdapter())
	  return;
#endif

	if (!security)
	  return;

	rptParam.pairRef = param->PairingRef;
	rptParam.dataPending = enableRcv;
	rptParam.reportType = param->nsdu[1];
	rptParam.reportId = param->nsdu[2];
	rptParam.reportDataLen = param->nsduLength - 3;
	rptParam.reportData = (uint8_t*)&param->nsdu[3];
	ZID_SetReportHandler(&rptParam);
      }
      break;
    default:
      /* Error Command Code not supported */
      DEBUG_MESSAGE(("callbackSchedule() error: ZID Command Code 0x%02x not supported\r\n", commandCode));
      break;
    }
  }
}


/* Public functions ----------------------------------------------------------*/

/**@brief  Start State machine.
  *@param  sub state
  *@retval unsigned value
  */
uint32_t startStateMachine(uint32_t subState)
{
  uint32_t newState = subState;
  startContextType *context =  (startContextType *) hidGlobalContext;
  
  switch (subState) {
  case HID_SUBSTATE_START_INIT:
    memset(cmdQ, 0, sizeof(cmdQ)); /* Queue used to store the start time of the HID command to wait the timeout */
    if (context->status == SUCCESS) {
      context->startComplete = TRUE;
      ZID_DeviceInitCallback(&context->status);
      SUBSTATE_TRANSITION(HID_SUBSTATE_START, INIT, DONE); /* Device started */
    } else {
      SUBSTATE_TRANSITION(HID_SUBSTATE_START, INIT, WAITING); /* Waiting start response */
      context->startTime = TIME_CurrentTime();
    }
    break;
  case HID_SUBSTATE_START_WAITING:
    if ((context->startComplete) || 
	(TIME_ELAPSED(context->startTime) >= ZID_START_TIMEOUT)) {
      SUBSTATE_TRANSITION(HID_SUBSTATE_START, WAITING, DONE); /* Device started || Timeout */
    } else {
      SUBSTATE_TRANSITION(HID_SUBSTATE_START, WAITING, WAITING); /* Wait start complete */
    }
    break;
  case HID_SUBSTATE_START_DONE:
    /* End state do nothing */
    break;
  default:
    /* Invalid sub state */
    break;
  }

  return newState;
}

/**@brief  Pairing State machine .
  *@param  sub state
  *@retval unsigned value
  */
uint32_t pairingStateMachine(uint32_t subState)
{
   uint32_t newState = subState;
  pairingContextType *context =  (pairingContextType *) hidGlobalContext;
  
  switch (subState) {
  case HID_SUBSTATE_PAIRING_INIT:
    context->pairingStatus = SUCCESS;
    context->dscConf = FALSE;
    context->autoDscConf = FALSE;
    context->pairConf = FALSE;
    context->pairInd = FALSE;
    context->commInd = FALSE;
#ifdef ZID_FULL
    if (isHIDAdapter() && !context->param.pairTT) {
      SUBSTATE_TRANSITION(HID_SUBSTATE_PAIRING, INIT, AUTODSC_REQ); // HID HOST device
    } else {
#endif
      SUBSTATE_TRANSITION(HID_SUBSTATE_PAIRING, INIT, DSC_REQ); // HID device
#ifdef ZID_FULL
    }
#endif
    break;
  case HID_SUBSTATE_PAIRING_DSC_REQ:
    {
      uint32_t status;
      NLME_DISCOVERY_REQUEST_Type dscParam;
      dscParam.DstPANId = 0xffff;
      dscParam.DstNwkAddr = 0xffff;
      dscParam.OrgAppCapabilities = 0x13; //User string - 1 Dev supported - 1 Profile Supported
      dscParam.OrgDevTypeList[0] = context->param.devType;
      dscParam.OrgProfileIdList[0] = 0x02; //ZigBee Input Device (ZID)
      dscParam.SearchDevType = context->param.searchDevType;
      dscParam.DiscProfileIdListSize = 1;
      dscParam.DiscProfileIdList[0] = 0x02; //ZigBee Input Device (ZID)
      dscParam.DiscDuration = 0x00186a; //100ms
      status = NLME_DISCOVERY_request(&dscParam);
      if (status != RF4CE_SAP_PENDING) {
	ZID_PairingCallback_Type pairCallback;
	NLME_DISCOVERY_CONFIRM_Type *dscConf = &(context->dscConfirm);

	pairCallback.rf4cePrimitive = ZID_DSC_CONFIRM;
	dscConf->Status = status;
	pairCallback.Pairing.dscConfirm = dscConf;
	ZID_PairingCallback(&pairCallback);
	SUBSTATE_TRANSITION(HID_SUBSTATE_PAIRING, DSC_REQ, DONE); // RF4CE busy
      } else {
	SUBSTATE_TRANSITION(HID_SUBSTATE_PAIRING, DSC_REQ, DSC_WAITING_CONF); // Dsc start
      }
    }
    break;
  case HID_SUBSTATE_PAIRING_DSC_WAITING_CONF:
    if (context->dscConf) {
      if (context->pairingStatus != SUCCESS) {
	SUBSTATE_TRANSITION(HID_SUBSTATE_PAIRING, DSC_WAITING_CONF, DONE); // Dsc Error
      } else {
	// No error start Pair procedure
	SUBSTATE_TRANSITION(HID_SUBSTATE_PAIRING, DSC_WAITING_CONF, PAIR_REQ); // Dsc Done
      }
    } else {
	SUBSTATE_TRANSITION(HID_SUBSTATE_PAIRING, DSC_WAITING_CONF, DSC_WAITING_CONF); // Wait Dsc Confirm
    }
    break;
  case HID_SUBSTATE_PAIRING_PAIR_REQ:
    {
      uint32_t status;
      NLME_PAIR_REQUEST_Type param;
      NLME_DISCOVERY_CONFIRM_Type *dscConfInfo = &(context->dscConfirm);
      deviceAttributeInfo_Type deviceAttr;
      uint8_t numberOfKey;;

      // Collect number of key to exchange attribute
      deviceAttr.id = aplKeyExchangeTransferCount_ID;
      deviceAttr.proxyAttr = FALSE;
      deviceAttr.value = &numberOfKey;
      if (ZID_GetDeviceAttr(&deviceAttr) == HID_ATTR_ILLEGAL_REQ) {
	numberOfKey = aplcMinKeyExchangeTransferCount;
      }
      
      param.LogicalChannel = dscConfInfo->NodeDescList[0].LogicalChannel;
      param.DstPANId = dscConfInfo->NodeDescList[0].PANId;
      memcpy(param.DstIEEEAddr, dscConfInfo->NodeDescList[0].longAddr, sizeof(IEEEAddr));
      param.OrgAppCapabilities = 0x13;
      param.OrgDevTypeList[0] = context->param.devType;
      param.OrgProfileIdList[0] =  0x02; //ZigBee Input Device (ZID)
      param.KeyExTransferCount = numberOfKey;
      
      status = NLME_PAIR_request(&param);
      if (status != RF4CE_SAP_PENDING) {
	ZID_PairingCallback_Type pairCallback;
	NLME_PAIR_CONFIRM_Type pairConf;

	pairCallback.rf4cePrimitive = ZID_PAIR_CONFIRM;
	pairConf.Status = status;
	pairCallback.Pairing.pairConfirm = &pairConf;
	ZID_PairingCallback(&pairCallback);
	SUBSTATE_TRANSITION(HID_SUBSTATE_PAIRING, PAIR_REQ, DONE); // RF4CE busy
      } else {
	SUBSTATE_TRANSITION(HID_SUBSTATE_PAIRING, PAIR_REQ, PAIR_WAITING_CONF); // Pair start
      }
    }
    break;
  case HID_SUBSTATE_PAIRING_PAIR_WAITING_CONF:
    if (context->pairConf) {
      SUBSTATE_TRANSITION(HID_SUBSTATE_PAIRING, PAIR_WAITING_CONF, DONE); // Pair Done || Error
    } else {
      SUBSTATE_TRANSITION(HID_SUBSTATE_PAIRING, PAIR_WAITING_CONF, PAIR_WAITING_CONF); // Wait Pair Confirm
    }
    break;
#ifdef ZID_FULL
  case HID_SUBSTATE_PAIRING_AUTODSC_REQ:
    {
      uint32_t status;
      NLME_AUTO_DISCOVERY_REQUEST_Type dscParam;

      dscParam.RecAppCapabilities = 0x13; //User string - 1 Dev supported - 1 Profile Supported
      dscParam.RecDevTypeList[0] = context->param.devType; 
      dscParam.RecProfileIdList[0] = 0x02; //ZigBee Input Device (ZID)
      dscParam.AutoDiscDuration = 0x1c9c38; //30 sec
      status = NLME_AUTO_DISCOVERY_request(&dscParam);
      if (status != RF4CE_SAP_PENDING) {
	ZID_PairingCallback_Type pairCallback;
	NLME_AUTO_DISCOVERY_CONFIRM_Type dscConf; 

	pairCallback.rf4cePrimitive = ZID_AUTO_DSC_CONFIRM;
	dscConf.Status = status;
	pairCallback.Pairing.autoDscConfirm = &dscConf;
	ZID_PairingCallback(&pairCallback);
	SUBSTATE_TRANSITION(HID_SUBSTATE_PAIRING, AUTODSC_REQ, DONE); // RF4CE busy
      } else {
	SUBSTATE_TRANSITION(HID_SUBSTATE_PAIRING, AUTODSC_REQ, AUTODSC_WAITING_CONF); // Wait AutoDsc Confirm
      }
    }
    break;
  case HID_SUBSTATE_PAIRING_AUTODSC_WAITING_CONF:
    if (context->autoDscConf) {
      if (context->pairingStatus != SUCCESS) {
	SUBSTATE_TRANSITION(HID_SUBSTATE_PAIRING, AUTODSC_WAITING_CONF, DONE); // AutoDsc Error
      } else {
	SUBSTATE_TRANSITION(HID_SUBSTATE_PAIRING, AUTODSC_WAITING_CONF, PAIR_IND); // Start Pair Procedure
	context->startTime = TIME_CurrentTime(); // Timeout to exit if the pair indication dosen't arrive.
      }
    } else {
      SUBSTATE_TRANSITION(HID_SUBSTATE_PAIRING, AUTODSC_WAITING_CONF, AUTODSC_WAITING_CONF); // Wait AutoDsc Confirm
    }
    break;
  case HID_SUBSTATE_PAIRING_PAIR_IND:
    if ((TIME_ELAPSED(context->startTime) <= aplcMaxPairIndicationWaitTime) && 
	(!context->pairInd)) {
      SUBSTATE_TRANSITION(HID_SUBSTATE_PAIRING, PAIR_IND, PAIR_IND); // Wait Pair Indication
    } else {
      if (context->pairInd) {
	SUBSTATE_TRANSITION(HID_SUBSTATE_PAIRING, PAIR_IND, PAIR_RSP); // Pair Ind arrived
      } else {
	context->pairingStatus = ZID_NO_PAIR_INDICATION;
	SUBSTATE_TRANSITION(HID_SUBSTATE_PAIRING, PAIR_IND, DONE); // Pair Indication timeout || Error
      }
    }
    break;
  case HID_SUBSTATE_PAIRING_PAIR_RSP:
    {
      uint32_t status=SUCCESS;
      NLME_PAIR_RESPONSE_Type param;

      // ??? Need to port the RF4CE stack to the new version about the indication of the number of key ???

      if ((context->pairingStatus == SUCCESS) || 
	  (context->pairingStatus == DUPLICATE_PAIRING)) {
	param.Status = SUCCESS; 
      } else {
	param.Status = NO_REC_CAPACITY; 
      }
      param.DstPANId = context->pairIndication.SrcPANId;
      memcpy(param.DstIEEEAddr, context->pairIndication.SrcIEEEAddr, sizeof(IEEEAddr));
      param.RecAppCapabilities = 0x13; //User string - 1 Dev supported - 1 Profile Supported
      param.RecDevTypeList[0] = context->param.devType;
      param.RecProfileIdList[0] = 0x02; //ZigBee Input Device (ZID)
      param.ProvPairingRef = context->pairIndication.ProvPairingRef;
      status = NLME_PAIR_response(&param);
      if (status != RF4CE_SAP_PENDING) {
	ZID_PairingCallback_Type pairCallback;
	NLME_COMM_STATUS_INDICATION_Type commIndInfo; 

	pairCallback.rf4cePrimitive = ZID_COMM_STATUS_INDICATION;
	commIndInfo.Status = status;
	pairCallback.Pairing.commStatusIndication = &commIndInfo;
	ZID_PairingCallback(&pairCallback);
	SUBSTATE_TRANSITION(HID_SUBSTATE_PAIRING, PAIR_RSP, DONE); // Pair response Error
      } else {
	SUBSTATE_TRANSITION(HID_SUBSTATE_PAIRING, PAIR_RSP, WAITING_COMM_IND); // Wait comm status ind
      }
    }
    break;
  case HID_SUBSTATE_PAIRING_WAITING_COMM_IND:
    if (context->commInd) {
      SUBSTATE_TRANSITION(HID_SUBSTATE_PAIRING, WAITING_COMM_IND, DONE);  // Device Paired || Error
    } else {
      SUBSTATE_TRANSITION(HID_SUBSTATE_PAIRING, WAITING_COMM_IND, WAITING_COMM_IND); // Wait comm ind
    }
    break;
#endif
  case HID_SUBSTATE_PAIRING_DONE:
     /* End state do nothing */
    break;
  default:
    /* Invalid SubState */
    break;
  }

  return newState;
}

/**@brief  configure State machine.
  *@param  sub state
  *@retval unsigned value
  */
uint32_t configurationStateMachine(uint32_t subState)
{
  uint32_t newState = subState;
  configurationContextType *context =  (configurationContextType *) hidGlobalContext;
  
  switch (subState) {
  case HID_SUBSTATE_CONFIGURATION_INIT:
    {
      context->startTime = TIME_CurrentTime();
      context->fragment = 0;
      context->configurationSuccess = FALSE;
      context->zidAttrType = ZID_DEV_ATTR_COLLECTION;
      context->nonStdDescSent = 0;
      context->unpairSignal = FALSE;
      context->descSizeRcv = 0;
      context->numNullRptSent = 0;
      memset(context->rptId, 0, aplcMaxNonStdDescCompsPerHID);

#ifdef ZID_FULL
      if (isHIDAdapter()) {
	initProxyTable(context->proxyTableAttr);
	SUBSTATE_TRANSITION(HID_SUBSTATE_CONFIGURATION, INIT, WAIT_GET_ATTR); // HID host wait Attribute
      } else {
#endif
	deviceAttributeInfo_Type devAttr;
	devAttr.id = aplHIDNumNonStdDescComps_ID;
	devAttr.proxyAttr = FALSE;
	devAttr.value = &context->numNonStdDesc;
	ZID_GetDeviceAttr(&devAttr);
	devAttr.id = aplHIDNumNullReports_ID;
	devAttr.value = &context->numNullRpt;
	ZID_GetDeviceAttr(&devAttr);
	SUBSTATE_TRANSITION(HID_SUBSTATE_CONFIGURATION, INIT, SEND_GET_ATTR); // HID device send attribute
#ifdef ZID_FULL
      }
#endif
    }
    break;
#ifdef ZID_FULL
  case HID_SUBSTATE_CONFIGURATION_WAIT_GET_ATTR:
    {
      if ((!context->msgRcv) && (TIME_ELAPSED(context->startTime) < aplcMaxConfigWaitTime)) {
	SUBSTATE_TRANSITION(HID_SUBSTATE_CONFIGURATION, WAIT_GET_ATTR, WAIT_GET_ATTR); // Wait Get Attribute
      } else {
	if (context->msgRcv)  {
	  uint8_t commandCode;
	  context->msgRcv = FALSE;
	  commandCode = context->msg[0] & ZID_COMMAND_CODE;
	  if (commandCode == GET_ATTRIBUTES_CODE) {
	    SUBSTATE_TRANSITION(HID_SUBSTATE_CONFIGURATION, WAIT_GET_ATTR, SEND_GET_ATTR_RSP); // Get attr received
	  }
	} else {
	  SUBSTATE_TRANSITION(HID_SUBSTATE_CONFIGURATION, WAIT_GET_ATTR, UNPAIR_DEVICE); // Get attr not received
	}
      }
    }
    break;
  case HID_SUBSTATE_CONFIGURATION_SEND_GET_ATTR_RSP:
    {
      uint8_t rspLen;
      uint8_t rsp[128];
      GetAttributesResponse_Type attrRsp;
      uint32_t status;
      
      buildAttributeRsp(&context->msg[1], (context->msgLen-1), rsp, &rspLen);
      attrRsp.pairRef = context->pairRef;
      attrRsp.statusRecLength = rspLen;
      attrRsp.statusRec = rsp;
      attrRsp.dataPending = FALSE;
      status = GDP_GetAttributesResponse(&attrRsp);

      if (status != RF4CE_SAP_PENDING) {
	SUBSTATE_TRANSITION(HID_SUBSTATE_CONFIGURATION, SEND_GET_ATTR_RSP, UNPAIR_DEVICE); // Error sending Attr Rsp
      } else {
	SUBSTATE_TRANSITION(HID_SUBSTATE_CONFIGURATION, SEND_GET_ATTR_RSP, WAIT_HID_ATTR); // Attr Rsp sent
	context->startTime = TIME_CurrentTime();  
      }
    }
    break;
  case HID_SUBSTATE_CONFIGURATION_WAIT_HID_ATTR:
    {
      GenericResponse_Type rsp;
      uint32_t status=SUCCESS;
      uint8_t commandCode;
      
      // Wait Push attributes command frame
      if ((!context->msgRcv) &&
	  (TIME_ELAPSED(context->startTime) < aplcMaxConfigWaitTime)) {
	SUBSTATE_TRANSITION(HID_SUBSTATE_CONFIGURATION, WAIT_HID_ATTR, WAIT_HID_ATTR); // Wait push attribute
      } else {
	if (context->msgRcv) {
	  context->msgRcv = FALSE;
	  commandCode = context->msg[0] & ZID_COMMAND_CODE;
	  // Configuration complete command received
	  if (commandCode == CONFIGURATION_COMPLETE_CODE) {
	    SUBSTATE_TRANSITION(HID_SUBSTATE_CONFIGURATION, WAIT_HID_ATTR, CHECK_ATTR); // Config Comp received
	  } else {
	    // Push attributes command received
	    if (commandCode == PUSH_ATTRIBUTES_CODE) {
	      rsp.rspCode = extractHIDAttributes();
	    }
	    // Set report command received 
	    if ((commandCode == SET_REPORT_CODE) && (context->msg[1] == 0x01)) {
	      uint8_t nullRpt[18];
	      nullRpt[0] = context->msg[2];
	      nullRpt[1] = context->msgLen - 3;
	      rsp.rspCode = ZID_INVALID_REPORT_ID;
	      if (nullRpt[1] <= aplcMaxNullReportSize) {
		memcpy(&nullRpt[2], &context->msg[3], nullRpt[1]);
		if (ZID_SetNullReport(context->pairRef, nullRpt, (nullRpt[1]+2)) == HID_ATTR_SUCCESS) {
		  rsp.rspCode = ZID_SUCCESS;
		  context->proxyTableAttr[14].received++;
		}
	      }
	    }
	    rsp.pairRef = context->pairRef;
	    rsp.dataPending = FALSE;
	    status = GDP_GenericResponse(&rsp);
	    context->startTime = TIME_CurrentTime(); 
	    if (status != RF4CE_SAP_PENDING) {
	      SUBSTATE_TRANSITION(HID_SUBSTATE_CONFIGURATION, WAIT_HID_ATTR, UNPAIR_DEVICE); // Error sending Gen Rsp
	    }
	  }
	} else {
	  SUBSTATE_TRANSITION(HID_SUBSTATE_CONFIGURATION, WAIT_HID_ATTR, CHECK_ATTR); // Timeout
	}
      }
    }
    break;
  case HID_SUBSTATE_CONFIGURATION_CHECK_ATTR:
    {
      GenericResponse_Type rsp;
      
      if (checkAttr(context->pairRef, context->proxyTableAttr)) { 
	context->configurationSuccess = TRUE;
	rsp.rspCode = ZID_SUCCESS;
	ZID_SetCurrentProtocol(context->pairRef, 1);
	ZID_SetIdleRate(context->pairRef, 0);
      } else {
	context->configurationSuccess = FALSE;
	rsp.rspCode = ZID_CONFIGURATION_FAILURE;
      } 
      rsp.pairRef = context->pairRef;
#ifdef TEST
      rsp.dataPending = TRUE;
#else
      rsp.dataPending = FALSE;
#endif
      context->genRspSent = FALSE;
      context->startTime = TIME_CurrentTime();
      if ((GDP_GenericResponse(&rsp)!= RF4CE_SAP_PENDING) || !context->configurationSuccess) {
	SUBSTATE_TRANSITION(HID_SUBSTATE_CONFIGURATION, CHECK_ATTR, UNPAIR_DEVICE); // Error during configuration
      } else {
	SUBSTATE_TRANSITION(HID_SUBSTATE_CONFIGURATION, CHECK_ATTR, DONE); // End configuration
      }
    }
    break;
  case HID_SUBSTATE_CONFIGURATION_UNPAIR_DEVICE:
    {
      if ((TIME_ELAPSED(context->startTime) < aplcMaxResponseWaitTime) && !context->genRspSent) {
	SUBSTATE_TRANSITION(HID_SUBSTATE_CONFIGURATION, UNPAIR_DEVICE, UNPAIR_DEVICE); // waiting Data confirm of the Genric Response message
      } else {
	if (NLME_UNPAIR_request(context->pairRef) != RF4CE_SAP_PENDING) {
	  PairingTableEntryType pairT;
	  NLME_SET_Type param;
	
	  memset(&pairT, 0xff, sizeof(pairT));
	  param.NIBAttribute = nwkPairingTable_ID;
	  param.NIBAttributeValue = (uint8_t*)&pairT;
	  param.NIBAttributeIndex = context->pairRef;
	  NLME_Set(&param);
	} 
	context->startTime = TIME_CurrentTime();
	SUBSTATE_TRANSITION(HID_SUBSTATE_CONFIGURATION, UNPAIR_DEVICE, WAIT_UNPAIR_CONFIRM); // Unpair request sent
      }
    }
    break;
  case HID_SUBSTATE_CONFIGURATION_WAIT_UNPAIR_CONFIRM:
    {
      if (context->unpairSignal || (TIME_ELAPSED(context->startTime) > aplcMaxResponseWaitTime)) {
	SUBSTATE_TRANSITION(HID_SUBSTATE_CONFIGURATION, WAIT_UNPAIR_CONFIRM, DONE); //Attributes not collected
      } else {
	SUBSTATE_TRANSITION(HID_SUBSTATE_CONFIGURATION, WAIT_UNPAIR_CONFIRM, WAIT_UNPAIR_CONFIRM); // waiting confirm
      }
    }
    break;
#endif    
  case HID_SUBSTATE_CONFIGURATION_SEND_GET_ATTR:
    {
      uint8_t attr[] = {aplZIDProfileVersion_ID, aplHIDParserVersion_ID, aplHIDCountryCode_ID, 
		   aplHIDDeviceReleaseNumber_ID, aplHIDVendorId_ID, aplHIDProductId_ID};
      GetAttributes_Type getAttr;
      uint32_t status;
      bool timeout = FALSE;

      if (TIME_ELAPSED(context->startTime) > aplcMaxConfigPrepTime) {
	timeout = TRUE;
      } else {
	context->msgRcv = FALSE;
	getAttr.pairRef = context->pairRef;
	getAttr.attrId = attr;
	getAttr.attrListLength = sizeof(attr);
	getAttr.dataPending = FALSE;
	status = GDP_GetAttributes(&getAttr);
      }
      if (timeout || (status != RF4CE_SAP_PENDING)) {
	SUBSTATE_TRANSITION(HID_SUBSTATE_CONFIGURATION, SEND_GET_ATTR, UNPAIR_HOST); //Time Out || Error
      } else {
	context->startTime = TIME_CurrentTime();
	SUBSTATE_TRANSITION(HID_SUBSTATE_CONFIGURATION, SEND_GET_ATTR, WAIT_ATTR_RSP); //Time Out Error
      }
    }
    break;
  case HID_SUBSTATE_CONFIGURATION_WAIT_ATTR_RSP:
    {
      bool hostCompatible = FALSE;
      uint8_t commandCode;

      if (!context->msgRcv && (TIME_ELAPSED(context->startTime) < aplcMaxResponseWaitTime)) {
	SUBSTATE_TRANSITION(HID_SUBSTATE_CONFIGURATION, WAIT_ATTR_RSP, WAIT_ATTR_RSP); //Wait Attr Rsp
      } else {
	if (context->msgRcv) {
	  commandCode = context->msg[0] & ZID_COMMAND_CODE;
	  if (commandCode == GET_ATTRIBUTES_RESPONSE_CODE) {
	    context->startTime = TIME_CurrentTime();
	    hostCompatible = checkCompatibility(context->pairRef, &context->msg[1], context->msgLen-1);
	  }
	}
	if (!hostCompatible || !context->msgRcv) {
	  SUBSTATE_TRANSITION(HID_SUBSTATE_CONFIGURATION, WAIT_ATTR_RSP, UNPAIR_HOST); // Host not compatible || not answer
	} else {		  
	  SUBSTATE_TRANSITION(HID_SUBSTATE_CONFIGURATION, WAIT_ATTR_RSP, PUSH_ATTR); // Host Compatible
	}
      }
    }
    break;
  case HID_SUBSTATE_CONFIGURATION_PUSH_ATTR:
    {
      PushAttributes_Type pushAttr;
      SetReport_Type rpt;
      uint32_t status = 0;

      context->msgRcv = FALSE;
      createCfgAttr(context->zidAttrType, context->msg, &context->msgLen);
      
      // Send the Push Attributes message
      if (context->zidAttrType < ZID_DEV_NULL_REPORT_COLLECTION) {
	pushAttr.pairRef = context->pairRef;
	pushAttr.attributesRec = context->msg;
	pushAttr.attributesRecLength = context->msgLen; 
	status = GDP_PushAttributes(&pushAttr);
      }

      // Send Set Report Command with NULL Report
      if ((context->zidAttrType == ZID_DEV_NULL_REPORT_COLLECTION) && (context->msgLen != 0)) {
	rpt.dataPending = FALSE;
	rpt.pairRef = context->pairRef;
	rpt.reportType = 0x01;
	rpt.reportId = context->msg[0];
	rpt.reportDataLen = context->msg[1];
	rpt.reportData = &context->msg[2];
	status = ZID_SetReport(&rpt);
      }

      if (context->zidAttrType == ZID_DEV_ATTR_COLLECTION)
	context->zidAttrType++;

      if ((context->zidAttrType == ZID_DEV_NON_STD_RPT_COLLECTION) && (context->nonStdDescSent == context->numNonStdDesc))
	context->zidAttrType++;

      if ((context->zidAttrType == ZID_DEV_NULL_REPORT_COLLECTION) && (context->numNullRptSent == context->numNullRpt))
	context->zidAttrType++;

      if ((TIME_ELAPSED(context->startTime) <= aplcMaxConfigPrepTime) && 
	  (status == RF4CE_SAP_PENDING)) {
	SUBSTATE_TRANSITION(HID_SUBSTATE_CONFIGURATION, PUSH_ATTR, WAIT_GENERIC_RESPONSE); // Push Command Frame sent
	context->startTime = TIME_CurrentTime();
      } else {
	SUBSTATE_TRANSITION(HID_SUBSTATE_CONFIGURATION, PUSH_ATTR, UNPAIR_HOST); // Timeout || Communication error
      }
    }
    break;
  case HID_SUBSTATE_CONFIGURATION_WAIT_GENERIC_RESPONSE:
    {
      bool error=FALSE;

      // Wait Generic Response command frame
      if ((!context->msgRcv) && 
	  (TIME_ELAPSED(context->startTime) < aplcMaxResponseWaitTime)) {
	SUBSTATE_TRANSITION(HID_SUBSTATE_CONFIGURATION, WAIT_GENERIC_RESPONSE, WAIT_GENERIC_RESPONSE); // Wait Generic Response
      } else {
	if (context->msgRcv) {
	  context->msgRcv = FALSE;
	  context->startTime = TIME_CurrentTime();
	  // Configuration Procedure END
	  if ((context->zidAttrType == ZID_CONFIG_DONE_COLLECTION) && (context->msg[1] == ZID_SUCCESS)) {
	    if (context->msg[0] & ZID_DATA_PENDING)
	      ZID_SetHDP(TRUE);
	    context->configurationSuccess = TRUE;
	    SUBSTATE_TRANSITION(HID_SUBSTATE_CONFIGURATION, WAIT_GENERIC_RESPONSE, DONE); // Configuration Success
	  } else {
	    // Send Configuration Complete command
	    if (context->zidAttrType == ZID_SEND_CONFIGURATION_COMPLETE_COLLECTION) {
	      SUBSTATE_TRANSITION(HID_SUBSTATE_CONFIGURATION, WAIT_GENERIC_RESPONSE, SEND_CFG_COMP); // Attr sent with success
	    } else {
	      // Need to send other Non Std Descriptor || Null reports
	      if ((context->zidAttrType == ZID_DEV_NULL_REPORT_COLLECTION) || (context->zidAttrType == ZID_DEV_NON_STD_RPT_COLLECTION)) {
		SUBSTATE_TRANSITION(HID_SUBSTATE_CONFIGURATION, WAIT_GENERIC_RESPONSE, PUSH_ATTR); // Generic Response received
	      } else {
		error = TRUE;
	      }
	    }
	  }
	} else {
	  error = TRUE;
	}
      }
      if (error || context->unpairSignal) {
	SUBSTATE_TRANSITION(HID_SUBSTATE_CONFIGURATION, WAIT_GENERIC_RESPONSE, UNPAIR_HOST); // Configuration Error
      }

    }
    break;
  case HID_SUBSTATE_CONFIGURATION_SEND_CFG_COMP:
    {
      ConfigurationComplete_Type configComplete;
      uint32_t status;

      context->zidAttrType = ZID_CONFIG_DONE_COLLECTION;
      configComplete.pairRef = context->pairRef;
      configComplete.status = ZID_SUCCESS;
      status = GDP_ConfigurationComplete(&configComplete);
      if ((status == RF4CE_SAP_PENDING) && (TIME_ELAPSED(context->startTime) <= aplcMaxConfigPrepTime)) {
	SUBSTATE_TRANSITION(HID_SUBSTATE_CONFIGURATION, SEND_CFG_COMP, WAIT_GENERIC_RESPONSE); // Config cmd sent
	context->startTime = TIME_CurrentTime();
      } else {
	SUBSTATE_TRANSITION(HID_SUBSTATE_CONFIGURATION, SEND_CFG_COMP, UNPAIR_HOST); // Command sent or Error
      }
  }
  break;
  case HID_SUBSTATE_CONFIGURATION_UNPAIR_HOST:
    {
      if (context->unpairSignal) {
	NLME_UNPAIR_response(context->pairRef);
      } else {
	PairingTableEntryType pairT;
	NLME_SET_Type param;
 
	memset(&pairT, 0xff, sizeof(pairT));
	param.NIBAttribute = nwkPairingTable_ID;
	param.NIBAttributeValue = (uint8_t*)&pairT;
	param.NIBAttributeIndex = context->pairRef;
	NLME_Set(&param);
       }
      SUBSTATE_TRANSITION(HID_SUBSTATE_CONFIGURATION, UNPAIR_HOST, DONE); // Pairing entry deleted
    }
    break;
  case HID_SUBSTATE_CONFIGURATION_DONE:
    /* End state do nothing */
    break;
  default:
    /* Invalid SubState */
    break;
  }

  return newState;
}

/**@brief  Proxy State machine.
  *@param  sub state
  *@retval unsigned value
  */
#ifdef ZID_FULL
uint32_t proxyStateMachine(uint32_t subState)
{
  uint32_t newState = subState;
  proxyContextType *context =  (proxyContextType *) hidGlobalContext;

  switch (subState) {
  case HID_SUBSTATE_PROXY_INIT:
    {
      if (context->sourceState != HID_STATE_CONFIGURATION) {
      	context->pairRef = 0;
      }
      HID_ClassDriver_HostReset(FALSE);
      context->startTime = TIME_CurrentTime();
      SUBSTATE_TRANSITION(HID_SUBSTATE_PROXY, INIT, GET_DEV_ATTR); // Get device attributes
    }
    break;
  case HID_SUBSTATE_PROXY_GET_DEV_ATTR:
    {
      deviceAttributeInfo_Type attr;
      deviceAttributeType devAttr;
      uint8_t list[aplcMaxStdDescCompsPerHID];
      bool proxyEnd=FALSE, devConfError=FALSE;
      
      if (isValidPairRef(context->pairRef)) {
	attr.proxyAttr = TRUE;
	attr.proxyRef = context->pairRef;
	  
	attr.value = (uint8_t*)&devAttr.aplHIDVendorId;
	attr.id = aplHIDVendorId_ID;
	ZID_GetDeviceAttr(&attr);
	  
	attr.value = (uint8_t*)&devAttr.aplHIDProductId;
	attr.id = aplHIDProductId_ID;
	ZID_GetDeviceAttr(&attr);
	  
	attr.value = (uint8_t*)&devAttr.aplHIDDeviceReleaseNumber;
	attr.id = aplHIDDeviceReleaseNumber_ID;
	ZID_GetDeviceAttr(&attr);

	attr.value = (uint8_t*)&devAttr.aplHIDNumEndpoints;
	attr.id = aplHIDNumEndpoints_ID;
	ZID_GetDeviceAttr(&attr);

	attr.value = (uint8_t*)&devAttr.aplHIDDeviceSubclass;
	attr.id = aplHIDDeviceSubclass_ID;
	ZID_GetDeviceAttr(&attr);

	attr.value = (uint8_t*)&devAttr.aplHIDProtocolCode;
	attr.id = aplHIDProtocolCode_ID;
	ZID_GetDeviceAttr(&attr);

	attr.value = (uint8_t*)&devAttr.aplHIDPollInterval;
	attr.id = aplHIDPollInterval_ID;
	ZID_GetDeviceAttr(&attr);

	attr.value = (uint8_t*)&devAttr.aplHIDParserVersion;
	attr.id = aplHIDParserVersion_ID;
	ZID_GetDeviceAttr(&attr);

	attr.value = (uint8_t*)&devAttr.aplHIDCountryCode;
	attr.id = aplHIDCountryCode_ID;
	ZID_GetDeviceAttr(&attr);

	attr.value = (uint8_t*)&devAttr.aplHIDNumStdDescComps;
	attr.id = aplHIDNumStdDescComps_ID;
	ZID_GetDeviceAttr(&attr);

	attr.value = (uint8_t*)&devAttr.aplHIDNumNonStdDescComps;
	attr.id = aplHIDNumNonStdDescComps_ID;
	ZID_GetDeviceAttr(&attr);

	if (HID_ClassDriver_SetUSBDescAttribute(&devAttr) == CLASS_DRIVER_SUCCESS) {
	  if (devAttr.aplHIDNumStdDescComps != 0) {
	    attr.value = list;
	    attr.id = aplHIDStdDescCompsList_ID;
	    ZID_GetDeviceAttr(&attr);
	    if (HID_ClassDriver_SetUSBRptDesc(TRUE, list, devAttr.aplHIDNumStdDescComps) !=  CLASS_DRIVER_SUCCESS)
	      devConfError = TRUE;
	  }
	
	  if (devAttr.aplHIDNumNonStdDescComps != 0) {
	    uint8_t i, id, *list;
	    uint16_t rptLen;
	    for (i=0; i<devAttr.aplHIDNumNonStdDescComps; i++) {
	      id = aplHIDNonStdDescCompSpec_MIN_ID + i;
	      list = (uint8_t *)ZID_GetNonStdReportAddr(context->pairRef, id, &rptLen);
	      if (HID_ClassDriver_SetUSBRptDesc(FALSE, list, rptLen) !=  CLASS_DRIVER_SUCCESS) {
		devConfError = TRUE;
	      }
	    }
	  }
	} else {
	  devConfError = TRUE;
	}
      }
	
      if (context->sourceState != HID_STATE_CONFIGURATION) {
	context->pairRef++;
	if (context->pairRef == nwkcMaxPairingTableEntries)
	  proxyEnd = TRUE;
      } else {
	proxyEnd = TRUE;
      }
      
      if (devConfError) {
	SUBSTATE_TRANSITION(HID_SUBSTATE_PROXY, CLASS_DRIVER_NOTIFICATION, DONE); // Error Device Not Notified
      } else {
	if (!proxyEnd) {
	  SUBSTATE_TRANSITION(HID_SUBSTATE_PROXY, GET_DEV_ATTR, GET_DEV_ATTR); // Collect Device attr
	} else {
	  SUBSTATE_TRANSITION(HID_SUBSTATE_PROXY, GET_DEV_ATTR, CLASS_DRIVER_NOTIFICATION); // All dev Attr collected
	}
      }
    }
    break;
  case HID_SUBSTATE_PROXY_CLASS_DRIVER_NOTIFICATION:
    {
      HID_ClassDriver_NotifyUSBDevice();
      SUBSTATE_TRANSITION(HID_SUBSTATE_PROXY, CLASS_DRIVER_NOTIFICATION, CLASS_DRIVER_WAIT_NOTIFICATION); // Device Notification on going
      context->startTime = TIME_CurrentTime();
    }
    break;
  case HID_SUBSTATE_PROXY_CLASS_DRIVER_WAIT_NOTIFICATION:
    {
      if (TIME_ELAPSED(context->startTime) <= HID_PROXY_RESET_TIMEOUT) {
	HID_ClassDriver_USBTick();
	SUBSTATE_TRANSITION(HID_SUBSTATE_PROXY, CLASS_DRIVER_WAIT_NOTIFICATION, CLASS_DRIVER_WAIT_NOTIFICATION); // Wait Device Notification 3 sec
      } else {
	SUBSTATE_TRANSITION(HID_SUBSTATE_PROXY, CLASS_DRIVER_WAIT_NOTIFICATION, DONE); // Device Notified
      }
    }
    break;
  case HID_SUBSTATE_PROXY_DONE:
    {
      // Nothing to do
    }
    break;
  default:
    /* Invalid SubState */
    break;
  }

  return newState;
}
#endif

/**@brief  HID top State machine.
  *@param  sub state
  *@retval unsigned value
  */
HID_StateType hidStateMachine(HID_StateType state)
{
  HID_StateType newState = state;

  switch (state.state) {
  case HID_STATE_INIT:
    /* Do nothing a NWK_Init() needs to be called to start the network
     * and configure the device.
     */
    break;
  case HID_STATE_START:
    {
      startContextType *context =  (startContextType *) hidGlobalContext;
      idleContext.startTimeActiveMode = TIME_CurrentTime();
      idleContext.enablePowerSave = FALSE;
      idleContext.radioSleepFlag = TRUE;
      idleContext.msgSentPending = 0;
#ifdef ZID_FULL
      memset(rptToRepeat, 0, sizeof(rptToRepeat));
#endif
      newState.subState = startStateMachine(state.subState);
      if (newState.subState == HID_SUBSTATE_START_DONE) {
	if ((!context->startComplete) || (context->status != SUCCESS)) {
	  STATE_TRANSITION(HID_STATE, START, INIT); // HID not started
	} else {
#ifndef TEST
	  ATOMIC(NWK_Flush(););
#endif
	  ZID_AttrFlush();
	  if (devIsPaired()) {
#if defined(ZID_FULL) && !defined(TEST)
	    if (isHIDAdapter()) {
	      proxyContextType *context =  (proxyContextType *) hidGlobalContext;
	      context->sourceState = HID_STATE_START;
	      STATE_TRANSITION(HID_STATE, START, PROXY); // HID adapter started & paired
	    } else {
#endif
		STATE_TRANSITION(HID_STATE, START, IDLE); // HID class device started & paired
#if defined(ZID_FULL) && !defined(TEST)
	      }
#endif
	  } else {
	    STATE_TRANSITION(HID_STATE, START, UNPAIRED); // HID started & unpaired
	  }
	}    
      }
    }
    break;
  case HID_STATE_UNPAIRED:
    /* Nothing to do */
    ZID_AttrFlush();
    break;
  case HID_STATE_PAIRING:
    {
      pairingContextType *context =  (pairingContextType *) hidGlobalContext;
      newState.subState = pairingStateMachine(state.subState);
      if (newState.subState == HID_SUBSTATE_PAIRING_DONE) {
	if (context->pairingStatus == SUCCESS) {
	  uint8_t pairRef;
	  pairRef = context->newPairRef;
	  { 
	    configurationContextType *context =  (configurationContextType *) hidGlobalContext; 
	    context->msgRcv = FALSE;
	    context->pairRef = pairRef;
	  }
	  STATE_TRANSITION(HID_STATE, PAIRING, CONFIGURATION); // Pairing successful
	} else {
	  if (!devIsPaired()) {
	    STATE_TRANSITION(HID_STATE, PAIRING, UNPAIRED); // Pairing unsuccessful and pairing table empty
	  } else {
	    STATE_TRANSITION(HID_STATE, PAIRING, IDLE); // Pairing unsuccessful and pairing table not empty
	  }
	}
      }
    }
    break;
  case HID_STATE_CONFIGURATION:
    {
#ifdef ZID_FULL
      configurationContextType *context =  (configurationContextType *) hidGlobalContext;
      uint8_t pairRef;
#endif
      newState.subState = configurationStateMachine(state.subState);
      if (newState.subState == HID_SUBSTATE_CONFIGURATION_DONE) { 
#ifdef TEST
	{
	  // Add to signal when the configuration is ended
	  uint8_t cmdCode;
	  ConfigurationComplete_Type cfgEnd;
	  if (!context->configurationSuccess)
	    cfgEnd.status = ZID_CONFIGURATION_FAILURE;
	  else 
	    cfgEnd.status = ZID_SUCCESS;
	  cmdCode = CONFIGURATION_COMPLETE_CODE|ZID_GDP_COMMAND_FRAME;
	  cfgEnd.pairRef = context->pairRef;
	  ZID_CommandNotification(cmdCode, &cfgEnd);
	}
#endif
#ifdef ZID_FULL
	pairRef = context->pairRef;
	if (context->configurationSuccess && isHIDAdapter()) {
	  proxyContextType *proxyContext =  (proxyContextType *) hidGlobalContext;
	  proxyContext->sourceState = HID_STATE_CONFIGURATION;
	  proxyContext->pairRef = pairRef;
	  ZID_AttrFlush();
#ifdef TEST
	  STATE_TRANSITION(HID_STATE, CONFIGURATION, IDLE); // HID Host Configuration ended with success in TEST mode
#else
	  STATE_TRANSITION(HID_STATE, CONFIGURATION, PROXY); // HID Host Configuration ended with success
#endif
	} else {
#endif
	  if (devIsPaired()) {
	    STATE_TRANSITION(HID_STATE, CONFIGURATION, IDLE); // Configuration exit and pairing table not empty
	  } else {
	    STATE_TRANSITION(HID_STATE, CONFIGURATION, UNPAIRED); // Configuration exit and pairing table empty
	  }
#ifdef ZID_FULL
	}
#endif
      } 
    }
    break;
#ifdef ZID_FULL
  case HID_STATE_PROXY:
    {
      newState.subState = proxyStateMachine(state.subState);
      if (newState.subState == HID_SUBSTATE_PROXY_DONE) {
	STATE_TRANSITION(HID_STATE, PROXY, IDLE); //  Proxy complete
      }
    }
    break;
#endif
  case HID_STATE_IDLE:
    {
      if (!isHIDAdapter()) {
	if (idleContext.hostDP) {
	  idleContext.hostDP = FALSE;
	  idleContext.startTimeActiveMode = TIME_CurrentTime();
	}
	if ((idleContext.msgSentPending == 0) && !cmdRspPending() && 
	    (TIME_ELAPSED(idleContext.startTimeActiveMode) > aplcMaxRxOnWaitTime)) {
	  if (idleContext.radioSleepFlag)
	    NWK_PowerDown();
	  else 
	    NWK_PowerUp(FALSE);
	  idleContext.enablePowerSave = TRUE;
	} else {
	  idleContext.enablePowerSave = FALSE;
	}
      }
#ifdef ZID_FULL
      else { 
	checkRptToRepeat();
      }
#endif
      STATE_TRANSITION(HID_STATE, IDLE, IDLE); // No action
    }
    break;
  default:
    /* Error state not defined */
    break;
  }

  return newState;
}

/**@brief  ZID Set HDP.
  *@param  boolean value
  *@retval None
  */
void ZID_SetHDP(bool value)
{
  idleContext.hostDP = value;
}

/**@brief  This function sets the HID command response
  *        waited  in aplcMaxResponseWaitTime
  *@param  pairRef pairing reference
  *@param  cmd command code sent
  *@return None
  */
void ZID_WaitCmdRsp(uint8_t pairRef, uint8_t cmd)
{
  if (pairRef < nwkcMaxPairingTableEntries) {
    cmdQ[pairRef].cmd = cmd;
    cmdQ[pairRef].startTime = TIME_CurrentTime();
  }
}

/**@brief  Enable internal power save
  *@param  None
  *@return boolean value
  */
bool internalEnablePowerSave(void) 
{
  return idleContext.enablePowerSave;
}

/**@brief  This function notifies the USB class driver request to the device
  *@param  commandCode USB command code notification
  *@param  packet contains the information of the command (first byte si always the
  *        device number)
  *@retval None
  */
#ifdef ZID_FULL
void HID_ClassDriver_USBCmdNotification(uint8_t commandCode, uint8_t *packet)
{
  uint8_t pairRef;

  pairRef = packet[0];

  switch (commandCode) {
  case GET_REPORT_TRANS_CODE:
    {
      /* Packet Format : byte 0=pairRef; 1=reportType; 2=reportId */
      GetReport_Type rpt;
      rpt.pairRef = pairRef;
      rpt.reportType = packet[1];
      rpt.reportId = packet[2];
      rpt.dataPending = FALSE;
      ZID_GetReport(&rpt);
      DEBUG_MESSAGE(("Get Report \r\n"));
    }
    break;
  case SET_REPORT_TRANS_CODE:
    {
      /* Packet Format : byte 0=pairRef; 1=reportType; 2=reportId
         3=report data length; 4...=reportData*/
      SetReport_Type rpt;
      rpt.pairRef = pairRef;
      rpt.reportType = packet[1];
      rpt.reportId = packet[2];
      rpt.reportDataLen = packet[3];
      rpt.reportData = &packet[4];
      rpt.dataPending = FALSE;
      ZID_SetReport(&rpt);
      DEBUG_MESSAGE(("Set Report \r\n"));
    }
    break;
  case GET_IDLE_TRANS_CODE:
    {
      /* Packet Format : byte 0=pairRef;*/
      uint8_t idleRate;
      if (ZID_GetIdleRate(pairRef, &idleRate) == HID_ATTR_SUCCESS) {
	HID_ClassDriver_USBCommandRsp(GET_IDLE_RSP_TRANS_CODE, pairRef, &idleRate, 1);
      }
      DEBUG_MESSAGE(("Get Idle \r\n"));
    }
    break;
  case SET_IDLE_TRANS_CODE:
    {
      uint8_t idleRate, numStdDesc=0, numNonStdDesc=0;
      deviceAttributeInfo_Type attr;

      attr.proxyAttr = TRUE;
      attr.proxyRef = pairRef;
      attr.id = aplHIDNumStdDescComps_ID;
      attr.value = &numStdDesc;
      ZID_GetDeviceAttr(&attr);
      attr.id = aplHIDNumNonStdDescComps_ID;
      attr.value = &numNonStdDesc;
      ZID_GetDeviceAttr(&attr);
      
      /* reportId = packet[1]; */
      idleRate = packet[2];
      if ((numStdDesc > 1) || (numNonStdDesc > 1))
	idleRate = 0; 

      ZID_SetIdleRate(pairRef, idleRate);
      DEBUG_MESSAGE(("Set Idle \r\n"));
    }
    break;
  case GET_PROTOCOL_TRANS_CODE:
    {
      uint8_t protocol;
      if (ZID_GetCurrentProtocol(pairRef, &protocol) == HID_ATTR_SUCCESS) {
	HID_ClassDriver_USBCommandRsp(GET_PROTOCOL_RSP_TRANS_CODE, pairRef, &protocol, 1);
      }
      DEBUG_MESSAGE(("GetProtocol\r\n"));
    }
    break;
  case SET_PROTOCOL_TRANS_CODE:
    {
      ZID_SetCurrentProtocol(pairRef, packet[1]);
      DEBUG_MESSAGE(("SetProtocol\r\n"));
    }
    break;
  case CONFIGURATION_COMPLETE_TRANS_CODE:
    {
      DEBUG_MESSAGE(("Configuration_Complete\r\n"));
    }
    break;
  case HOST_DEVICE_FULL_TRANS_CODE:
    {
      DEBUG_MESSAGE(("Host_Device_Full\r\n"));
    }
    break;
  }
}

/**@brief  This function saves the last report received from an HID device.
  *        This report will be repeated at idle time to the USB host
  *@param  pairRef pairing reference
  *@param  rptId report identifier
  *@param  rptData report data
  *@param  rptLen report data length
  *@retval None
  */
void ZID_SetReportToRepeat(uint8_t pairRef, uint8_t rptId, uint8_t *rptData, uint8_t rptLen)
{
  rptToRepeat[pairRef].id = rptId;
  rptToRepeat[pairRef].length = rptLen;
  memcpy(rptToRepeat[pairRef].data, rptData, rptLen);
  ZID_GetIdleRate(pairRef, &rptToRepeat[pairRef].idleRate);
  rptToRepeat[pairRef].startIdleTime = TIME_CurrentTime();
  rptToRepeat[pairRef].startTime = TIME_CurrentTime();
}
#endif

#ifndef TEST
WEAK_FUNCTION(void ZID_CommandNotification(uint8_t cmdCode, void *param))
{
  if (cmdCode & ZID_GDP_COMMAND_FRAME) {
    switch(cmdCode & ZID_COMMAND_CODE) {
    case GENERIC_RESPONSE_CODE:
      {
      }
      break;
#ifdef ZID_FULL
    case CONFIGURATION_COMPLETE_CODE:
      {
      }
      break;
    case HEARTBEAT_CODE:
      {
      }
      break;
#endif
    case GET_ATTRIBUTES_RESPONSE_CODE:
      {
      }
      break;
    }
  } else {
    switch(cmdCode) {
    case GET_REPORT_CODE:
      {
      }
      break;
    case REPORT_DATA_CODE:
      {
      }
      break;
    case SET_REPORT_CODE:
      {
      }
      break;
    }
  }
}
#endif

/**@brief  Put the radio in the sleep mode when the the state is idle
  *@param  flag sleep
  *@retval None
  */
void ZID_RadioSleepWhenIdle(bool flagSleep)
{
  idleContext.radioSleepFlag = flagSleep;
}

/**@brief  set message to be sent
  *@param  None
  *@retval None
  */
void ZID_SetMessageSent(void)
{
  idleContext.msgSentPending++;
}

/* NWK RF4CE callback */

/**@brief  start confirmation
  *@param  status
  *@retval None
  */
void NLME_START_confirm (uint32_t *status)
{
  startContextType *context =  (startContextType *) hidGlobalContext;

  context->startComplete = TRUE;
  context->status = *status;
  ZID_DeviceInitCallback(status);
}

/**@brief  NLME discovery confirmation
  *@param  discovery confirmation
  *@retval None
  */
void NLME_DISCOVERY_confirm (NLME_DISCOVERY_CONFIRM_Type *param)
{
  pairingContextType *context =  (pairingContextType *) hidGlobalContext;
  ZID_PairingCallback_Type dscConfInfo;

  dscConfInfo.rf4cePrimitive = ZID_DSC_CONFIRM;
  dscConfInfo.Pairing.dscConfirm = param;
  context->pairingStatus = param->Status;
  memcpy(&context->dscConfirm, param, sizeof(NLME_DISCOVERY_CONFIRM_Type));
  if (param->NumNodes > 1)  {
    context->pairingStatus = ZID_MORE_ONE_NODE;
    dscConfInfo.Pairing.dscConfirm->Status = ZID_MORE_ONE_NODE;
  }
  context->dscConf = TRUE;
  ZID_PairingCallback(&dscConfInfo);
}

/**@brief  NLME auto discovery confirmation
  *@param  auto discovery confirmation
  *@retval None
  */
void NLME_AUTO_DISCOVERY_confirm(NLME_AUTO_DISCOVERY_CONFIRM_Type *param)
{
#ifdef ZID_FULL
  pairingContextType *context =  (pairingContextType *) hidGlobalContext;
  ZID_PairingCallback_Type autoDscConf;
  
  context->pairingStatus = param->Status;
  context->autoDscConf = TRUE;
  autoDscConf.rf4cePrimitive = ZID_AUTO_DSC_CONFIRM;
  autoDscConf.Pairing.autoDscConfirm = param;
  ZID_PairingCallback(&autoDscConf);
#endif
}

/**@brief  NLME pair confirmation
  *@param  pair confirmation
  *@retval None
  */
void NLME_PAIR_confirm(NLME_PAIR_CONFIRM_Type *param)
{
  pairingContextType *context =  (pairingContextType *) hidGlobalContext;
  ZID_PairingCallback_Type pairConfInfo;

  pairConfInfo.rf4cePrimitive = ZID_PAIR_CONFIRM;
  pairConfInfo.Pairing.pairConfirm = param;
  context->pairingStatus = param->Status;
  context->pairConf = TRUE;
  context->newPairRef = param->PairingRef;
  ZID_PairingCallback(&pairConfInfo);
}

/**@brief  NLME pair indication
  *@param  pair indication type
  *@retval None
  */
void NLME_PAIR_indication (NLME_PAIR_INDICATION_Type *params)
{
#ifdef ZID_FULL
  pairingContextType *context =  (pairingContextType *) hidGlobalContext;
  ZID_PairingCallback_Type pairIndInfo;

  memcpy(&context->pairIndication, params, sizeof(NLME_PAIR_INDICATION_Type));
  context->pairInd = TRUE;
  context->pairingStatus = params->Status;
  pairIndInfo.rf4cePrimitive = ZID_PAIR_INDICATION;
  pairIndInfo.Pairing.pairIndication = params;
  ZID_PairingCallback(&pairIndInfo);
#endif
}

/**@brief  NLME comm status indication
  *@param  comm status indication
  *@retval None
  */
void NLME_COMM_STATUS_indication (NLME_COMM_STATUS_INDICATION_Type *params)
{
#ifdef ZID_FULL
  pairingContextType *context =  (pairingContextType *) hidGlobalContext;
  ZID_PairingCallback_Type commIndInfo;

  context->newPairRef = params->PairingRef;
  context->commInd = TRUE;
  context->pairingStatus = params->Status;
  commIndInfo.rf4cePrimitive = ZID_COMM_STATUS_INDICATION;
  commIndInfo.Pairing.commStatusIndication = params;
  ZID_PairingCallback(&commIndInfo);
#endif
}

/**@brief  NLDE data confirmation
  *@param  data confirmation type
  *@retval None
  */
void NLDE_DATA_confirm(NLDE_DATA_CONFIRM_Type *param)
{
#ifdef TEST
  ZID_NwkFrameCallback_Type nwkFrame;

  if (reportData) {
    nwkFrame.rf4cePrimitive = ZID_DATA_CONFIRM;
    nwkFrame.dataCmd.dataConfirm = param; 
    ZID_NwkFrameCallback(&nwkFrame);
    reportData = FALSE;
  }
#endif

  if (GET_CURRENT_STATE() == HID_STATE_IDLE) 
    idleContext.msgSentPending--;

  if (GET_CURRENT_STATE() == HID_STATE_CONFIGURATION) {
    configurationContextType *context =  (configurationContextType *) hidGlobalContext;
    if (!context->genRspSent)
      context->genRspSent = TRUE;
  }
}

/**@brief  NLDE data indication 
  *@param  NLDE data indication type
  *@retval None
  */
void NLDE_DATA_indication(NLDE_DATA_INDICATION_Type *param)
{
  uint8_t commandCode;
  bool security;

  commandCode = param->nsdu[0] & ZID_COMMAND_CODE;
  security = param->RxFlags & 0x02;

  if (param->RxFlags & ZID_VENDOR_SPECIFIC_DATA) {
    ZID_NwkFrameCallback_Type nwkFrame;
    nwkFrame.rf4cePrimitive = ZID_DATA_INDICATION;
    nwkFrame.dataCmd.dataIndication = param; 
    ZID_NwkFrameCallback(&nwkFrame);
    return;
  }

  if (GET_CURRENT_STATE() == HID_STATE_CONFIGURATION)  {
    configurationContextType *context =  (configurationContextType *) hidGlobalContext;
    if (security && (context->pairRef == param->PairingRef) && 
	(((commandCode == PUSH_ATTRIBUTES_CODE) && isHIDAdapter()) ||
	 ((commandCode == GET_ATTRIBUTES_RESPONSE_CODE) && !isHIDAdapter()) || 
	 ((commandCode == GENERIC_RESPONSE_CODE) && !isHIDAdapter()) || 
	 ((commandCode == GET_ATTRIBUTES_CODE) && isHIDAdapter()) ||
	 ((commandCode == CONFIGURATION_COMPLETE_CODE) && isHIDAdapter()) ||
	 ((commandCode == SET_REPORT_CODE) && isHIDAdapter()))) {
      context->msgRcv = TRUE;
      context->msgLen = param->nsduLength;
      memcpy(context->msg, param->nsdu, context->msgLen);
    }
  } else {
    if (GET_CURRENT_STATE() == HID_STATE_IDLE) {
      verifyCmdResponse(param->PairingRef, param->nsdu[0]);
      callbackSchedule(param); 
    }
  }
#ifdef TRACE
  printDebugInfo(param);
#endif
}

/**@brief  NLME unpair indication
  *@param  pairing reference
  *@retval None
  */
void NLME_UNPAIR_indication (uint8_t *PairingRef)
{
  configurationContextType *context =  (configurationContextType *) hidGlobalContext;

  if ((GET_CURRENT_STATE() == HID_STATE_CONFIGURATION) &&
      (context->pairRef == *PairingRef))
    context->unpairSignal = TRUE;

#ifdef TEST
  if (GET_CURRENT_STATE() == HID_STATE_IDLE) {
    NLME_UNPAIR_response(*PairingRef);
  }
#endif
}

/**@brief  NLME unpair confirmation
  *@param  Unpair confirm type
  *@retval None
  */
void NLME_UNPAIR_confirm (NLME_UNPAIR_CONFIRM_Type *param)
{
  configurationContextType *context =  (configurationContextType *) hidGlobalContext;

  context->unpairSignal = TRUE;
}

/**@brief  NWK transmit MAC message handler
  *@param  status
  *@retval None
  */
void NWK_TransmitMACMessageHandler(uint32_t status)
{
}

/**@brief  NWK incoming MAC message handler
  *@param  raw packet received type
  *@retval None
  */
void NWK_IncomingMACMessageHandler(rawPacketReceivedType *msg)
{
}

/**@brief  NLME discovery indication type
  *@param  discovery indication
  *@retval None
  */
void NLME_DISCOVERY_indication(NLME_DISCOVERY_INDICATION_Type *param)
{
}


/**@brief  verify the right works
  *@param  NLDE data indication
  *@retval None
  */
#ifdef TRACE
static void printDebugInfo(NLDE_DATA_INDICATION_Type *param)
{
  uint8_t commandCode, isGDPCmd;

  DEBUG_MESSAGE(("State : 0x%08x SubState : 0x%08x\r\n", HID_State.state, HID_State.subState));

  commandCode = param->nsdu[0] & ZID_COMMAND_CODE;
  isGDPCmd = param->nsdu[0] & ZID_GDP_COMMAND_FRAME;
  
  if (isGDPCmd) {
    switch(commandCode) {
    case GENERIC_RESPONSE_CODE:
      {
	GenericResponse_Type genRsp;
	
	genRsp.pairRef = param->PairingRef;
	genRsp.rspCode = (GRSP_Status) param->nsdu[1];
	
	DEBUG_MESSAGE(("GDP_GenericResponseHandler PairingRef = 0x%02x status = ", genRsp.pairRef));
	switch(genRsp.rspCode) {
	case ZID_SUCCESS:
	  DEBUG_MESSAGE((" ZID_SUCCESS\r\n"));
	  break;
	case ZID_INVALID_REPORT_ID:
	  DEBUG_MESSAGE((" ZID_INVALID_REPORT_ID\r\n"));
	  break;
	case ZID_UNSUPPORTED_REQUEST:
	  DEBUG_MESSAGE((" ZID_UNSUPPORTED_REQ\r\n"));
	break;
	case ZID_INVALID_PARAMETER:
	  DEBUG_MESSAGE((" ZID_INVALID_PARAMETER\r\n"));
	  break;
	case ZID_CONFIGURATION_FAILURE:
	  DEBUG_MESSAGE((" ZID_CONFIGURATION_FAILURE\r\n"));
	  break;
	case ZID_MISSING_FRAGMENT:
	  DEBUG_MESSAGE((" ZID_MISSING_FRAGMENT\r\n"));
	  break;
	}
      }
      break;
    case CONFIGURATION_COMPLETE_CODE:
      {
	ConfigurationComplete_Type cfgEnd;
	
	if (!isHIDAdapter())
	  return; /* Command not supported for host device */
	
	cfgEnd.pairRef = param->PairingRef;
	cfgEnd.status = (GRSP_Status)param->nsdu[1];
	
	DEBUG_MESSAGE(("GDP_ConfigurationCompleteHandler PairingRef = 0x%02x status = 0x%02x\r\n", cfgEnd.pairRef, cfgEnd.status));
      }
      break;
    case HEARTBEAT_CODE:
      {
      HeartBeat_Type attr;
      
      attr.pairRef = param->PairingRef;
      DEBUG_MESSAGE(("GDP_HeartBeatHandler PairingRef = 0x%02x\r\n", attr.pairRef));      
      }
      break;
    case GET_ATTRIBUTES_CODE:
      {
	GetAttributes_Type attr;
	uint8_t i;
	
	attr.pairRef = param->PairingRef;
	attr.attrListLength = param->nsduLength - 1;
	attr.attrId = &param->nsdu[1];
	DEBUG_MESSAGE(("GDP_GetAttributersHandler PairingRef = 0x%02x listLength = 0x%02x\r\n",
		       attr.pairRef, attr.attrListLength));
	DEBUG_MESSAGE(("List of attributes: "));
	for (i=0; i<attr.attrListLength; i++) {
	  DEBUG_MESSAGE(("0x%02x ", attr.attrId[i]));
	}
	DEBUG_MESSAGE(("\r\n"));
      }
      break;
    case GET_ATTRIBUTES_RESPONSE_CODE:
      {
	GetAttributesResponse_Type attr;
	uint8_t i, index=0, status, len;
	
	attr.pairRef = param->PairingRef;
	attr.statusRecLength = param->nsduLength - 1;
	attr.statusRec = &param->nsdu[1];
	DEBUG_MESSAGE(("GDP_GetAttributersResponseHandler PairingRef = 0x%02x recordLength = 0x%02x\r\n",
		       attr.pairRef, attr.statusRecLength));
	DEBUG_MESSAGE(("List of record:\r\n"));
	index = 0;
	while(index < attr.statusRecLength) {
	  DEBUG_MESSAGE(("ID: 0x%02x Status: 0x%02x ", 
			 attr.statusRec[index], 
			 attr.statusRec[index+1]));
	  status = attr.statusRec[index+1];
	  index += 2;
	  if (status == 0) {
	    DEBUG_MESSAGE(("Length: 0x%02x Value: ", attr.statusRec[index]));
	    len = attr.statusRec[index];
	    index++;
	    for (i=0; i<len; i++) {
	      DEBUG_MESSAGE(("0x%02x ", attr.statusRec[index++]));
	    }
	  }
	  DEBUG_MESSAGE(("\r\n"));
	}
      }
      break;
    case PUSH_ATTRIBUTES_CODE:
      {
	PushAttributes_Type attr;
	uint8_t i;
	
	if (!isHIDAdapter())
	  return; /* Command not supported for hid device */
	
	attr.pairRef = param->PairingRef;
	attr.attributesRecLength = param->nsduLength - 1;
	attr.attributesRec = &param->nsdu[1];
	DEBUG_MESSAGE(("GDP_PushAttributesHandler PairingRef = 0x%02x attributesLength = 0x%02x\r\n",
		       attr.pairRef, attr.attributesRecLength));
	DEBUG_MESSAGE(("List of attributes: "));
	for (i=0; i<attr.attributesRecLength; i++) {
	  DEBUG_MESSAGE(("0x%02x ", attr.attributesRec[i]));
	}
	DEBUG_MESSAGE(("\r\n"));
      }
      break;
    default:
      break;
    }
  } else {
    switch(commandCode) {
    case GET_REPORT_CODE:
      {
	GetReport_Type attr;
      
	if (isHIDAdapter())
	  return; /* Command not supported for host device */
      
	attr.pairRef = param->PairingRef;
	attr.reportType = param->nsdu[1];
	attr.reportId = param->nsdu[2];
	DEBUG_MESSAGE(("ZID_GetReportHandler PairingRef = 0x%02x reportType = 0x%02x reportId = 0x%02x\r\n",
		       attr.pairRef, attr.reportType, attr.reportId));
      }
      break;
    case REPORT_DATA_CODE:
      {
	ReportData_Type rpt;
	uint8_t index, i, reportType, reportId, reportDataLen, *reportData;
      
	rpt.pairRef = param->PairingRef;
	rpt.reportPayloadLength = param->nsduLength - 1;
	rpt.reportDataRecords = &param->nsdu[1];

	index = 0;
	DEBUG_MESSAGE(("ZID_ReportData PairingRef = 0x%02x \r\n", rpt.pairRef));
	while(index < rpt.reportPayloadLength) {
	  reportDataLen = rpt.reportDataRecords[index] - 2;
	  reportType = rpt.reportDataRecords[index+1];
	  reportId = rpt.reportDataRecords[index+2];
	  reportData = &(rpt.reportDataRecords[index+3]);
	  index += reportDataLen + 3;
	  DEBUG_MESSAGE(("Report Type = 0x%02x ReportId = 0x%02x \r\n", reportType, reportId));
	  DEBUG_MESSAGE(("Report Data: "));
	  for (i=0; i<reportDataLen; i++) {
	    DEBUG_MESSAGE(("0x%02x ", reportData[i]));
	  }
	  DEBUG_MESSAGE(("\r\n"));
	}
      }
      break;
    case SET_REPORT_CODE:
      {
	SetReport_Type rptParam;
	uint8_t i;

	rptParam.pairRef = param->PairingRef;
	rptParam.reportType = param->nsdu[1];
	rptParam.reportId = param->nsdu[2];
	rptParam.reportDataLen = param->nsduLength - 3;
	rptParam.reportData = (uint8_t*)&param->nsdu[3];
      
	DEBUG_MESSAGE(("ZID_SetReportHandler PairingRef = 0x%02x reportType = 0x%02x reportId = 0x%02x reportDataLen = 0x%02x\r\n",
		       rptParam.pairRef, rptParam.reportType, rptParam.reportId, rptParam.reportDataLen));
	DEBUG_MESSAGE(("ReportData: "));
	for (i=0; i<rptParam.reportDataLen; i++) {
	  DEBUG_MESSAGE((" 0x%02x", rptParam.reportData[i]));
	}
	DEBUG_MESSAGE(("\r\n"));
      }
      break;
    default:
      break;
    }
  }
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
