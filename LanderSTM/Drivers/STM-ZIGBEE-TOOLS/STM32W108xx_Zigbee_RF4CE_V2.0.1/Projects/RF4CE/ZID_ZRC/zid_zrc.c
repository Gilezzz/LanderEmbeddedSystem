/**
  ******************************************************************************
  * @file    zid_zrc.c 
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   ZID ZRC RF4CE implementation
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
#include <stdlib.h>
#include <string.h>

#include "timer.h"
#include "zid_zrc.h"
#include "zid_zrc_sm.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct LastReportToRepeatS {
  uint8_t pairRef;
  uint8_t length;
  uint8_t rpt[64];
  uint32_t startTime;
} LastReportToRepeat_Type;

typedef struct LastCommandSentS{
  uint8_t length;
  uint8_t code;
  uint8_t payload[128];
  uint32_t startTime;
} LastCommand_Type;

/* Private define ------------------------------------------------------------*/
#define HID_ADAPTER_NODECAP      0x07 /* Security Capable - Main Power - Target */
#define HID_CLASS_DEVICE_NODECAP 0x04 /* Security Capable - Battery Power - Controller */

#define ZID_COMMAND_CODE         0x0F /* Bits 0-3 Command code */
#define ZID_DATA_PENDING         0x80 /* HID data pending field  enabled */
#define ZID_GDP_COMMAND_FRAME    0x40 /* GDP Command Frame */

#define SAFE_TX_TIME_MULTIPLIER  1000

/* Private macro -------------------------------------------------------------*/
#define STATE_TRANSITION(prefix, prevState, nextState) { HID_State.state = prefix##_##nextState; HID_State.subState = prefix##_##nextState | 0x0001;}
#define GET_CURRENT_STATE() HID_State.state
#define GET_CURRENT_SUBSTATE() HID_State.subState

/* Private variables ---------------------------------------------------------*/
static uint32_t startUnsafeTx[nwkcMaxPairingTableEntries];
static LastReportToRepeat_Type lastReport;
static LastCommand_Type lastCommand;

/* Private function prototypes -----------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
HID_StateType HID_State = {HID_STATE_INIT, 0};
#ifdef TEST
bool reportData=FALSE;
#endif

/* Private functions ---------------------------------------------------------*/

/**@brief  Checks the validation of the pairing reference.
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

/**@brief  assures the Tx sefety.
  *@param  Pairing reference
  *@retval boolean value
  */
static bool safeTx(uint8_t pairRef)
{
  bool retVal = FALSE;
  deviceAttributeInfo_Type attr;
  uint16_t maxUnsafeTxWin;
  
  attr.id = aplIntPipeUnsafeTxWindowTime_ID;
  attr.value = (uint8_t *)&maxUnsafeTxWin;
  attr.proxyAttr = FALSE;

  if (ZID_GetDeviceAttr(&attr) != HID_ATTR_SUCCESS) 
    maxUnsafeTxWin = aplcMinIntPipeUnsafeTxWindowTime;

  if ((startUnsafeTx[pairRef] == 0) ||
      (TIME_ELAPSED(startUnsafeTx[pairRef]) >= (maxUnsafeTxWin*SAFE_TX_TIME_MULTIPLIER))) {
    retVal = TRUE;
    startUnsafeTx[pairRef] = TIME_CurrentTime();
  } else {
    retVal = FALSE;
  }

  return retVal;
}

/**@brief  Checks the report data to send .
  *@param  pointer to Report data 
  *@retval boolean value
  */
static bool rptToSend(ReportData_Type *param)
{
  uint8_t rptInterval;
  deviceAttributeInfo_Type attr;
  ATTR_Status status;

  if (!isHIDAdapter()) {
    if ((lastReport.pairRef != param->pairRef) || 
	(lastReport.length != param->reportPayloadLength) ||
	(memcmp(param->reportDataRecords, lastReport.rpt, param->reportPayloadLength) != 0)) {
      memcpy(lastReport.rpt, param->reportDataRecords, param->reportPayloadLength);
      lastReport.length = param->reportPayloadLength;
      lastReport.pairRef = param->pairRef;
      lastReport.startTime = TIME_CurrentTime();
      return TRUE;
    }
    attr.proxyAttr = FALSE;
    attr.id = aplReportRepeatInterval_ID;
    attr.value = &rptInterval;
    status = ZID_GetDeviceAttr(&attr);
    if (status == HID_ATTR_SUCCESS) {
      if ((rptInterval != 0) && 
	  (TIME_ELAPSED(lastReport.startTime) < (rptInterval*1000))) {
	return FALSE;
      }
    }
    lastReport.startTime = TIME_CurrentTime();
  }
  return TRUE;
}

/**@brief  Verifies the report info.
  *@param  report type
  *@param  reportidentity
  *@retval unsigned value
  */
static uint8_t verifyRptInfo (uint8_t reportType, uint8_t reportId)
{
  deviceAttributeInfo_Type attr;
  uint8_t rpt[260];
  uint8_t numStdLen, numNonStdLen, i;

  if ((reportType == 0) || (reportType > 0x03))
    return ZID_INVALID_PARAMETER;
  
  attr.proxyAttr = FALSE;

  attr.id = aplHIDNumStdDescComps_ID;
  attr.value = &numStdLen;
  ZID_GetDeviceAttr(&attr);

  attr.id = aplHIDStdDescCompsList_ID;
  attr.value = rpt;
  ZID_GetDeviceAttr(&attr);
  for (i=0; i<numStdLen; i++) {
    if (rpt[i] == reportId)
      return ZID_SUCCESS;
  }
  
  attr.id = aplHIDNumNonStdDescComps_ID;
  attr.value = &numNonStdLen;
  ZID_GetDeviceAttr(&attr);

  for (i=0; i<numNonStdLen; i++) {
    attr.id = aplHIDNonStdDescCompSpec_MIN_ID + i;
    attr.value = rpt;
    if (rpt[3] == reportId)
      return ZID_SUCCESS;
  }

  return ZID_INVALID_REPORT_ID;
}

/* Public functions ----------------------------------------------------------*/

/**@brief  Device Init
  *@param  deviceType The HID device type. Valid device are:
  *         - HID_CLASS_DEVICE
  *         - HID_ADAPTOR
  *@param  coldStart This parameter allow the network layer to perform 
  *         a cold start (e.g. the first startup outside the factory).
  *
  *@return The status of the device init attempt. Valid values are:
  *          - SUCCESS
  *          - RF4CE_BUSY
  *          - RF4CE_INVALID_PART
  *          - RF4CE_NVM_DATA_INVALID
  *          - RF4CE_SAP_PENDING
  */
