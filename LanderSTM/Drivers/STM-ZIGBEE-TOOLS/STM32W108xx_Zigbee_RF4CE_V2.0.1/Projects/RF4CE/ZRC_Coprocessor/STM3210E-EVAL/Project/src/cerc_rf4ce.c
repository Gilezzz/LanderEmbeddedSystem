/**
  ******************************************************************************
  * @file    cerc_rf4ce.c
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   This file provides the CERC RF4CE profile implemenation
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
#include "rf4ce.h"
#include "cerc_rf4ce.h"
#include "rf4ce.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
typedef struct CERC_Context_S {
  /* device type */
  bool isTarget;
  /* state of the CERC state machine */
  uint8_t state;
  /* Number of key to exchange during pairing with security */
  uint8_t numberOfKey;
  /* Device type supported by the node issuing this primitive */
  uint8_t devType;
  /* Number of nodes discovered from primitive Discovery_request ??? Non utilizzata */
  uint8_t numNodes; 
  /* Channel of the device responding to Discovery_request/Pair_indication */
  uint8_t logicalChannel; 
  /* PAN identifier of the device responding to Discovery_request/Pair_indication */
  uint16_t PANId; 
  /* IEEE address of the device responding to Discovery_request/Pair_indication */
  IEEEAddr longAddr; 
  /* Start time to wait for a command */
  uint32_t startTime;
  /* Pairing reference */
  uint8_t pairingRef;
  /* Command Code */
  uint8_t commandKeyCode[2];
  /* Active Time */
  uint32_t startIdleTime;
  /* Flag to indicate if the power save can be enabled */
  bool enablePowerSave;
} CERC_Context_Type;

/* Private define ------------------------------------------------------------*/
/* State for the CER state machine */
#define CERC_IDLE                      0x00
#define CERC_WAIT_DSC_PAIR_CONFIRM     0x01
#define CERC_SEND_PAIR_REQUEST         0x02
#define CERC_WAIT_DSC_PAIR_INDICATION  0x03
#define CERC_WAIT_TX_HANDLER           0x04
#define CERC_WAIT_NWK_START_CONFIRM    0x05
#define CERC_SEND_COMMAND_REPEATED     0x06

/* Max wait time to accept pair indication from remote node */
#define TARGET_WAIT_PAIR_IDNCIATION    0x1c9c38 // 30 sec implementation specific
#define CONTROLLER_WAIT_RESPONSE_TIME  0x098968 // 10 sec implementation specific

/* If the device is in IDLE state for 250 ms goes in deep sleep mode 2 */
#define MAX_IDLE_TIME 250000 

/* Private macro -------------------------------------------------------------*/
extern __IO int timeTick_ms;

/* Public variables ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static CERC_Context_Type cercContext;
static uint8_t cmd_repeated = CERC_USER_CONTROL_REPEATED;

/* Private function prototypes -----------------------------------------------*/
static void CERC_SendPair(void);

/* Private functions ---------------------------------------------------------*/
#define ASCII_XON         0x11  /* requests host to pause sending */ 
#define ASCII_XOFF        0x13  /* requests host to resume sending */


/**@brief  stop commands.
  *@param  None
  *@retval None
  */
void hostStopCommands(void)
{
  putchar(ASCII_XOFF);
}

/**@brief  start commands.
  *@param  None
  *@retval None
  */
void hostStartCommands(void)
{
    putchar(ASCII_XON);
}

extern __IO int timeTick1_ms;

/**@brief  set the current time.
  *@param  None
  *@retval current time in ms
  */
static int TIME_CurrentTime(void)
{
  return timeTick1_ms;
}

/**@brief  get the elapsed time.
  *@param  time
  *@retval elapsed time in ms
  */
static int TIME_ELAPSED(int time)
{
  if (timeTick1_ms < time)
    printf("Error\r\n");
  return (timeTick1_ms - time);
}

/* Public functions ---------------------------------------------------------*/

/** 
  *@brief  The CERC_Init function initializes the NWK and
  *        it configures the NIB attibutes 
  *        according to CERC profile.
  *@param  param The structure containing the CERC_Init parameters
  *@retval The status of the CERC Init.
  */
