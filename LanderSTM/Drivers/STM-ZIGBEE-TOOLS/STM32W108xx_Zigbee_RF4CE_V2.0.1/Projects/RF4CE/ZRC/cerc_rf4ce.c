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
#include "timer.h"
#include "rf4ce.h"
#include "cerc_rf4ce.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include PLATFORM_HEADER
#include "hal.h"
#include "error.h"


/** @addtogroup ZRC_demos
  * @{
  */

/** @addtogroup CERC8RF4CE
  * @{
  */

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
  /*  Start time to wait for a command */
  uint32_t startTime;
  /* Pairing reference */
  uint8_t pairingRef;
  /* Command Code */
  uint8_t commandKeyCode[2];
  /* Active Time */
  uint32_t startIdleTime;
  /*  Flag to indicate if the power save can be enabled */
  bool enablePowerSave;
} CERC_Context_Type;

/* Private define ------------------------------------------------------------*/
/*  State for the CER state machine */
#define CERC_IDLE                      0x00
#define CERC_WAIT_DSC_PAIR_CONFIRM     0x01
#define CERC_SEND_PAIR_REQUEST         0x02
#define CERC_WAIT_DSC_PAIR_INDICATION  0x03
#define CERC_WAIT_TX_HANDLER           0x04
#define CERC_WAIT_NWK_START_CONFIRM    0x05
#define CERC_SEND_COMMAND_REPEATED     0x06

/* Max wait time to accept pair indication from remote node */
#define TARGET_WAIT_PAIR_INDICATION    0x1c9c38 // 30 sec implementation specific
#define CONTROLLER_WAIT_RESPONSE_TIME  0x098968 // 10 sec implementation specific

/* If the device is in IDLE state for 250 ms goes in deep sleep mode 2 */
#define MAX_IDLE_TIME 250000 

#define ASCII_XON         0x11  /* requests host to pause sending  */
#define ASCII_XOFF        0x13  /* requests host to resume sending */
/* Private macro -------------------------------------------------------------*/
/* Public variables --------------------------*-------------------------------*/
/* Private variables ---------------------------------------------------------*/
static CERC_Context_Type cercContext;
static uint8_t cmd_repeated = CERC_USER_CONTROL_REPEATED;
static bool radioDisabled;

/* Private function prototypes -----------------------------------------------*/
static void CERC_SendPair(void);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  host stop commands
  * @param  None
  * @retval None
  */
void hostStopCommands(void)
{
  __io_putchar(ASCII_XOFF);
}

/**
  * @brief  host start commands
  * @param  None
  * @retval None
  */
void hostStartCommands(void)
{
  __io_putchar(ASCII_XON);
}

/* Public functions ---------------------------------------------*------------*/

/**
  * @brief  CERC initialization
  * @param  *param pointer on CERC init structure 
  * @retval None
  */