uint32_t ZID_DeviceInit(uint8_t deviceType, bool coldStart, uint8_t profileSupported)
{
  uint32_t status, tmp;
  NLME_SET_Type val;
  startContextType *context =  (startContextType *) hidGlobalContext;
  uint8_t nodeCap;

#ifndef ZID_FULL
  if (deviceType == HID_ADAPTER) {
    return RF4CE_INVALID_CALL;
  }
#endif

  if ((profileSupported == 0) || (profileSupported > 3))
    return RF4CE_INVALID_CALL;

  memset(startUnsafeTx, 0, sizeof(startUnsafeTx));
  memset(&lastReport, 0, sizeof(LastReportToRepeat_Type));
  memset(&lastCommand, 0, sizeof(LastCommand_Type));

  context->startComplete = FALSE;
  context->profileSupported = profileSupported;
  if (deviceType == HID_ADAPTER) {
    nodeCap = HID_ADAPTER_NODECAP;
  } else {
    nodeCap = HID_CLASS_DEVICE_NODECAP;
  }

#ifdef ZID_FULL
  if (deviceType == HID_ADAPTER)
    HID_ClassDriver_USBInit();
#endif

  if (coldStart) {
    uint16_t myVendorId = 0xFFF2;
    NWK_Config(nwkcVendorIdentifier_ID, &myVendorId);
    NWK_Config(nwkcVendorString_ID, "Vendor");
  }

  /* Start the network and the NIB with the 
   * default value
   */  
  status = NWK_Init(nodeCap, coldStart);
  context->status = status;

  if ((status != SUCCESS) && (status != RF4CE_SAP_PENDING))
    return status;

  ZID_DeviceAttrInit(coldStart, deviceType);

  if (coldStart) {

    /* Set the HID profile 
     * initial NIB attribute
     */
    val.NIBAttribute = nwkActivePeriod_ID;
    val.NIBAttributeIndex = 0;
    tmp = 0xffffff; // Device always ON
    val.NIBAttributeValue = (uint8_t *)&tmp;
    NLME_Set(&val);

    val.NIBAttribute = nwkDiscoveryLQIThreshold_ID;
    val.NIBAttributeIndex = 0;
    tmp = 1; // LQI Default value
    val.NIBAttributeValue = (uint8_t *)&tmp;
    NLME_Set(&val);

    val.NIBAttribute = nwkDiscoveryRepetitionInterval_ID;
    val.NIBAttributeIndex = 0;
    tmp = 0x00f424; // 1s Default value
    val.NIBAttributeValue = (uint8_t *)&tmp;
    NLME_Set(&val);

    val.NIBAttribute = nwkDutyCycle_ID;
    val.NIBAttributeIndex = 0;
    tmp = 0;
    val.NIBAttributeValue = (uint8_t *)&tmp;
    NLME_Set(&val);

    val.NIBAttribute = nwkMaxDiscoveryRepetitions_ID;
    val.NIBAttributeIndex = 0;
    tmp = 0x1e;
    val.NIBAttributeValue = (uint8_t *)&tmp;
    NLME_Set(&val);

    val.NIBAttribute = nwkMaxReportedNodeDescriptors_ID;
    val.NIBAttributeIndex = 0;
    tmp = 0x1;
    val.NIBAttributeValue = (uint8_t *)&tmp;
    NLME_Set(&val);

    val.NIBAttribute = nwkUserString_ID;
    val.NIBAttributeIndex = 0;
    if (deviceType == HID_ADAPTER)
      val.NIBAttributeValue = (uint8_t*)"STM32W HID HOST";
    else
      val.NIBAttributeValue = (uint8_t*)"STM32W HID DEV";
    NLME_Set(&val);
  }

  STATE_TRANSITION(HID_STATE, INIT, START); // Init device

  return RF4CE_SAP_PENDING;
}

/**@brief  The ZID_Pairing
  *        function manages the case of 
  *        discovery/pairing procedure between HID class device
  *        and HID adapter
  *@param  param The structure containing the 
  *        ZID_Pairing_Type parameters
  *@return The status of the ZID pairing function call.
  */
uint32_t ZID_Pairing(ZID_Pairing_Type *param)
{
  uint32_t state;
  pairingContextType *context =  (pairingContextType *) hidGlobalContext;

  state = GET_CURRENT_STATE();
  if ((state != HID_STATE_UNPAIRED) && (state != HID_STATE_IDLE))
    return RF4CE_BUSY;

  /* Load all the infos inside the Pairing context */
  memcpy(&context->param, param, sizeof(ZID_Pairing_Type));

  switch (state) {
  case HID_STATE_UNPAIRED:
    STATE_TRANSITION(HID_STATE, UNPAIRED, PAIRING); // Pairing push button
    break;
  case HID_STATE_IDLE:
    NWK_PowerUp(FALSE);
    STATE_TRANSITION(HID_STATE, IDLE, PAIRING); // Pairing push button
    break;
  }

  return RF4CE_SAP_PENDING;
}

/**@brief  A tick routine that should be called periodically in the 
  *        application's main event loop to check events, advance state machine, 
  *        invoke callbacks, manage power state, check sleep timer
  *@return None
  */
void ZID_Tick(void)
{
  uint32_t retVal;
  
  retVal = NWK_Tick();

  /* Check whether cache flush is required */
  if (!(retVal & RF4CE_STATE_BUSY) && 
      (retVal & RF4CE_STATE_NEED_CACHE_FLUSH) &&
      ((GET_CURRENT_STATE() == HID_STATE_IDLE) || (GET_CURRENT_STATE() == HID_STATE_UNPAIRED))) {
#ifndef TEST
    ATOMIC(NWK_Flush(););
#endif
  }

  HID_State = hidStateMachine(HID_State);

#ifdef ZID_FULL
  HID_ClassDriver_USBTick();
#endif
  if ((GET_CURRENT_STATE() == HID_STATE_IDLE) || 
      (GET_CURRENT_STATE() == HID_STATE_UNPAIRED)) {
    ZID_AttrFlush();
  }
}

/**@brief  This function sends a generic RF4CE network frame
  *@param  packet contains the network frame parameter
  *@return Status of the function call
  */