uint32_t CERC_Init(CERC_Init_Type *param)
{
  uint32_t status, app;
  NLME_SET_Type val;

  cercContext.isTarget = (bool)(param->nodeCap & 0x01);

  if (param->coldStart) {
    uint16_t myVendorId = 0xFFF2;
    NWK_Config(nwkcVendorIdentifier_ID, &myVendorId);
    NWK_Config(nwkcVendorString_ID, "Vendor");
  }
  /* Start the network and the NIB with the 
   * default value
   */  
  status = NWK_Init(param->nodeCap, param->coldStart);
  if ((status != SUCCESS) && (status != RF4CE_SAP_PENDING))
    return status;

  /* Set the CERC profile 
   * initial NIB attribute
   */
  val.NIBAttribute = nwkActivePeriod_ID;
  val.NIBAttributeIndex = 0;
  val.NIBAttributeValue = (uint8_t *)&param->activePeriod;
  NLME_Set(&val);

  val.NIBAttribute = nwkDiscoveryLQIThreshold_ID;
  val.NIBAttributeIndex = 0;
  val.NIBAttributeValue = (uint8_t *)&param->discoveryLQIThreshold;
  NLME_Set(&val);

  val.NIBAttribute = nwkDiscoveryRepetitionInterval_ID;
  val.NIBAttributeIndex = 0;
  app = 0x00f424; /* 1s */
  val.NIBAttributeValue = (uint8_t *)&app;
  NLME_Set(&val);

  val.NIBAttribute = nwkDutyCycle_ID;
  val.NIBAttributeIndex = 0;
  val.NIBAttributeValue = (uint8_t *)&param->dutyCycle;
  NLME_Set(&val);

  val.NIBAttribute = nwkMaxDiscoveryRepetitions_ID;
  val.NIBAttributeIndex = 0;
  app = 0x1e;
  val.NIBAttributeValue = (uint8_t *)&app;
  NLME_Set(&val);

  val.NIBAttribute = nwkMaxReportedNodeDescriptors_ID;
  val.NIBAttributeIndex = 0;
  app = 0x1;
  val.NIBAttributeValue = (uint8_t *)&app;
  NLME_Set(&val);

  val.NIBAttribute = nwkUserString_ID;
  val.NIBAttributeIndex = 0;
  if (cercContext.isTarget)
    val.NIBAttributeValue = "STM32W Target";
  else
    val.NIBAttributeValue = "STM32W RC";
  NLME_Set(&val);

  if (status == SUCCESS) 
    cercContext.state = CERC_IDLE;

  if (status == RF4CE_SAP_PENDING) 
    cercContext.state = CERC_WAIT_NWK_START_CONFIRM;

  cercContext.startTime = 0;
  cercContext.startIdleTime = 0;
  cercContext.enablePowerSave = FALSE;
  memset(cercContext.commandKeyCode, 0xff, sizeof(cercContext.commandKeyCode));

  return status;
}

/**@brief  The CERC_DscPair
  *        function manages the case of 
  *        discovery/pairing procedure for Controller
  *        and Target
  *@param  param The structure containing the 
  *        CERC_DscPair_Type parameters
  *@retval The status of the CERC Discovery/Pair function call.
  */
uint32_t CERC_DscPair(CERC_DscPair_Type *param)
{
  uint32_t status = SUCCESS;

  if (cercContext.state != CERC_IDLE)
    return CERC_INVALID_STATE;

  cercContext.devType = param->devType;

  if ((cercContext.isTarget) && !param->pairTT) {
    NLME_AUTO_DISCOVERY_REQUEST_Type dscParam;
    dscParam.RecAppCapabilities = 0x13;
    dscParam.RecDevTypeList[0] = param->devType; 
    dscParam.RecProfileIdList[0] = 0x01; /* Consumer Electronics Remote control */
    dscParam.AutoDiscDuration = 0x1c9c38; /* 30 sec */
    status = NLME_AUTO_DISCOVERY_request(&dscParam);  
    cercContext.startTime = TIME_CurrentTime();
  } else {
    NLME_DISCOVERY_REQUEST_Type dscParam;
    dscParam.DstPANId = 0xffff;
    dscParam.DstNwkAddr = 0xffff;
    dscParam.OrgAppCapabilities = 0x13;
    dscParam.OrgDevTypeList[0] = param->devType;
    dscParam.OrgProfileIdList[0] = 0x01; /* Consumer Electronics Remote control */
    dscParam.SearchDevType = param->searchDevType;
    dscParam.DiscProfileIdListSize = 1;
    dscParam.DiscProfileIdList[0] = 0x01; /* Consumer Electronics Remote control */
    dscParam.DiscDuration = 0x00186a; /* 100ms */
    status = NLME_DISCOVERY_request(&dscParam);
    cercContext.numberOfKey = param->numberOfKey;
  }
  
  if (status != RF4CE_SAP_PENDING)
    cercContext.state = CERC_IDLE; 
  else
    cercContext.state = CERC_WAIT_DSC_PAIR_CONFIRM;
  
  return status;
}