uint32_t CERC_Init(CERC_Init_Type *param)
{
  uint32_t status, app;
  NLME_SET_Type val;

  if (radioDisabled && !cercContext.isTarget) {
    NWK_PowerUp(FALSE);
    radioDisabled = FALSE;
  }
  radioDisabled = FALSE;

  cercContext.isTarget = param->nodeCap & 0x01;

  if (param->coldStart) {
    uint16_t myVendorId = 0xFFF2;
    NWK_Config(nwkcVendorIdentifier_ID, &myVendorId);
    NWK_Config(nwkcVendorString_ID, "Vendor");
  }
  /* Start the network and the NIB with the 
     default value */  
  status = NWK_Init(param->nodeCap, param->coldStart);
  if ((status != SUCCESS) && (status != RF4CE_SAP_PENDING))
    return status;

  /* Set the CERC profile 
     initial NIB attribute */
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
    val.NIBAttributeValue = (uint8_t*)"STM32W Target";
  else
    val.NIBAttributeValue = (uint8_t*)"STM32W RC";
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

/**
  * @brief  CERC Destination pair
  * @param  *param pointer on CERC DscPair structure
  * @retval None
  */
uint32_t CERC_DscPair(CERC_DscPair_Type *param)
{
  uint32_t status = SUCCESS;

  if (radioDisabled && !cercContext.isTarget) {
    NWK_PowerUp(FALSE);
    radioDisabled = FALSE;
  }

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

/**
  * @brief  NLME discovery confirm
  * @param  *param pointer on NLME Discovery confirm structure
  * @retval None
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

/**
  * @brief  NLME auto discovery confirm
  * @param  *param pointer on NLME auto Discovery confirm structure
  * @retval None
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

/**
  * @brief CERC send pair
  * @param  None
  * @retval None
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

/**
  * @brief  NLME pair confirm
  * @param  *params pointer on NLME pair confirm structure
  * @retval None
  */
void NLME_PAIR_confirm(NLME_PAIR_CONFIRM_Type *params)
{
  CERC_DscPairCallback_Type cerc_param;

  cerc_param.rf4cePrimitive = CERC_PAIR_CONFIRM;
  cerc_param.DscPair.pairConfirm = params;
  CERC_DscPairCallback(&cerc_param);
  cercContext.state = CERC_IDLE;
}

/**
  * @brief  NLME pair indication
  * @param  *param pointer on NLME pair indication structure
  * @retval None
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

/**
  * @brief  CERC pair Response
  * @param  None
  * @retval Pair response status
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

/**
  * @brief  NLME communication status indication
  * @param  *params pointer on NLME comm sttsus indication structure
  * @retval None
  */
void NLME_COMM_STATUS_indication (NLME_COMM_STATUS_INDICATION_Type *params)
{
  CERC_DscPairCallback_Type cerc_param;

  cerc_param.rf4cePrimitive = CERC_RF4CE_COMM_STATUS_INDICATION;
  cerc_param.DscPair.commStatusIndication = params;
  CERC_DscPairCallback(&cerc_param);

  cercContext.state = CERC_IDLE;
}

/**
  * @brief  CERC Send communication
  * @param  *cerc_param CERC SendCommand structure
  * @retval None
  */
uint32_t CERC_SendCommand(CERC_SendCommand_Type *cerc_param)
{
  uint32_t status;
  NLDE_DATA_REQUEST_Type param;

  if (cercContext.state != CERC_IDLE)
    return CERC_DATA_DISCARDED;

  if ((cerc_param->commandCode[0] == CERC_USER_CONTROL_PRESSED) &&
      (memcmp(cercContext.commandKeyCode, &cerc_param->commandCode[1], (cerc_param->commandLength-1)) == 0) &&
      (TIME_ELAPSED(cercContext.startTime) <= (aplcMaxKeyRepeatInterval*16))) {
    if(TIME_ELAPSED(cercContext.startTime) < (aplKeyRepeatInterval_Default*16)) {
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
    param.TxOptions = RF4CE_DATA_REQ_TX_MODE; /* Broadcast - nwk address - no ack - no security */
                                             /* - multiple channel - no channel designator - non vendor specific */
  } else {
    param.TxOptions = 
      RF4CE_DATA_REQ_ACK_MODE | /* Unicast - nwk address - ack - security - */
      RF4CE_DATA_REQ_SEC_MODE ; /* multiple channel - no channel designator - non vendor specific */
  }
  
  if (radioDisabled && !cercContext.isTarget) {
    NWK_PowerUp(FALSE);
    radioDisabled = FALSE;
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
    cercContext.state = CERC_IDLE;
    status = CERC_DATA_DISCARDED;
  }

  return status;
}

/**
  * @brief  CERC Send Network frame
  * @param  *param pointer on NLDE Data request structure
  * @retval data request status
  */
uint32_t CERC_SendNwkFrame(NLDE_DATA_REQUEST_Type *param)
{
  uint32_t status;

  if (cercContext.state != CERC_IDLE)
    return CERC_DATA_DISCARDED;

  status = NLDE_DATA_request(param);
  if (status == RF4CE_SAP_PENDING) {
    cercContext.state = CERC_WAIT_TX_HANDLER;
    status = CERC_DATA_QUEUED;
  } else {
    cercContext.state = CERC_IDLE;
    status = CERC_DATA_DISCARDED;
  }

  return status;
}

/**
  * @brief  NLDE data confirm
  * @param  *param pointer on NLDE Data confirm structure
  * @retval None
  */
void NLDE_DATA_confirm(NLDE_DATA_CONFIRM_Type *param)
{
  CERC_CommandCallback_Type cerc_param;
  
  cercContext.state = CERC_IDLE;
  cerc_param.dataCmd.dataConfirm = param;
  cerc_param.rf4cePrimitive = CERC_DATA_CONFIRM;
  CERC_CommandCallback(&cerc_param);
}

/**
  * @brief  NLME start confirm
  * @param  *status start confirm status
  * @retval None
  */
void NLME_START_confirm (uint32_t *status)
{
  CERC_InitCallback(status);
  cercContext.state = CERC_IDLE;
}

/**
  * @brief  NLME data indication
  * @param  *param pointer on NLDE data indication structure
  * @retval None
  */
void NLDE_DATA_indication(NLDE_DATA_INDICATION_Type *param)
{
  CERC_CommandCallback_Type cerc_param;

  cerc_param.rf4cePrimitive = CERC_DATA_INDICATION;
  cerc_param.dataCmd.dataIndication = param;
  CERC_CommandCallback(&cerc_param);
}

/**
  * @brief  NLME unpair indication
  * @param  *PairingRef pairing reference
  * @retval None
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

/**
  * @brief  NLME unpair confirm
  * @param  *param pointer on NLME unpair confirm structure
  * @retval None
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

/**
  * @brief  CERC Tick
  * @param  None
  * @retval None
  */
void CERC_Tick(void)
{
  switch (cercContext.state) {
  case CERC_IDLE:
    if (!radioDisabled && !cercContext.isTarget) {
#if !defined(ST_BTL) && defined(STM32W108CB)
      NWK_PowerDown();
#endif
      radioDisabled = TRUE;
    }
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
    if ((cercContext.startTime != 0) && (TIME_ELAPSED(cercContext.startTime) > (TARGET_WAIT_PAIR_INDICATION*16))) {
      CERC_DscPairCallback_Type cerc_param;
      NLME_AUTO_DISCOVERY_CONFIRM_Type param;

      cerc_param.rf4cePrimitive = CERC_AUTO_DSC_CONFIRM;
      param.Status = CERC_NO_PAIR_INDICATION;
      cerc_param.DscPair.autoDscConfirm = &param;
      CERC_DscPairCallback(&cerc_param);
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
  {
    uint32_t retVal;
    retVal = NWK_Tick();
    /* Check whether cache flush is required */
    if (!(retVal & RF4CE_STATE_BUSY) && 
	(retVal & RF4CE_STATE_NEED_CACHE_FLUSH)) {
      hostStopCommands();
      ATOMIC(NWK_Flush();)
      hostStartCommands();
    }

    if ((TIME_ELAPSED(cercContext.startIdleTime) >= MAX_IDLE_TIME)  &&
	!cercContext.isTarget && !(retVal & RF4CE_STATE_BUSY) &&
	!(retVal & RF4CE_STATE_NEED_CACHE_FLUSH) &&
	(cercContext.state == CERC_IDLE))
      cercContext.enablePowerSave = TRUE; 
  }
}

/**
  * @brief  CERC Enable Power Save
  * @param  enable: power save status
  * @retval CERC enable status
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
/**
  * @brief  NLME Discovery indication
  * @param  *param pointer on NLME discovery indication stucture
  * @retval None
  */
void NLME_DISCOVERY_indication(NLME_DISCOVERY_INDICATION_Type *param)
{
  /* RF4CE callback to insert */
  return;
}

/**
  * @}
  */

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