uint32_t ZID_SendNwkFrame(ZID_NwkFrame_Type *packet)
{
  uint32_t status=SUCCESS;
  NLDE_DATA_REQUEST_Type param;

  param.nsduLength = packet->dataLen;
  param.nsdu = packet->data;
  param.PairingRef = packet->pairingRef;
  param.ProfileId = packet->profileId;
  param.VendorId = 0xfff1; //Test vendor id #1
  if (packet->pairingRef == 0xff) {
    param.TxOptions = RF4CE_DATA_REQ_TX_MODE; //Broadcast - nwk address - no ack - no security
                                             //- multiple channel - no channel designator - non vendor specific
  } else {
    param.TxOptions = packet->TxOptions;
  }
  
  status = NWK_PowerUp(FALSE);
  if (status == SUCCESS)
    status = NLDE_DATA_request(&param);

  return status;
}

/* ZID Commands */

/**@brief  The generic response command frame allows a node to issue a response 
  *        to a command frame that does not have a corresponding response frame.
  *@param  param The structure containing the Generic Response parameters
  *@return command result. Valid value are:
  *        - RF4CE_SAP_PENDING
  *        - RF4CE_BUSY
  */
uint32_t GDP_GenericResponse(GenericResponse_Type *param)
{
  uint32_t state, status;
  uint8_t data[2];
  ZID_NwkFrame_Type msg;

  state = GET_CURRENT_STATE();

  if ((state != HID_STATE_IDLE) && (state != HID_STATE_CONFIGURATION))
    return RF4CE_BUSY;

  if (!isValidPairRef(param->pairRef))
    return NO_PAIRING;

  msg.pairingRef = param->pairRef;
  msg.profileId = 0x02; /* ZID ZigBee Input Device */
  data[0] = GENERIC_RESPONSE_CODE | ZID_GDP_COMMAND_FRAME;
  if (param->dataPending && isHIDAdapter()) {
    data[0] |= ZID_DATA_PENDING;
  }
  data[1] = param->rspCode;
  msg.data = data;
  msg.dataLen = sizeof(data);
  msg.TxOptions = RF4CE_DATA_REQ_ACK_MODE | /* Unicast - nwk address - ack - security - */ 
                  RF4CE_DATA_REQ_SEC_MODE ;/* multiple channel - no channel designator - non vendor specific */ 
  status = ZID_SendNwkFrame(&msg);

  if ((status == RF4CE_SAP_PENDING) && 
      (state == HID_STATE_IDLE))
    ZID_SetMessageSent();

  return status;
}

/**@brief  The configuration complete command frame allows a node 
  *        to indicate that the current profile configuration step is complete.
  *@param  param The structure containing the Configuration Complete parameters
  *@return command result. Valid value are:
  *        - RF4CE_SAP_PENDING
  *        - RF4CE_BUSY
  *        - RF4CE_INVALID_CALL
  */
uint32_t GDP_ConfigurationComplete(ConfigurationComplete_Type *param)
{
  uint32_t state, status;
  uint8_t data[2];
  ZID_NwkFrame_Type msg;

  /* HID adaptor not support this HID command */
  if (isHIDAdapter())
    return RF4CE_INVALID_CALL;

  state = GET_CURRENT_STATE();

  if ((state != HID_STATE_IDLE) && (state != HID_STATE_CONFIGURATION))
    return RF4CE_BUSY;

  if (!isValidPairRef(param->pairRef))
    return NO_PAIRING;

  msg.pairingRef = param->pairRef;
  msg.profileId = 0x02; /* ZID ZigBee Input Device */
  data[0] = CONFIGURATION_COMPLETE_CODE | ZID_GDP_COMMAND_FRAME;
  data[1] = param->status;
  msg.data = data;
  msg.dataLen = 2;
  msg.TxOptions = RF4CE_DATA_REQ_ACK_MODE | /*Unicast - nwk address - ack - security - */ 
                  RF4CE_DATA_REQ_SEC_MODE ;/* multiple channel - no channel designator - non vendor specific */ 
  status = ZID_SendNwkFrame(&msg);

  if ((status == RF4CE_SAP_PENDING) && 
      (state == HID_STATE_IDLE))
    ZID_SetMessageSent();

  return status;
}

/**@brief  The heartbeat command frame allows a node, which sleeps most of the time,
  *        to check in with a target in case it wishes to send it a command.
  *@param  param The structure containing the Heart Beat parameters
  *@return command result. Valid value are:
  *        - RF4CE_SAP_PENDING
  *        - RF4CE_BUSY
  *        - RF4CE_INVALID_CALL
  */
uint32_t GDP_HeartBeat(HeartBeat_Type *param)
{
  uint32_t state, status;
  uint8_t data;
  ZID_NwkFrame_Type msg;

  /* Host not support this HID command */
  if (isHIDAdapter())
    return RF4CE_INVALID_CALL;

  state = GET_CURRENT_STATE();

  if (state != HID_STATE_IDLE)
    return RF4CE_BUSY;

  if (!isValidPairRef(param->pairRef))
    return NO_PAIRING;

  msg.pairingRef = param->pairRef;
  msg.profileId = 0x02; /* ZID ZigBee Input Device */
  data = HEARTBEAT_CODE | ZID_GDP_COMMAND_FRAME;
  msg.data = &data;
  msg.dataLen = 1;
  msg.TxOptions = RF4CE_DATA_REQ_ACK_MODE | /* Unicast - nwk address - ack - security - */ 
                  RF4CE_DATA_REQ_SEC_MODE ;/* multiple channel - no channel designator - non vendor specific */ 
  status = ZID_SendNwkFrame(&msg);

  if (status == RF4CE_SAP_PENDING) {
    ZID_WaitCmdRsp(param->pairRef, (HEARTBEAT_CODE|ZID_GDP_COMMAND_FRAME));
  }

  if ((status == RF4CE_SAP_PENDING) && 
      (state == HID_STATE_IDLE))
    ZID_SetMessageSent();

  return status;
}

/**@brief  The get attributes command frame allows a node to read one or more 
  *        profile attributes from a remote node.
  *@param  param The structure containing the Get Attributes parameters
  *@return command result. Valid value are:
  *        - RF4CE_SAP_PENDING
  *        - RF4CE_BUSY
  *        - RF4CE_INVALID_CALL
  */