/**@brief  Nlme discovery confirmation
  *@param  NLME discovery confirm
  *@retval None
  */
void NLME_DISCOVERY_confirm (NLME_DISCOVERY_CONFIRM_Type *param)
{
  if ((param->NumNodes > 1) || (param->Status != SUCCESS)) {
    CERC_DscPairCallback_Type cerc_param;
    cerc_param.rf4cePrimitive = CERC_DSC_CONFIRM;
    cerc_param.DscPair.dscConfirm = param;  
    if (param->NumNodes > 1) {
      cerc_param.DscPair.dscConfirm->Status = CERC_MORE_ONE_NODE;
    }
    CERC_DscPairCallback(&cerc_param);
    cercContext.state = CERC_IDLE;
    return; 
  }
  
  cercContext.logicalChannel = param->NodeDescList[0].LogicalChannel;
  cercContext.PANId = param->NodeDescList[0].PANId;
  memcpy(cercContext.longAddr, param->NodeDescList[0].longAddr, sizeof(IEEEAddr));
  cercContext.state = CERC_SEND_PAIR_REQUEST; 
}

/**@brief  NLME auto discovery confirmation
  *@param  NLME auto discovery confirm
  *@retval None
  */
void NLME_AUTO_DISCOVERY_confirm(NLME_AUTO_DISCOVERY_CONFIRM_Type *param)
{
  if (param->Status != SUCCESS) {
    CERC_DscPairCallback_Type cerc_param;
    cerc_param.rf4cePrimitive = CERC_AUTO_DSC_CONFIRM;
    cerc_param.DscPair.autoDscConfirm = param;
    CERC_DscPairCallback(&cerc_param);
    cercContext.state = CERC_IDLE;
    return; 
  }
  cercContext.state = CERC_WAIT_DSC_PAIR_INDICATION;
}

/**@brief  cerc send pairing
  *@param  None
  *@retval None
  */
static void CERC_SendPair(void)
{
  uint32_t status;
  NLME_PAIR_REQUEST_Type param;

  param.LogicalChannel = cercContext.logicalChannel;
  param.DstPANId = cercContext.PANId;
  memcpy(param.DstIEEEAddr, cercContext.longAddr, sizeof(IEEEAddr));
  param.OrgAppCapabilities = 0x13;
  param.OrgDevTypeList[0] = cercContext.devType;
  param.OrgProfileIdList[0] =  0x01; /* Consumer Electronics Remote control */
  if (cercContext.numberOfKey == 0)
    param.KeyExTransferCount = aplKeyExchangeTransferCount_Default;
  else
    param.KeyExTransferCount = cercContext.numberOfKey;

  status = NLME_PAIR_request(&param);

  if(status != RF4CE_SAP_PENDING) {
    CERC_DscPairCallback_Type cerc_param;
    NLME_PAIR_CONFIRM_Type pairConfirm;
    cerc_param.rf4cePrimitive = CERC_PAIR_CONFIRM;
    pairConfirm.Status = status;
    cerc_param.DscPair.pairConfirm = &pairConfirm;
    CERC_DscPairCallback(&cerc_param);
    cercContext.state = CERC_IDLE;
  } else {
    cercContext.state = CERC_WAIT_DSC_PAIR_CONFIRM;
  }
}

/**@brief  NLME pairing confirmation
  *@param  NLME pair confirm type
  *@retval None
  */
void NLME_PAIR_confirm(NLME_PAIR_CONFIRM_Type *params)
{
  CERC_DscPairCallback_Type cerc_param;

  cerc_param.rf4cePrimitive = CERC_PAIR_CONFIRM;
  cerc_param.DscPair.pairConfirm = params;
  CERC_DscPairCallback(&cerc_param);
  cercContext.state = CERC_IDLE;
}

/**@brief  NLME pairing indication
  *@param  NLME pair indication type
  *@retval None
  */
void NLME_PAIR_indication (NLME_PAIR_INDICATION_Type *params)
{
  CERC_DscPairCallback_Type cerc_param;

  if (cercContext.state != CERC_WAIT_DSC_PAIR_INDICATION)
    return; /* Ignore primitive received */
  
  if (params->Status == NO_REC_CAPACITY) {
    cercContext.state = CERC_IDLE;
    return;
  }

  cercContext.PANId = params->SrcPANId;
  cercContext.pairingRef = params->ProvPairingRef;
  memcpy(cercContext.longAddr, params->SrcIEEEAddr, sizeof(IEEEAddr));

  cerc_param.rf4cePrimitive = CERC_PAIR_INDICATION;
  cerc_param.DscPair.pairIndication = params;
  if (CERC_DscPairCallback(&cerc_param))
    CERC_PairRsp();
}