uint32_t GDP_GetAttributes(GetAttributes_Type *param)
{  
  uint32_t state, status;
  uint8_t data[128];
  ZID_NwkFrame_Type msg;

  state = GET_CURRENT_STATE();

  if ((state != HID_STATE_IDLE) && (state != HID_STATE_CONFIGURATION))
    return RF4CE_BUSY;

  if (!isValidPairRef(param->pairRef))
    return NO_PAIRING;

  msg.pairingRef = param->pairRef;
  msg.profileId = 0x02; /* ZID ZigBee Input Device */
  data[0] = GET_ATTRIBUTES_CODE | ZID_GDP_COMMAND_FRAME;
  if (param->dataPending && isHIDAdapter())
    data[0] |= ZID_DATA_PENDING;
  memcpy(&data[1], param->attrId, param->attrListLength);  
  msg.data = data;
  msg.dataLen = param->attrListLength + 1;
  msg.TxOptions = RF4CE_DATA_REQ_ACK_MODE | /* Unicast - nwk address - ack - security - */ 
                  RF4CE_DATA_REQ_SEC_MODE ;/* multiple channel - no channel designator - non vendor specific */ 
  status = ZID_SendNwkFrame(&msg);

  if ((status == RF4CE_SAP_PENDING) && (state != HID_STATE_CONFIGURATION)) {
    ZID_WaitCmdRsp(param->pairRef, (GET_ATTRIBUTES_CODE|ZID_GDP_COMMAND_FRAME));
  }

  if ((status == RF4CE_SAP_PENDING) && 
      (state == HID_STATE_IDLE))
    ZID_SetMessageSent();
  
  return status;
}

/**@brief  The get attributes response command frame allows a remote node to respond 
  *        to a get attribute command frame.
  *@param  param The structure containing the Get Attributes Response parameters
  *@return command result. Valid value are:
  *        - RF4CE_SAP_PENDING
  *        - RF4CE_BUSY
  *        - RF4CE_INVALID_CALL
  */
uint32_t GDP_GetAttributesResponse(GetAttributesResponse_Type *param)
{
  uint32_t state, status;
  uint8_t data[128];
  ZID_NwkFrame_Type msg;

  state = GET_CURRENT_STATE();

  if ((state != HID_STATE_IDLE) && (state != HID_STATE_CONFIGURATION))
    return RF4CE_BUSY;

  if (!isValidPairRef(param->pairRef))
    return NO_PAIRING;

  msg.pairingRef = param->pairRef;
  msg.profileId = 0x02; //ZID ZigBee Input Device
  data[0] = GET_ATTRIBUTES_RESPONSE_CODE | ZID_GDP_COMMAND_FRAME;
  if (param->dataPending && isHIDAdapter()) {
    data[0] |= ZID_DATA_PENDING;
  }
  memcpy(&data[1], param->statusRec, param->statusRecLength);
  msg.data = data;
  msg.dataLen = param->statusRecLength + 1;
  msg.TxOptions = RF4CE_DATA_REQ_ACK_MODE | /* Unicast - nwk address - ack - security - */ 
                  RF4CE_DATA_REQ_SEC_MODE ;/* multiple channel - no channel designator - non vendor specific */ 
  status = ZID_SendNwkFrame(&msg);

  if ((status == RF4CE_SAP_PENDING) && 
      (state == HID_STATE_IDLE))
    ZID_SetMessageSent();

  return status;
}

/**@brief  The push attributes command frame allows a node to push attributes to 
  *        a remote node.
  *@param  param The structure containing the Push Attributes parameters
  *@return command result. Valid value are:
  *        - RF4CE_SAP_PENDING
  *        - RF4CE_BUSY
  *        - RF4CE_INVALID_CALL
  */
uint32_t GDP_PushAttributes(PushAttributes_Type *param)
{  
  uint32_t state, status;
  uint8_t data[128];
  ZID_NwkFrame_Type msg;

  /* Host not support this HID command */
  if (isHIDAdapter())
    return RF4CE_INVALID_CALL;

  state = GET_CURRENT_STATE();

  if ((state != HID_STATE_IDLE) && (state != HID_STATE_CONFIGURATION))
    return RF4CE_BUSY;

  if (!isValidPairRef(param->pairRef))
    return NO_PAIRING;

  msg.pairingRef = param->pairRef;
  msg.profileId = 0x02; /* ZID ZigBee Input Device */
  data[0] = PUSH_ATTRIBUTES_CODE | ZID_GDP_COMMAND_FRAME;
  memcpy(&data[1], param->attributesRec, param->attributesRecLength);
  msg.data = data;
  msg.dataLen = param->attributesRecLength + 1;
  msg.TxOptions = RF4CE_DATA_REQ_ACK_MODE | /* Unicast - nwk address - ack - security - */ 
                  RF4CE_DATA_REQ_SEC_MODE ;/* multiple channel - no channel designator - non vendor specific */ 
  status = ZID_SendNwkFrame(&msg);

  if ((status == RF4CE_SAP_PENDING) && (state != HID_STATE_CONFIGURATION)) {
    ZID_WaitCmdRsp(param->pairRef, (PUSH_ATTRIBUTES_CODE|ZID_GDP_COMMAND_FRAME));
  }

  if ((status == RF4CE_SAP_PENDING) && 
      (state == HID_STATE_IDLE))
    ZID_SetMessageSent();
  
  return status;
}

/**@brief  The get report command frame allows the HID adaptor to request 
  *        a report from a HID class device.
  *@param  param The structure containing the Get Report parameters
  *@retval command result. Valid value are:
  *        - RF4CE_SAP_PENDING
  *        - RF4CE_BUSY
  *        - RF4CE_INVALID_CALL
  */
#ifdef ZID_FULL
uint32_t ZID_GetReport(GetReport_Type *param)
{
  uint32_t state, status;
  uint8_t data[3];
  ZID_NwkFrame_Type msg;

  /* Device not support this HID command */
  if (!isHIDAdapter())
    return RF4CE_INVALID_CALL;

  state = GET_CURRENT_STATE();

  if (state != HID_STATE_IDLE)
    return RF4CE_BUSY;

  if (!isValidPairRef(param->pairRef))
    return NO_PAIRING;

  msg.pairingRef = param->pairRef;
  msg.profileId = 0x02; /* ZID ZigBee Input Device */
  data[0] = GET_REPORT_CODE;
  if (param->dataPending) {
    data[0] |= ZID_DATA_PENDING;
  }
  data[1] = param->reportType;
  data[2] = param->reportId;
  msg.data = data;
  msg.dataLen = sizeof(data);
  msg.TxOptions = RF4CE_DATA_REQ_ACK_MODE | /* Unicast - nwk address - ack - security - */ 
                  RF4CE_DATA_REQ_SEC_MODE ;/* multiple channel - no channel designator - non vendor specific */ 
  status = ZID_SendNwkFrame(&msg);

  if (status == RF4CE_SAP_PENDING) {
    ZID_WaitCmdRsp(param->pairRef, GET_REPORT_CODE);
  }
  
  return status;
}
#endif

/**@brief  The report data command frame allows a remote node to respond 
  *        to a get report command frame or to send an unsolicited report.
  *@param  param The structure containing the Report Data parameters
  *@retval command result. Valid value are:
  *        - RF4CE_SAP_PENDING
  *        - RF4CE_BUSY
  */
uint32_t ZID_ReportData(ReportData_Type *param)
{
  uint32_t state, status=RF4CE_SAP_PENDING;
  uint8_t data[128];
  ZID_NwkFrame_Type msg;
  bool security;

  state = GET_CURRENT_STATE();

  if (state != HID_STATE_IDLE)
    return RF4CE_BUSY;

  if (!isValidPairRef(param->pairRef))
    return NO_PAIRING;

  msg.pairingRef = param->pairRef;
  msg.profileId = 0x02; /* ZID ZigBee Input Device */
  data[0] = REPORT_DATA_CODE;
  if (param->dataPending && isHIDAdapter()) {
    data[0] |= ZID_DATA_PENDING;
  }
  memcpy(&data[1], param->reportDataRecords, param->reportPayloadLength);
  msg.data = data;
  msg.dataLen = param->reportPayloadLength + 1;
  security = param->txOptions & TX_SECURITY_ENABLED;
  switch(param->txOptions & 0x07) {
  case TX_CONTROL_PIPE_UNICAST:
    {
      msg.TxOptions = RF4CE_DATA_REQ_ACK_MODE; /* Unicast - nwk address - ack - multiple channel - no channel designator - non vendor specific */
      if (security) 
	msg.TxOptions |= RF4CE_DATA_REQ_SEC_MODE ; /* Security */  
    }
    break;
  case TX_CONTROL_PIPE_BROADCAST:
    {
      msg.TxOptions = RF4CE_DATA_REQ_TX_MODE; //Broadcast
    }
    break;
  case TX_INTERRUPT_PIPE_UNICAST:
    {  
      if (safeTx(param->pairRef))
	msg.TxOptions = RF4CE_DATA_REQ_ACK_MODE; //Unicast - nwk address - ack - multiple channel - no channel designator - non vendor specific 
      else
	msg.TxOptions = RF4CE_DATA_REQ_CHANNEL_AGILITY_MODE; //Unicast-nwk address-NO ACK-single channel-no channel designator-non vendor specific 
      
      if (security)
	msg.TxOptions |= RF4CE_DATA_REQ_SEC_MODE ; //Security 
    }
    break;
  default:
    {
      return ZID_WRONG_PARAMETER;
    }
  }  
  if (rptToSend(param))
    status = ZID_SendNwkFrame(&msg);

  if ((status == RF4CE_SAP_PENDING) && 
      (state == HID_STATE_IDLE))
    ZID_SetMessageSent();
  
#ifdef TEST
  if (status == 0x0100)
    reportData = TRUE;
#endif

  return status;
}

/**@brief  The set report command frame allows a node to send 
  *        a report to a remote node.
  *@param  param The structure containing the Set Report parameters
  *@retval command result. Valid value are:
  *        - RF4CE_SAP_PENDING
  *        - RF4CE_BUSY
  *        - RF4CE_INVALID_CALL
  */
uint32_t ZID_SetReport(SetReport_Type *param)
{
  uint32_t state, status;
  uint8_t data[128];
  ZID_NwkFrame_Type msg;

  state = GET_CURRENT_STATE();

  /* Device not support this HID command */
  if (!isHIDAdapter() && (state != HID_STATE_CONFIGURATION))
    return RF4CE_INVALID_CALL;

  if ((state != HID_STATE_IDLE) && (state != HID_STATE_CONFIGURATION))
    return RF4CE_BUSY;

  if (!isValidPairRef(param->pairRef))
    return NO_PAIRING;

  msg.pairingRef = param->pairRef;
  msg.profileId = 0x02; /* ZID ZigBee Input Device */
  data[0] = SET_REPORT_CODE;
  if (param->dataPending) {
    data[0] |= ZID_DATA_PENDING;
  }
  data[1] = param->reportType;
  data[2] = param->reportId;
  memcpy(&data[3], param->reportData, param->reportDataLen);
  msg.data = data;
  msg.dataLen = param->reportDataLen + 3;
  msg.TxOptions = RF4CE_DATA_REQ_ACK_MODE | /* Unicast - nwk address - ack - security - */ 
                  RF4CE_DATA_REQ_SEC_MODE ;/* multiple channel - no channel designator - non vendor specific */ 
  status = ZID_SendNwkFrame(&msg);

  if (status == RF4CE_SAP_PENDING) {
    ZID_WaitCmdRsp(param->pairRef, SET_REPORT_CODE);
  }

  return status;
}

/**@brief  The generic response command frame allows a node to issue a response 
  *        to a command frame that does not have a corresponding response frame.
  *@param  param The structure containing the Generic Response parameters
  *@retval command result. Valid value are:
  *        - RF4CE_SAP_PENDING
  *        - RF4CE_BUSY
  */
bool ZID_EnablePowerSave(void)
{
  if ((GET_CURRENT_STATE() == HID_STATE_IDLE) && internalEnablePowerSave())
    return TRUE;
  else
    return FALSE;
}

/**@brief  The generic response command handler frame allows 
  *        a node to issue a response to a command frame that does 
  *        not have a corresponding response frame.
  *@param  param The structure containing the Generic Response parameters
  *@return Nothing
  */
void GDP_GenericResponseHandler(GenericResponse_Type *param)
{
  uint8_t cmdCode;
  
  cmdCode = GENERIC_RESPONSE_CODE | ZID_GDP_COMMAND_FRAME; 

  if (param->dataPending && !isHIDAdapter())
    ZID_SetHDP(TRUE);

  ZID_CommandNotification(cmdCode, param);
}

/**@brief  The configuration complete command handler frame allows a node 
  *        to indicate that the current profile configuration step is complete.
  *@param  param The structure containing the Configuration Complete parameters
  *@return None
  */
#ifdef ZID_FULL
void GDP_ConfigurationCompleteHandler(ConfigurationComplete_Type *param) 
{
  uint8_t cmdCode;

  cmdCode = CONFIGURATION_COMPLETE_CODE | ZID_GDP_COMMAND_FRAME;

  ZID_CommandNotification(cmdCode, param);
}

/**@brief  The heartbeat command handler frame allows the recipient the opportunity
  *        to send a command frame back to the originator. 
  *@param  param The structure containing the heart beat parameters
  *@return None
  */
void GDP_HeartBeatHandler(HeartBeat_Type *param) 
{
  uint8_t cmdCode;

  cmdCode = HEARTBEAT_CODE | ZID_GDP_COMMAND_FRAME;

  ZID_CommandNotification(cmdCode, param);
}
#endif /* ZID_FULL */