/**@brief  The CERC_Pair_response
  *        maps the RF4CE NLME_PAIR_response()
  *        primitive
  *@param  CERC DscPairCallback Type
  *@retval Status of the function call
  */
uint32_t CERC_PairRsp(void)
{
  uint32_t status=SUCCESS;
  NLME_PAIR_RESPONSE_Type param;

  if (cercContext.state != CERC_WAIT_DSC_PAIR_INDICATION)
    return CERC_INVALID_STATE;

  param.Status = SUCCESS;
  param.DstPANId = cercContext.PANId;
  memcpy(param.DstIEEEAddr, cercContext.longAddr, sizeof(IEEEAddr));
  param.RecAppCapabilities = 0x13;
  param.RecDevTypeList[0] = cercContext.devType;
  param.RecProfileIdList[0] = 0x01; //Consumer Electronics Remote control
  param.ProvPairingRef = cercContext.pairingRef;

  cercContext.startTime = 0;
  status = NLME_PAIR_response(&param);
  if (status != RF4CE_SAP_PENDING) {
    cercContext.state = CERC_IDLE;
    return status;
  }
  cercContext.state = CERC_WAIT_DSC_PAIR_INDICATION;

  return status;
}

/**@brief  NLME comm status indication
  *@param  NLME comm status indication type
  *@retval None
  */
void NLME_COMM_STATUS_indication (NLME_COMM_STATUS_INDICATION_Type *params)
{
  CERC_DscPairCallback_Type cerc_param;

  cerc_param.rf4cePrimitive = CERC_RF4CE_COMM_STATUS_INDICATION;
  cerc_param.DscPair.commStatusIndication = params;
  CERC_DscPairCallback(&cerc_param);

  cercContext.state = CERC_IDLE;
}

/**@brief  The CERC_SendCommand
  *        sends the CERC
  *        command frame
  *@param  Command frame param
  *@retval Status of the function call
  */
uint32_t CERC_SendCommand(CERC_SendCommand_Type *cerc_param)
{
  uint32_t status;
  NLDE_DATA_REQUEST_Type param;

  if (cercContext.state != CERC_IDLE)
    return CERC_DATA_DISCARDED;

  if ((cerc_param->commandCode[0] == CERC_USER_CONTROL_PRESSED) &&
      (memcmp(cercContext.commandKeyCode, &cerc_param->commandCode[1], (cerc_param->commandLength-1)) == 0) &&
      (TIME_ELAPSED(cercContext.startTime) <= ((aplcMaxKeyRepeatInterval*16)/1000))) {
    if(TIME_ELAPSED(cercContext.startTime) < ((aplKeyRepeatInterval_Default*16)/1000)) {
      return SUCCESS;
    } else {
      param.nsduLength = 1;
      param.nsdu = &cmd_repeated;
    }
  } else {
    param.nsduLength = cerc_param->commandLength;
    param.nsdu = cerc_param->commandCode; 
  }

  param.PairingRef = cerc_param->pairingRef;
  param.ProfileId = 0x01; /* Consumer Electronics Remote control */
  param.VendorId = 0xfff1; /* Test vendor id #1 */
  if (cerc_param->pairingRef == 0xff) {
    param.TxOptions = 0x01; /* Broadcast - nwk address - no ack - no security */
                           /* - multiple channel - no channel designator - no vendor specific */
  } else {
    param.TxOptions = 0x0c; /* Unicast - nwk address - ack - security - */ 
                            /* multiple channel - no channel designator - no vendor specific */
  }

  status = NLDE_DATA_request(&param);
  if (status == RF4CE_SAP_PENDING) {
    cercContext.state = CERC_WAIT_TX_HANDLER;
    if (cerc_param->commandCode[0] == CERC_USER_CONTROL_PRESSED)
      memcpy(cercContext.commandKeyCode, &cerc_param->commandCode[1], (param.nsduLength-1));
    else
      memset(cercContext.commandKeyCode, 0xff, sizeof(cercContext.commandKeyCode));
    cercContext.startTime = TIME_CurrentTime();
    status = CERC_DATA_QUEUED;
  } else {
    printf("Error send cmd 0x%08x\r\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n", status);
    cercContext.state = CERC_IDLE;
    status = CERC_DATA_DISCARDED;
  }

  return status;
}