/**@brief  The get attributes command handler frame allows the recipient to process 
  *        each specified attribute identifier and respond with a get attributes response 
  *        command frame.
  *@param  param The structure containing the Get Attributes parameters
  *@return None
  */
void GDP_GetAttributesHandler(GetAttributes_Type *param)
{
  GetAttributesResponse_Type response;
  deviceAttributeInfo_Type deviceAttr;
  uint32_t res, state;
  uint8_t listVal[84];
  uint8_t attrRecord[128];
  ATTR_Status status;
  uint8_t i, index;
  bool isList;
  uint16_t idVal;

  state = GET_CURRENT_STATE();

  if (!isHIDAdapter() && param->dataPending)
    ZID_SetHDP(TRUE);

  deviceAttr.proxyAttr = FALSE;

  /* Collect all the attributes information */
  index = 0;
  for (i = 0; i < param->attrListLength; i++) {
    if ((param->attrId[i] != aplHIDStdDescCompsList_ID) &&
	(param->attrId[i] <= aplHIDNumNonStdDescComps_ID)) {
      deviceAttr.value = (uint8_t*)&idVal;
      isList = FALSE;
    }else {
      deviceAttr.value = listVal;
      isList = TRUE;
    }
    deviceAttr.id = param->attrId[i];
    status = ZID_GetDeviceAttr(&deviceAttr);
    /* Process the attribute received only if it isn't an aplHID attribute */
    /* and the state is different from CONFIGURATION */
    if ((param->attrId[i] >= aplHIDParserVersion_ID)  && 
	(state != HID_STATE_CONFIGURATION) && (status == HID_ATTR_SUCCESS)) {
      status = HID_ATTR_ILLEGAL_REQ;
    } 
    attrRecord[index++] = deviceAttr.id;
    if ((param->attrId[i] >= aplHIDNonStdDescCompSpec_MIN_ID) && 
	(param->attrListLength > 1) && 
	(status == HID_ATTR_SUCCESS)) {
      status = HID_ATTR_ILLEGAL_REQ;
    }
    attrRecord[index++] = status;
    if (status == HID_ATTR_SUCCESS) {
      attrRecord[index++] = deviceAttr.length;
      if(isList) {
	memcpy(&attrRecord[index], listVal, deviceAttr.length);
      } else {
	memcpy(&attrRecord[index], &idVal, deviceAttr.length);
      }
      index += deviceAttr.length;
    } /* ZID_GetDeviceAttr == SUCCESS */
  } /* end for */

  /* Send get attributes response command */
  response.pairRef = param->pairRef;
  response.dataPending = FALSE;
  response.statusRecLength = index;
  response.statusRec = attrRecord;
  res = GDP_GetAttributesResponse(&response);
  if (res != RF4CE_SAP_PENDING) {
    CommandDeliveryStatus_Type delivery;
    
    delivery.commandCode = GET_ATTRIBUTES_RESPONSE_CODE;
    delivery.status = res;
    delivery.pairingRef = param->pairRef;
    ZID_CommandDeliveryStatus(&delivery);
  }
}

/**@brief  The get attributes response handler command frame allows the recipient 
  *        to notify the results of its original get attributes request.
  *@param  param The structure containing the Get Attributes Response parameters
  *@return Nothing
  */
void GDP_GetAttributesResponseHandler(GetAttributesResponse_Type *param)
{
  uint8_t index, status, cmdCode;
  deviceAttributeInfo_Type attr;

  index = 0;
  if (isHIDAdapter()) {
    attr.proxyAttr = TRUE;
    attr.proxyRef = param->pairRef;
    while(index < param->statusRecLength) {
      attr.id = param->statusRec[index];
      status = param->statusRec[index+1];
      index += 2;
      if (status == 0) {
	attr.length = param->statusRec[index];
	index++;
	attr.value = &param->statusRec[index];
	ZID_SetDeviceAttr(&attr);
	index += attr.length;
      }
    }
  } else {
    if (param->dataPending)
      ZID_SetHDP(TRUE);
  }
  cmdCode = GET_ATTRIBUTES_RESPONSE_CODE  | ZID_GDP_COMMAND_FRAME;
  ZID_CommandNotification(cmdCode, param);
}

/**@brief  The push attributes handler frame allows 
  *        a node to receive the attributes 
  *        from a remote node.
  *@param  param The structure containing the Push Attributes parameters
  *@return Nothing
  */
#ifdef ZID_FULL
void GDP_PushAttributesHandler(PushAttributes_Type *param)
{
  GenericResponse_Type genRsp;
  deviceAttributeInfo_Type attr;
  uint8_t index;
  uint32_t status, state;

  state = GET_CURRENT_STATE();

  genRsp.pairRef = param->pairRef;
  genRsp.dataPending = 0;
  genRsp.rspCode = ZID_SUCCESS;
  index = 0;
  while ((index < param->attributesRecLength) &&
	 (genRsp.rspCode == ZID_SUCCESS)) {
    
    /* Process the attribute received only if it isn't an aplHID attribute */ 
    /* and the state is different from CONFIGURATION */
    if ((param->attributesRec[index] >= aplHIDParserVersion_ID) && 
	(state != HID_STATE_CONFIGURATION)) 
      genRsp.rspCode = ZID_INVALID_PARAMETER;

    /* Check also if are report descriptor inside the request */
    if ((param->attributesRec[index] >= aplHIDNonStdDescCompSpec_MIN_ID) &&
	((param->attributesRec[index+1]+2) != param->attributesRecLength))
      genRsp.rspCode = ZID_INVALID_PARAMETER;

    /* Set the attributes value */
    if (genRsp.rspCode == ZID_SUCCESS) {
      attr.proxyAttr = TRUE;
      attr.proxyRef = param->pairRef;
      attr.id = param->attributesRec[index];
      attr.length = param->attributesRec[index+1];
      attr.value = &param->attributesRec[index+2];
      if (ZID_SetDeviceAttr(&attr) != HID_ATTR_SUCCESS)
	genRsp.rspCode = ZID_INVALID_PARAMETER;
    }
    index += attr.length + 2;
  }
  
  status = GDP_GenericResponse(&genRsp);
  if (status != RF4CE_SAP_PENDING) {
    CommandDeliveryStatus_Type delivery;
    
    delivery.commandCode = GENERIC_RESPONSE_CODE;
    delivery.status = status;
    delivery.pairingRef = param->pairRef;
    ZID_CommandDeliveryStatus(&delivery);
  }
}
#endif /* ZID_FULL */

/**@brief  The get report command handler frame allows a HID class device 
  *        to receive a report request from a HID adaptor.
  *@param  param The structure containing the Get Report parameters
  *@return None
  */