/**@brief  NLDE data confirmation
  *@param  NLDE dta confirm type
  *@retval status
  */
void NLDE_DATA_confirm(NLDE_DATA_CONFIRM_Type *param)
{
  CERC_CommandCallback_Type cerc_param;

  cercContext.state = CERC_IDLE;
  cerc_param.dataCmd.dataConfirm = param;
  cerc_param.rf4cePrimitive = CERC_DATA_CONFIRM;
  CERC_CommandCallback(&cerc_param);
}

/**@brief  NLME start confirmation
  *@param  status
  *@retval None
  */
void NLME_START_confirm (uint32_t *status)
{
  CERC_InitCallback(status);
  cercContext.state = CERC_IDLE;
}

/**@brief  NLDE data indication
  *@param  NLDE data indication
  *@retval None
  */
void NLDE_DATA_indication(NLDE_DATA_INDICATION_Type *param)
{
  CERC_CommandCallback_Type cerc_param;

  cerc_param.rf4cePrimitive = CERC_DATA_INDICATION;
  cerc_param.dataCmd.dataIndication = param;
  CERC_CommandCallback(&cerc_param);
}

/**@brief  NLME unpairing indication
  *@param  pairing reference
  *@retval None
  */
void NLME_UNPAIR_indication (uint8_t *PairingRef)
{
#if 0 /* Not supported from CERC profile specification */
  CERC_DscPairCallback_Type cerc_param;

  cerc_param.pairingRef = *PairingRef;
  cerc_param.rf4cePrimitive = CERC_UNPAIR_INDICATION;
  CERC_DscPairCallback(cerc_param);
  NLME_UNPAIR_response(*PairingRef);
#endif
}

/**@brief  NLME unpairing confirmation
  *@param  NLME unpair confirm type
  *@retval None
  */
void NLME_UNPAIR_confirm (NLME_UNPAIR_CONFIRM_Type *param)
{
#if 0 /* Not supported from CERC profile specification */
  CERC_DscPairCallback_Type cerc_param;

  cerc_param.pairingRef = param->PairingRef;
  cerc_param.status = param->Status;
  cerc_param.rf4cePrimitive = CERC_UNPAIR_CONFIRM;
  CERC_DscPairCallback(cerc_param);  
#endif
}

/**@brief  The CERC_Tick
  *        A tick routine that should be called periodically in the application's main event loop to
  *        check events, advance state machine, invoke callbacks, manage power state, check sleep timer
  *@param  None
  *@retval None
  */
void CERC_Tick(void)
{
  switch (cercContext.state) {
  case CERC_IDLE:
    if ((!cercContext.enablePowerSave) && 
	(cercContext.startIdleTime == 0))
      cercContext.startIdleTime = TIME_CurrentTime();
    break;
  case CERC_SEND_PAIR_REQUEST:
    CERC_SendPair();
    break;
  case CERC_WAIT_NWK_START_CONFIRM:
  case CERC_WAIT_DSC_PAIR_CONFIRM:
    /* Nothing to do */
    break;
  case CERC_WAIT_DSC_PAIR_INDICATION:
    if ((cercContext.startTime != 0) && (TIME_ELAPSED(cercContext.startTime) > ((TARGET_WAIT_PAIR_IDNCIATION*16)/1000))) {
      cercContext.state = CERC_IDLE;
      cercContext.startTime = 0;
    }
    break;
  case CERC_WAIT_TX_HANDLER:
    /* Nothing to do wait a callback */
    break;
  case CERC_SEND_COMMAND_REPEATED:
    break;
  }
}

/**@brief   The CERC_EnablePowerSave
  *         This function return TRUE if the device can enable the power save
  *         FALSE otherwise
  *@param   flag to signal if the board can check if go to power save
  *@retval  TRUE if the device can enable power save, FALSE otherwise
  */
bool CERC_EnablePowerSave(bool enable)
{
  bool retVal;

  if (enable && (cercContext.state == CERC_IDLE)) {
    retVal =  cercContext.enablePowerSave;
    if (cercContext.enablePowerSave) {
      cercContext.enablePowerSave = FALSE;
      cercContext.startIdleTime = 0;    
    }
  } else {
    retVal = FALSE;
  }

  return retVal;
}

/**@brief  RF4CE callback to insert
  *@param  NLME discovery indication type
  *@retval None
  */
void NLME_DISCOVERY_indication(NLME_DISCOVERY_INDICATION_Type *param)
{
  return;
}


 /************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