void ZID_GetReportHandler(GetReport_Type *param) 
{
  uint8_t status, cmdCode;
  uint32_t res;
  GenericResponse_Type genRsp;


  status = verifyRptInfo(param->reportType, param->reportId);
  
  if (status != ZID_SUCCESS) {
    genRsp.pairRef = param->pairRef;
    genRsp.dataPending = FALSE;
    genRsp.rspCode = (GRSP_Status)status;
    res = GDP_GenericResponse(&genRsp);
    if (res != RF4CE_SAP_PENDING) {
      CommandDeliveryStatus_Type delivery;
      
      delivery.commandCode = REPORT_DATA_CODE;
      delivery.status = res;
      delivery.pairingRef = param->pairRef;
      ZID_CommandDeliveryStatus(&delivery);
    }
  } else {
    cmdCode = GET_REPORT_CODE;
    ZID_CommandNotification(cmdCode, param);
  }

  if (param->dataPending)
    ZID_SetHDP(TRUE);
}

/**@brief  The report data command handler frame allows a remote node 
  *        to receive the response to a get report command frame or 
  *        to receive an unsolicited report.
  *@param  param The structure containing the Report Data parameters
  *@return None
  */
void ZID_ReportDataHandler(ReportData_Type *param) 
{
  uint8_t cmdCode;
#if defined(ZID_FULL) && !defined(TEST)
  uint8_t index, rptId = 0, rptLen = 0, *rptData = 0;
  bool rptOk;

  if (isHIDAdapter()) {
    index = 0;
    while(index < param->reportPayloadLength) {
      rptOk = TRUE;
      rptLen = param->reportDataRecords[index] - 2;
      rptId = param->reportDataRecords[index+2];
      rptData = &(param->reportDataRecords[index+3]);
      /* Keyboard and keypad device shall transmit report with security enabled */
      if ((rptId == 0x02) && (param->txOptions != TX_SECURITY_ENABLED))
	rptOk = FALSE;
      if (rptOk) 
	HID_ClassDriver_USBRptDataNotify(param->pairRef, rptId, rptData, rptLen);
      index += rptLen + 3;
    }
    ZID_SetReportToRepeat(param->pairRef, rptId, rptData, rptLen);
  }
#endif
  if (!isHIDAdapter()) {
    if (param->dataPending)
      ZID_SetHDP(TRUE);
  }
  cmdCode = REPORT_DATA_CODE;
  ZID_CommandNotification(cmdCode, param);
}  

/**@brief  The set report command handler frame allows a node to 
  *        receive a report from a remote node.
  *@param  param The structure containing the Set Report parameters
  *@return None
  */
void ZID_SetReportHandler(SetReport_Type *param) 
{
  GenericResponse_Type genRsp;
  uint32_t res;
  uint8_t status, cmdCode;

  status = verifyRptInfo(param->reportType, param->reportId);

  genRsp.pairRef = param->pairRef;
  genRsp.dataPending = FALSE;
  genRsp.rspCode = (GRSP_Status)status;
  res = GDP_GenericResponse(&genRsp);
  if (res != RF4CE_SAP_PENDING) {
    CommandDeliveryStatus_Type delivery;
    
    delivery.commandCode = GENERIC_RESPONSE_CODE;
    delivery.status = res;
    delivery.pairingRef = param->pairRef;
    ZID_CommandDeliveryStatus(&delivery);
  } 

  cmdCode = SET_REPORT_CODE;
  ZID_CommandNotification(cmdCode, param);

  if (param->dataPending)
    ZID_SetHDP(TRUE);
}


/* ZRC Support */

/**@brief  This function send the ZRC command discovery request to the remote node
  *@param  param the structure with the command discovery request parameters
  *@retval command result. Valid value are:
  *        - RF4CE_SAP_PENDING
  *        - RF4CE_BUSY
  */
uint32_t ZRC_CommandDiscoveryRequest(CommandDiscoveryRequest_Type *param)
{
  uint32_t state, status;
  uint8_t data[2];
  ZID_NwkFrame_Type msg;

  state = GET_CURRENT_STATE();

  if ((state != HID_STATE_IDLE) && (state != HID_STATE_CONFIGURATION))
    return RF4CE_BUSY;

  if (!isValidPairRef(param->pairRef))
    return NO_PAIRING;

  msg.pairingRef = param->pairRef;
  msg.profileId = 0x01; /* ZRC ZigBee Remote Control */
  data[0] = COMMAND_DISCOVERY_REQUEST_CODE;
  data[1] = 0;
  msg.data = data;
  msg.dataLen = sizeof(data);
  msg.TxOptions = RF4CE_DATA_REQ_ACK_MODE | /* Unicast - nwk address - ack - security - */ 
                  RF4CE_DATA_REQ_SEC_MODE ;/* multiple channel - no channel designator - non vendor specific */ 
  status = ZID_SendNwkFrame(&msg);

  if (status == RF4CE_SAP_PENDING) {
    ZID_WaitCmdRsp(param->pairRef, COMMAND_DISCOVERY_REQUEST_CODE);
  }
  
  if ((status == RF4CE_SAP_PENDING) && 
      (state == HID_STATE_IDLE))
    ZID_SetMessageSent();

  return status;  
}

/**@brief  This function send the ZRC command discovery response to the remote node
  *@param  param the structure with the command discovery response parameters
  *@retval command result. Valid value are:
  *        - RF4CE_SAP_PENDING
  *        - RF4CE_BUSY
  */
uint32_t ZRC_CommandDiscoveryResponse(CommandDiscoveryResponse_Type *param)
{
  uint32_t state, status;
  uint8_t length;
  uint8_t data[34];
  ZID_NwkFrame_Type msg;

  state = GET_CURRENT_STATE();

  if ((state != HID_STATE_IDLE) && (state != HID_STATE_CONFIGURATION))
    return RF4CE_BUSY;

  if (!isValidPairRef(param->pairRef))
    return NO_PAIRING;

  memset(data, 0, sizeof(data));
  ZRC_GetCommandSupported(&data[2], &length);
  msg.pairingRef = param->pairRef;
  msg.profileId = 0x01; /* ZRC ZigBee Remote Control */
  data[0] = COMMAND_DISCOVERY_RESPONSE_CODE;
  data[1] = 0;
  msg.data = data;
  msg.dataLen = sizeof(data);
  msg.TxOptions = RF4CE_DATA_REQ_ACK_MODE | /* Unicast - nwk address - ack - security - */ 
                  RF4CE_DATA_REQ_SEC_MODE ;/* multiple channel - no channel designator - non vendor specific */ 
  status = ZID_SendNwkFrame(&msg);

  if ((status == RF4CE_SAP_PENDING) && 
      (state == HID_STATE_IDLE))
    ZID_SetMessageSent();

  return status;  
}

/**@brief  This function allows a node to request a remote node performs 
  *        the specified RC (HDMI CEC) command, using specified tx options.
  *@param  param the structure with the user control pressed parameters
  *@retval command result. Valid value are:
  *        - RF4CE_SAP_PENDING
  *        - RF4CE_BUSY
  */ 
uint32_t ZRC_UserControlPressed(UserControlPressed_Type *param)
{
  uint32_t state, status=RF4CE_SAP_PENDING;
  uint8_t data[128];
  ZID_NwkFrame_Type msg;
  bool security, send=TRUE;

  state = GET_CURRENT_STATE();

  if (state != HID_STATE_IDLE)
    return RF4CE_BUSY;

  if (!isValidPairRef(param->pairRef))
    return NO_PAIRING;

  msg.pairingRef = param->pairRef;
  msg.profileId = 0x01; /* ZRC ZigBee Remote Control */
  data[0] = USER_CONTROL_PRESSED_CODE;
  data[1] = param->commandCode;
  memcpy(&data[2], param->commandPayload, param->payloadLength);
  msg.data = data;
  msg.dataLen = param->payloadLength + 2;
  security = param->txOptions & ZRC_TX_SECURITY;
  switch(param->txOptions & 0x07) {
  case ZRC_TX_ACK_UNICAST:
    {
      msg.TxOptions = RF4CE_DATA_REQ_ACK_MODE; /* Unicast - nwk address - ack - multiple channel - no channel designator - non vendor specific */
      if (security) 
	msg.TxOptions |= RF4CE_DATA_REQ_SEC_MODE ; /* Security */  
    }
    break;
  case ZRC_TX_NO_ACK_UNICAST:
    {
      msg.TxOptions = RF4CE_DATA_REQ_CHANNEL_AGILITY_MODE; /* Unicast-nwk address-NO ACK-single channel-no channel designator-non vendor specific */
      if (security)
	msg.TxOptions |= RF4CE_DATA_REQ_SEC_MODE ; /* Security */ 
    }
    break;
  case ZRC_TX_BROADCAST:
    {  
      msg.TxOptions = RF4CE_DATA_REQ_TX_MODE; /* Broadcast */
    }
    break;
  default:
    {
      return ZID_WRONG_PARAMETER;
    }
  }  

  if (param->txOptions & ZRC_TX_KEY_REPEAT) {
    if ((lastCommand.code == param->commandCode) && 
	(memcmp(lastCommand.payload, param->commandPayload, lastCommand.length) == 0)) {
      deviceAttributeInfo_Type attr;
      uint32_t rptInterval;
      
      attr.proxyAttr = FALSE;
      attr.id = aplKeyRepeatInterval_ID;
      attr.value = (uint8_t *)&rptInterval;
      ZID_GetDeviceAttr(&attr);
      if (TIME_ELAPSED(lastCommand.startTime) < rptInterval)
	send = FALSE;
      else
	lastCommand.startTime = TIME_CurrentTime();
      data[0] = USER_CONTROL_REPEATED_CODE; 
    } else {
      lastCommand.code = param->commandCode;
      lastCommand.length = param->payloadLength;
      memcpy(lastCommand.payload, param->commandPayload, param->payloadLength);
      lastCommand.startTime = TIME_CurrentTime();
    }
  }

  if (send)
    status = ZID_SendNwkFrame(&msg);

  if (send && (status == RF4CE_SAP_PENDING) && 
      (state == HID_STATE_IDLE))
    ZID_SetMessageSent();

  return status;
}

/**@brief  This function communicates that the specified RC (HDMI CEC)
  *        command is released, using specified tx options. This command is optional,
  *        the application can decide if send it or not.
  *@param  param the structure with the user control released parameters
  *@retval command result. Valid value are:
  *        - RF4CE_SAP_PENDING
  *        - RF4CE_BUSY
  */ 
uint32_t ZRC_UserControlReleased(UserControlReleased_Type *param) {
  uint32_t state, status=RF4CE_SAP_PENDING;
  uint8_t data[2], retryNumber;
  ZID_NwkFrame_Type msg;
  bool security;

  state = GET_CURRENT_STATE();

  if (state != HID_STATE_IDLE)
    return RF4CE_BUSY;

  if (!isValidPairRef(param->pairRef))
    return NO_PAIRING;

  /* Last Command released */
  lastCommand.code = 0;
  lastCommand.length = 0;

  msg.pairingRef = param->pairRef;
  msg.profileId = 0x01; /* ZRC ZigBee Remote Control */
  data[0] = USER_CONTROL_RELEASED_CODE;
  data[1] = param->commandCode;
  msg.data = data;
  msg.dataLen = 2;
  security = param->txOptions & ZRC_TX_SECURITY;
  switch(param->txOptions & 0x07) {
  case ZRC_TX_ACK_UNICAST:
    {
      msg.TxOptions = RF4CE_DATA_REQ_ACK_MODE; /* Unicast - nwk address - ack - multiple channel - no channel designator - non vendor specific */
      if (security) 
	msg.TxOptions |= RF4CE_DATA_REQ_SEC_MODE ; /* Security */  
    }
    break;
  case ZRC_TX_NO_ACK_UNICAST:
    {
      msg.TxOptions = RF4CE_DATA_REQ_CHANNEL_AGILITY_MODE; /* Unicast-nwk address-NO ACK-single channel-no channel designator-non vendor specific */ 
      if (security)
	msg.TxOptions |= RF4CE_DATA_REQ_SEC_MODE ; /* Security */ 
    }
    break;
  case ZRC_TX_BROADCAST:
    {  
      msg.TxOptions = RF4CE_DATA_REQ_TX_MODE; /* Broadcast */
    }
    break;
  default:
    {
      return ZID_WRONG_PARAMETER;
    }
  }  

  /* The key release need to be sent */
  retryNumber = 4;
  do {
    status = ZID_SendNwkFrame(&msg);

    if ((status == RF4CE_SAP_PENDING) && 
	(state == HID_STATE_IDLE))
      ZID_SetMessageSent();
    else {
      uint32_t startTime;
      startTime = TIME_CurrentTime();
      /* Wait 10 msec for each retry ??? */ 
      while(TIME_ELAPSED(startTime) <= 10000)
	ZID_Tick();
    }
    retryNumber--;
  } while ((status != RF4CE_SAP_PENDING) && (retryNumber > 0));

  return status;
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
