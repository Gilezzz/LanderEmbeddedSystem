/**
  ******************************************************************************
  * @file    rf4ce_cmds.c
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   This file provides the host commands for nwk
  *          coprocessor implemenation
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
#include "rf4ce_nwkcop.h"
#include "stm32_sc.h"
#include "rf4ce.h"
#include <string.h>
#include <stdio.h>

/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static uint32_t nwkCop_NLDE_DATA_indication(uint8_t *packet);
static uint32_t nwkCop_NLDE_DATA_confirm(uint8_t *packet);
static uint32_t nwkCop_NLME_AUTO_DISCOVERY_confirm(uint8_t *packet);
static uint32_t nwkCop_NLME_COMM_STATUS_indication(uint8_t *packet);
static uint32_t nwkCop_NLME_DISCOVERY_indication(uint8_t *packet);
static uint32_t nwkCop_NLME_DISCOVERY_confirm(uint8_t *packet);
static uint32_t nwkCop_NLME_PAIR_indication(uint8_t *packet);
static uint32_t nwkCop_NLME_PAIR_confirm(uint8_t *packet);
static uint32_t nwkCop_NLME_START_confirm(uint8_t *packet);
static uint32_t nwkCop_NLME_UNPAIR_indication(uint8_t *packet);
static uint32_t nwkCop_NLME_UNPAIR_confirm(uint8_t *packet);

/* Private macro -------------------------------------------------------------*/
/* Public variables ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**@brief  network cop  nop
  *@param  None
  *@retval status
  */
uint32_t nwkCop_NOP(void)
{
  uint8_t cmd[]={0xc0, 0x05, 0x0, 0x01, 0x02, 0x03, 0x04, 0xfc};
  uint32_t status;
  uint8_t rsp[7];
  
  //printf("NOP Action started\r\n");
  status = sendHostCmd(cmd);
  if (status == SUCCESS) {
    status = waitRspCmd(rsp);
  }
  return status;
}

/**@brief  NLDE data request
  *@param  NLEDE data request type
  *@retval status
  */
uint32_t NLDE_DATA_request(NLDE_DATA_REQUEST_Type *param)
{
  uint32_t status;
  uint8_t rsp[7];
  uint8_t cmd[200];

  cmd[0] = NWKCOP_COMMAND_PACKET;
  cmd[1] = 7 + param->nsduLength;
  cmd[2] = NWKCOP_NLDE_DATA_REQUEST;
  cmd[3] = param->PairingRef;
  cmd[4] = param->ProfileId;
  memcpy(&cmd[5], &param->VendorId, 2);
  cmd[7] = param->nsduLength;
  memcpy(&cmd[8], param->nsdu, param->nsduLength);
  cmd[8+param->nsduLength] = param->TxOptions;
  cmd[9+param->nsduLength] = NWKCOP_END_PACKET;

  status = sendHostCmd(cmd);
  if (status == SUCCESS)
    status = waitRspCmd(rsp);
  if (status == SUCCESS)
    memcpy(&status, &rsp[2], 4);

  return status;
}

/**@brief  NLDE data indication
  *@param  packet
  *@retval success
  */
static uint32_t nwkCop_NLDE_DATA_indication(uint8_t *packet)
{
  uint8_t paramLen, start;
  uint8_t *cmdParam;
  NLDE_DATA_INDICATION_Type param;
  uint8_t nsdu[128];

  paramLen = packet[1] - 1;
  cmdParam = &packet[3];
  if (paramLen < 6)
    return NWKCOP_WRONG_PARAM_LENGTH;

  param.PairingRef = cmdParam[0];
  param.ProfileId = cmdParam[1];
  start = 2;
  if (cmdParam[paramLen-1] & (1 << 2)) {
    memcpy(&param.VendorId, &cmdParam[2], 2);
    start = 4;
  }
  param.nsduLength = cmdParam[start];
  memcpy(nsdu, &cmdParam[start+1], param.nsduLength);
  param.nsdu = nsdu;
  param.RxLinkQuality = cmdParam[start+1+param.nsduLength];
  param.RxFlags = cmdParam[start+2+param.nsduLength];

  NLDE_DATA_indication(&param);
  
  return SUCCESS;
}

/**@brief  network cop NLDE data confirmation
  *@param  packet
  *@retval Success
  */
static uint32_t nwkCop_NLDE_DATA_confirm(uint8_t *packet)
{
  uint8_t paramLen;
  uint8_t *cmdParam;
  NLDE_DATA_CONFIRM_Type param;

  paramLen = packet[1] - 1;
  cmdParam = &packet[3];
  if (paramLen < 5)
    return NWKCOP_WRONG_PARAM_LENGTH;

  memcpy(&param.Status, cmdParam, 4);
  param.PairingRef = cmdParam[4];

  NLDE_DATA_confirm(&param);

  return SUCCESS;
}

/**@brief  NLME auto discovery request
  *@param  NLME_AUTO_DISCOVERY_REQUEST_Type
  *@retval status
  */
uint32_t NLME_AUTO_DISCOVERY_request(NLME_AUTO_DISCOVERY_REQUEST_Type *param)
{
  uint32_t status;
  uint8_t rsp[7];
  uint8_t cmd[255];
  uint8_t numDev, numProf;

  cmd[0] = NWKCOP_COMMAND_PACKET;
  cmd[2] = NWKCOP_NLME_AUTO_DISCOVERY_REQUEST;
  cmd[3] = param->RecAppCapabilities;
  numDev = (param->RecAppCapabilities>>1) & 0x01;
  numProf = (param->RecAppCapabilities>>4) & 0x07;
  memcpy(&cmd[4], param->RecDevTypeList, numDev);
  memcpy(&cmd[4+numDev], param->RecProfileIdList, numProf);
  memcpy(&cmd[4+numDev+numProf], &param->AutoDiscDuration, 4);
  cmd[1] = 0x6 + numDev + numProf ;
  cmd[8+numDev+numProf] = NWKCOP_END_PACKET;

  status = sendHostCmd(cmd);
  if (status == SUCCESS)
    status = waitRspCmd(rsp);
  if (status == SUCCESS)
    memcpy(&status, &rsp[2], 4);

  return status;
}

/**@brief  NLME network cop auto discovery confirmation
  *@param  packet
  *@retval success
  */
static uint32_t nwkCop_NLME_AUTO_DISCOVERY_confirm(uint8_t *packet)
{
  uint8_t paramLen;
  uint8_t *cmdParam;
  NLME_AUTO_DISCOVERY_CONFIRM_Type param;

  paramLen = packet[1] - 1;
  cmdParam = &packet[3];
  if (paramLen < 12)
    return NWKCOP_WRONG_PARAM_LENGTH;

  memcpy(&param.Status, cmdParam, 4);
  memcpy(param.Addr, &cmdParam[4], 8);

  NLME_AUTO_DISCOVERY_confirm(&param);

  return SUCCESS;
}

/**@brief  NLME network cop comm status indication
  *@param  packet
  *@retval success
  */
static uint32_t nwkCop_NLME_COMM_STATUS_indication(uint8_t *packet)
{
  uint8_t paramLen, addrLen;
  uint8_t *cmdParam;
  NLME_COMM_STATUS_INDICATION_Type param;

  paramLen = packet[1] - 1;
  cmdParam = &packet[3];

  if ((paramLen !=  10) && (paramLen != 16))
    return NWKCOP_WRONG_PARAM_LENGTH;

  param.PairingRef = cmdParam[0];
  memcpy(&param.DstPANId, &cmdParam[1], 2);
  param.DstAddrMode = cmdParam[3];
  if (param.DstAddrMode == 1) {
    memcpy(param.DstAddr.IEEEAddress, &cmdParam[4], 8);
    addrLen = 8;
  } else {
    memcpy(&param.DstAddr.ShortAddress, &cmdParam[4], 2);
    addrLen = 2;
  }
  memcpy(&param.Status, &cmdParam[4+addrLen], 4);

  NLME_COMM_STATUS_indication (&param);

  return SUCCESS;
}

/**@brief  NLME discovery request
  *@param  NLME_DISCOVERY_REQUEST_Type
  *@retval status
  */
uint32_t NLME_DISCOVERY_request(NLME_DISCOVERY_REQUEST_Type *param)
{
  uint32_t status;
  uint8_t rsp[7];
  uint8_t cmd[255];
  uint8_t numDev, numProf;

  cmd[0] = NWKCOP_COMMAND_PACKET;
  cmd[2] = NWKCOP_NLME_DISCOVERY_REQUEST;
  memcpy(&cmd[3], &param->DstPANId, 2);
  memcpy(&cmd[5], &param->DstNwkAddr, 2);
  cmd[7] = param->OrgAppCapabilities;
  numDev = (param->OrgAppCapabilities>>4) & 0x01;
  numProf = (param->OrgAppCapabilities>>4) & 0x07;
  memcpy(&cmd[8], param->OrgDevTypeList, numDev);
  memcpy(&cmd[8+numDev], param->OrgProfileIdList, numProf);
  cmd[8+numDev+numProf] = param->SearchDevType;
  cmd[9+numDev+numProf] = param->DiscProfileIdListSize;
  memcpy(&cmd[10+numDev+numProf], param->DiscProfileIdList, param->DiscProfileIdListSize);
  memcpy(&cmd[10+numDev+numProf+param->DiscProfileIdListSize], &param->DiscDuration, 4);
  cmd[1] = 12 + numDev + numProf + param->DiscProfileIdListSize;
  cmd[14+numDev+numProf+param->DiscProfileIdListSize] = NWKCOP_END_PACKET;

  status = sendHostCmd(cmd);
  if (status == SUCCESS)
    status = waitRspCmd(rsp);
  if (status == SUCCESS)
    memcpy(&status, &rsp[2], 4);

  return status;
}

/**@brief  NLME network cop discovery indication
  *@param  packet
  *@retval success
  */
static uint32_t nwkCop_NLME_DISCOVERY_indication(uint8_t *packet)
{
  uint8_t paramLen, numDev, numProf, start;
  uint8_t *cmdParam;
  NLME_DISCOVERY_INDICATION_Type param;

  paramLen = packet[1] - 1;
  cmdParam = &packet[3];
  if (paramLen < 19)
    return NWKCOP_WRONG_PARAM_LENGTH;

  memset(param.OrgUserString, 0, sizeof(param.OrgUserString));
  memset(param.OrgVendorString, 0, sizeof(param.OrgVendorString));
  memset(param.OrgDevTypeList, 0, sizeof(param.OrgDevTypeList));
  memset(param.OrgProfileIdList, 0, sizeof(param.OrgProfileIdList));

  param.Status = cmdParam[0];
  memcpy(param.SrcIEEEAddr, &cmdParam[1], 8);
  param.OrgNodeCapabilities = cmdParam[9];
  memcpy(&param.OrgVendorId, &cmdParam[10], 2);
  memcpy(param.OrgVendorString, &cmdParam[12], 8);
  param.OrgAppCapabilities = cmdParam[20];
  numDev = (param.OrgAppCapabilities>>1) & 0x01;
  numProf = (param.OrgAppCapabilities>>4) & 0x07;
  if (param.OrgAppCapabilities&0x01) {
    memcpy(param.OrgUserString, &cmdParam[21], 16);
    start = 37;
  } else {
    start = 21;
  }
  memcpy(param.OrgDevTypeList, &cmdParam[start], numDev);
  memcpy(param.OrgProfileIdList, &cmdParam[start+numDev], numProf);
  param.SearchDevType = cmdParam[start+numDev+numProf];
  param.RxLinkQuality = cmdParam[start+numDev+numProf+1];
  
  NLME_DISCOVERY_indication(&param);

  return SUCCESS;
}

/**@brief  NLME discovery response
  *@param  NLME_DISCOVERY_RESPONSE_Type
  *@retval status
  */
uint32_t NLME_DISCOVERY_response(NLME_DISCOVERY_RESPONSE_Type *param)
{
  uint32_t status;
  uint8_t rsp[7];
  uint8_t cmd[255];
  uint8_t numDev, numProf;

  cmd[0] = NWKCOP_COMMAND_PACKET;
  cmd[2] = NWKCOP_NLME_DISCOVERY_RESPONSE;
  cmd[3] = param->Status;
  memcpy(&cmd[4], param->DstIEEEAddr, 8);
  cmd[12] = param->RecAppCapabilities;
  numDev = (param->RecAppCapabilities>>1) & 0x01;
  numProf = (param->RecAppCapabilities>>4) & 0x07;
  memcpy(&cmd[13], param->RecDevTypeList, numDev);
  memcpy(&cmd[13+numDev], param->RecProfileIdList, numProf);
  cmd[13+numDev+numProf] = param->DiscReqLQI;
  cmd[1] = 12 + numDev + numProf;
  cmd[14+numDev+numProf] = NWKCOP_END_PACKET;

  status = sendHostCmd(cmd);
  if (status == SUCCESS)
    status = waitRspCmd(rsp);
  if (status == SUCCESS)
    memcpy(&status, &rsp[2], 4);

  return status;
}

/**@brief  NLME network cop discovery confirmation.
  *@param  Packet
  *@retval success
  */
static uint32_t nwkCop_NLME_DISCOVERY_confirm(uint8_t *packet)
{
  uint8_t paramLen;
  uint8_t *cmdParam;
  NLME_DISCOVERY_CONFIRM_Type param;

  paramLen = packet[1] - 1;
  cmdParam = &packet[3];
  if (paramLen < 5)
    return NWKCOP_WRONG_PARAM_LENGTH;

  memcpy(&param.Status, cmdParam, 4);
  param.NumNodes = cmdParam[4];
  memset(param.NodeDescList, 0, sizeof(NodeDescType)*param.NumNodes);
  memcpy(param.NodeDescList, &cmdParam[5], sizeof(NodeDescType)*param.NumNodes);

  NLME_DISCOVERY_confirm (&param);
  
  return SUCCESS;
}

/**@brief  get NLME
  *@param  NLME_GET_Type
  *@retval status
  */
uint32_t NLME_Get(NLME_GET_Type *param)
{
  uint32_t status;
  uint8_t cmd[6];
  uint8_t rsp[255];

  cmd[0] = NWKCOP_COMMAND_PACKET;
  cmd[1] = 0x03;
  cmd[2] = NWKCOP_NLME_GET;
  cmd[3] = param->NIBAttribute;
  cmd[4] = param->NIBAttributeIndex;
  cmd[5] = NWKCOP_END_PACKET;

  status = sendHostCmd(cmd);
  if (status == SUCCESS)
    status = waitRspCmd(rsp);
  if (status == SUCCESS)
    memcpy(&status, &rsp[2], 4);
  if (status == SUCCESS) {
    status = receiveHostCmd(rsp);
    if ((status == SUCCESS) && (rsp[2] == NWKCOP_NLME_GET)) {
      switch(param->NIBAttribute) {
      case 0x68:
	memcpy(param->NIBAttributeValue, &rsp[3], sizeof(PairingTableEntryType));
	break;
      case 0x6f:
	memcpy(param->NIBAttributeValue, &rsp[3], 16);
	break;
      default:
	memcpy(param->NIBAttributeValue, &rsp[3], 4);
      }
    }
  }

  return status;
}

/**@brief  NLME pairing request
  *@param  NLME_PAIR_REQUEST_Type
  *@retval status
  */
uint32_t NLME_PAIR_request (NLME_PAIR_REQUEST_Type *param)
{
  uint32_t status;
  uint8_t rsp[7];
  uint8_t cmd[255];
  uint8_t numDev, numProf;

  cmd[0] = NWKCOP_COMMAND_PACKET;
  cmd[2] = NWKCOP_NLME_PAIR_REQUEST;
  cmd[3] = param->LogicalChannel;
  memcpy(&cmd[4], &param->DstPANId, 2);
  memcpy(&cmd[6], param->DstIEEEAddr, 8);
  cmd[14] = param->OrgAppCapabilities;
  numDev = (param->OrgAppCapabilities>>1) & 0x01;
  numProf = (param->OrgAppCapabilities>>4) & 0x07;
  memcpy(&cmd[15], param->OrgDevTypeList, numDev);
  memcpy(&cmd[15+numDev], param->OrgProfileIdList, numProf);
  cmd[15+numDev+numProf] = param->KeyExTransferCount;
  cmd[1] = 14 + numDev + numProf;
  cmd[16+numDev+numProf] = NWKCOP_END_PACKET;

  status = sendHostCmd(cmd);
  if (status == SUCCESS)
    status = waitRspCmd(rsp);
  if (status == SUCCESS)
    memcpy(&status, &rsp[2], 4);
  
  return status;  
}

/**@brief  NLME network cop pairing indication
  *@param  packet
  *@retval success
  */
static uint32_t nwkCop_NLME_PAIR_indication(uint8_t *packet)
{
  uint8_t paramLen, start, numDev, numProf;
  uint8_t *cmdParam;
  NLME_PAIR_INDICATION_Type params;

  paramLen = packet[1] - 1;
  cmdParam = &packet[3];
  if (paramLen < 28)
    return NWKCOP_WRONG_PARAM_LENGTH;

  memset(&params, 0, sizeof(NLME_PAIR_INDICATION_Type));

  memcpy(&params.Status, &cmdParam[0], 4);
  memcpy(&params.SrcPANId, &cmdParam[4], 2);
  memcpy(params.SrcIEEEAddr, &cmdParam[6], 8);
  params.OrgNodeCapabilities = cmdParam[14];
  memcpy(&params.OrgVendorId, &cmdParam[15], 2);
  memcpy(params.OrgVendorString, &cmdParam [17], 8);
  params.OrgAppCapabilities = cmdParam[25];
  start = 26;
  if(params.OrgAppCapabilities&0x1) {
    memcpy(params.OrgUserString, &cmdParam[26], 16);
    start = 42;
  } 
  numDev = (params.OrgAppCapabilities>>1) & 0x01;
  numProf = (params.OrgAppCapabilities>>4) & 0x07;
  memcpy(params.OrgDevTypeList, &cmdParam[start], numDev);
  memcpy(params.OrgProfileIdList, &cmdParam[start+numDev], numProf);
  params.ProvPairingRef = cmdParam[start+numDev+numProf];

  NLME_PAIR_indication (&params);

  return SUCCESS;
}

/**@brief  NLME pairing response
  *@param  NLME_PAIR_RESPONSE_Type
  *@retval status
  */
uint32_t NLME_PAIR_response(NLME_PAIR_RESPONSE_Type *param)
{
  uint32_t status;
  uint8_t rsp[7];
  uint8_t cmd[255];
  uint8_t numDev, numProf;

  cmd[0] = NWKCOP_COMMAND_PACKET;
  cmd[2] = NWKCOP_NLME_PAIR_RESPONSE;
  memcpy(&cmd[3], &param->Status, 4);
  memcpy(&cmd[7], &param->DstPANId, 2);
  memcpy(&cmd[9], param->DstIEEEAddr, 8);
  cmd[17] = param->RecAppCapabilities;
  numDev = (param->RecAppCapabilities>>1) & 0x01;
  numProf = (param->RecAppCapabilities>>4) & 0x07;
  memcpy(&cmd[18], param->RecDevTypeList, numDev);
  memcpy(&cmd[18+numDev], param->RecProfileIdList, numProf);
  cmd[18+numDev+numProf] = param->ProvPairingRef;
  cmd[1] = 17 + numDev + numProf;
  cmd[19+numDev+numProf] = NWKCOP_END_PACKET;

  status = sendHostCmd(cmd);
  if (status == SUCCESS)
    status = waitRspCmd(rsp);
  if (status == SUCCESS)
    memcpy(&status, &rsp[2], 4);

  return status;  
}

/**@brief  NLME network cop pairing confirmation
  *@param  packet
  *@retval success
  */
static uint32_t nwkCop_NLME_PAIR_confirm(uint8_t *packet)
{
  uint8_t paramLen, numDev, numProf, start;
  uint8_t *cmdParam;
  NLME_PAIR_CONFIRM_Type params;

  paramLen = packet[1] - 1;
  cmdParam = &packet[3];  
  if (paramLen < 15)
    return NWKCOP_WRONG_PARAM_LENGTH;

  memset(&params, 0, sizeof(NLME_PAIR_CONFIRM_Type));
  
  memcpy(&params.Status, &cmdParam[0], 4);
  params.PairingRef = cmdParam[4];
  memcpy(&params.RecVendorId, &cmdParam[5], 2);
  memcpy(params.RecVendorString, &cmdParam[7], 8);
  params.RecAppCapabilities = cmdParam[15];
  numDev = (params.RecAppCapabilities>>1) & 0x01;
  numProf = (params.RecAppCapabilities>>4) & 0x07;
  if (params.RecAppCapabilities&0x01) {
    memcpy(params.RecUserString, &cmdParam[16], 16);
    start = 32;
  } else {
    start = 16;
  }
  memcpy(params.RecDevTypeList, &cmdParam[start], numDev);
  memcpy(params.RecProfileIdList, &cmdParam[start+numDev], numProf);

  NLME_PAIR_confirm(&params);

  return SUCCESS;
}

/**@brief  NLME reset request
  *@param  setDEfaultPIB
  *@retval status
  */
uint32_t NLME_RESET_request(bool SetDefaultPIB)
{
  uint32_t status;
  uint8_t rsp[7];
  uint8_t cmd[5];

  cmd[0] = NWKCOP_COMMAND_PACKET;
  cmd[1] = 2;
  cmd[2] = NWKCOP_NLME_RESET;
  cmd[3] = SetDefaultPIB;
  cmd[4] = NWKCOP_END_PACKET;

  status = sendHostCmd(cmd);
  if (status == SUCCESS)
    status = waitRspCmd(rsp);
  if (status == SUCCESS)
    memcpy(&status, &rsp[2], 4);

  return status;    
}

/**@brief  NLME RX enabling request
  *@param  RxOnDuration
  *@retval status
  */
uint32_t NLME_RX_ENABLE_request (uint32_t RxOnDuration)
{
  uint32_t status;
  uint8_t rsp[7];
  uint8_t cmd[8];

  cmd[0] = NWKCOP_COMMAND_PACKET;
  cmd[1] = 5;
  cmd[2] = NWKCOP_NLME_RX_ENABLE;
  memcpy(&cmd[3], &RxOnDuration, 4);
  cmd[7] = NWKCOP_END_PACKET;

  status = sendHostCmd(cmd);
  if (status == SUCCESS)
    status = waitRspCmd(rsp);
  if (status == SUCCESS)
    memcpy(&status, &rsp[2], 4);

  return status;    
}

/**@brief  set NLME
  *@param  NLME set type
  *@retval status
  */
uint32_t NLME_Set(NLME_SET_Type *param)
{
  uint32_t status;
  uint8_t cmd[50];
  uint8_t rsp[8];

  cmd[0] = NWKCOP_COMMAND_PACKET;
  cmd[2] = NWKCOP_NLME_SET;
  cmd[3] = param->NIBAttribute;
  cmd[4] = param->NIBAttributeIndex;
  switch(param->NIBAttribute) {
  case 0x68:
    memcpy(&cmd[5], param->NIBAttributeValue, sizeof(PairingTableEntryType));
    cmd[5+sizeof(PairingTableEntryType)] = NWKCOP_END_PACKET;
    cmd[1] = 3 + sizeof(PairingTableEntryType);
    break;
  case 0x6f:
    memcpy(&cmd[5], param->NIBAttributeValue, 16);
    cmd[21] = NWKCOP_END_PACKET;
    cmd[1] = 19;
    break;
  default:
    memcpy(&cmd[5], param->NIBAttributeValue, 4);
    cmd[9] = NWKCOP_END_PACKET;
    cmd[1] = 7;
  }

  status = sendHostCmd(cmd);
  if (status == SUCCESS) 
    status = waitRspCmd(rsp);
  if (status == SUCCESS)
    memcpy(&status, &rsp[2], 4);

  return status;
}

/**@brief  start NLME request
  *@param  None
  *@retval status
  */
uint32_t NLME_START_request(void)
{
  uint32_t status;
  uint8_t cmd[9];
  uint8_t rsp[8];

  cmd[0] = NWKCOP_COMMAND_PACKET;
  cmd[1] = 0x01;
  cmd[2] = NWKCOP_NLME_START_REQUEST;
  cmd[5] = NWKCOP_END_PACKET;

  status = sendHostCmd(cmd);
  if (status == SUCCESS) 
    status = waitRspCmd(rsp);
  if (status == SUCCESS)
    memcpy(&status, &rsp[2], 4);

  return status;  
}

/**@brief  start NLME network cop confirmation
  *@param  packet
  *@retval success
  */
static uint32_t nwkCop_NLME_START_confirm(uint8_t *packet)
{
  uint8_t paramLen;
  uint8_t *cmdParam;
  uint32_t status;

  paramLen = packet[1] - 1;
  cmdParam = &packet[3];
  if (paramLen != 4)
    return NWKCOP_WRONG_PARAM_LENGTH;

  memcpy(&status, &cmdParam[0], 4);

  NLME_START_confirm(&status);

  return SUCCESS;
}

/**@brief  Unpair NLME request
  *@param  pairing reference
  *@retval status
  */
uint32_t NLME_UNPAIR_request (uint8_t PairingRef)
{
  uint32_t status;
  uint8_t rsp[7];
  uint8_t cmd[5];

  cmd[0] = NWKCOP_COMMAND_PACKET;
  cmd[1] = 2;
  cmd[2] = NWKCOP_NLME_UNPAIR_REQUEST;
  cmd[3] = PairingRef;
  cmd[4] = NWKCOP_END_PACKET;

  status = sendHostCmd(cmd);
  if (status == SUCCESS)
    status = waitRspCmd(rsp);
  if (status == SUCCESS)
    memcpy(&status, &rsp[2], 4);

  return status;    
}


/**@brief  unpair network NLME indication
  *@param  packet
  *@retval success
  */
static uint32_t nwkCop_NLME_UNPAIR_indication(uint8_t *packet)
{
  uint8_t paramLen, PairingRef;
  uint8_t *cmdParam;

  paramLen = packet[1] - 1;
  cmdParam = &packet[3];
  if (paramLen != 1)
    return NWKCOP_WRONG_PARAM_LENGTH;

  PairingRef = cmdParam[0];

  NLME_UNPAIR_indication(&PairingRef);

  return SUCCESS;
}

/**@brief  unpair NLME response
  *@param  pairing reference
  *@retval status
  */
uint32_t NLME_UNPAIR_response(uint8_t PairingRef)
{
  uint32_t status;
  uint8_t rsp[7];
  uint8_t cmd[5];

  cmd[0] = NWKCOP_COMMAND_PACKET;
  cmd[1] = 2;
  cmd[2] = NWKCOP_NLME_UNPAIR_RESPONSE;
  cmd[3] = PairingRef;
  cmd[4] = NWKCOP_END_PACKET;

  status = sendHostCmd(cmd);
  if (status == SUCCESS)
    status = waitRspCmd(rsp);
  if (status == SUCCESS)
    memcpy(&status, &rsp[2], 4);

  return status;    
}

/**@brief  unpair network cop NLME confirmation
  *@param  packet
  *@retval success
  */
static uint32_t nwkCop_NLME_UNPAIR_confirm(uint8_t *packet)
{
  uint8_t paramLen;
  uint8_t *cmdParam;
  NLME_UNPAIR_CONFIRM_Type param;

  paramLen = packet[1] - 1;
  cmdParam = &packet[3];
  if (paramLen != 5)
    return NWKCOP_WRONG_PARAM_LENGTH;

  memcpy(&param.Status, &cmdParam[0], 4);
  param.PairingRef = cmdParam[4];

  NLME_UNPAIR_confirm(&param);

  return SUCCESS;
}

/**@brief  Update NLME key request
  *@param  pairing reference
  *@param  linkKeyType 
  *@retval status
  */
uint32_t NLME_UPDATE_KEY_request(uint8_t PairingRef, linkKeyType NewLinkKey)
{
  uint32_t status;
  uint8_t rsp[7];
  uint8_t cmd[21];

  cmd[0] = NWKCOP_COMMAND_PACKET;
  cmd[1] = 18;
  cmd[2] = NWKCOP_NLME_UPDATE_KEY;
  cmd[3] = PairingRef;
  memcpy(&cmd[4], NewLinkKey, 16);
  cmd[20] = NWKCOP_END_PACKET;

  status = sendHostCmd(cmd);
  if (status == SUCCESS)
    status = waitRspCmd(rsp);
  if (status == SUCCESS)
    memcpy(&status, &rsp[2], 4);

  return status;    
}

/**@brief  Initialize NWK
  *@param  node Cap
  *@param  force cold start
  *@retval status
  */
uint32_t NWK_Init(uint8_t nodeCap, bool forceColdStart)
{
  uint32_t status;
  uint8_t cmd[6];
  uint8_t rsp[7];

  cmd[0] = NWKCOP_COMMAND_PACKET;
  cmd[1] = 0x03;
  cmd[2] = NWKCOP_NWK_INIT;
  cmd[3] = nodeCap;
  cmd[4] = forceColdStart;
  cmd[5] = NWKCOP_END_PACKET;

  status = sendHostCmd(cmd);
  if (status == SUCCESS) 
    status = waitRspCmd(rsp);
  if (status == SUCCESS)
    memcpy(&status, &rsp[2], 4);

  return status;
}

/**@brief  response callback
  *@param  packet
  *@retval status
  */
void RspCallback(uint8_t *packet)
{
  uint32_t status=0;
  
  switch(packet[2]) {
  case NWKCOP_NLDE_DATA_INDICATION:
    status = nwkCop_NLDE_DATA_indication(packet);
    break;
  case NWKCOP_NLDE_DATA_CONFIRM:
    status = nwkCop_NLDE_DATA_confirm(packet);
    break;
  case NWKCOP_NLME_AUTO_DISCOVERY_CONFIRM:
    status = nwkCop_NLME_AUTO_DISCOVERY_confirm(packet);
    break;
  case NWKCOP_NLME_COMM_STATUS_INDICATION:
    status = nwkCop_NLME_COMM_STATUS_indication(packet);
    break;
  case NWKCOP_NLME_DISCOVERY_INDICATION:
    status = nwkCop_NLME_DISCOVERY_indication(packet);
    break;
  case NWKCOP_NLME_DISCOVERY_CONFIRM:
    status = nwkCop_NLME_DISCOVERY_confirm(packet);
    break;
  case NWKCOP_NLME_PAIR_INDICATION:
    status = nwkCop_NLME_PAIR_indication(packet);
    break;
  case NWKCOP_NLME_PAIR_CONFIRM:
    status = nwkCop_NLME_PAIR_confirm(packet);
    break;
  case NWKCOP_NLME_START_CONFIRM:
    status = nwkCop_NLME_START_confirm(packet);
    break;
  case NWKCOP_NLME_UNPAIR_INDICATION:
    status = nwkCop_NLME_UNPAIR_indication(packet);
    break;
  case NWKCOP_NLME_UNPAIR_CONFIRM:
    status = nwkCop_NLME_UNPAIR_confirm(packet);
    break;
  default:
    printf("Code received %d !!!!!!!\r\n", packet[2]);
  }
  
  if (status != SUCCESS)
    printf("Error 0x%08x in function %02x \r\n", status, packet[2]);
}

/**@brief  Get the MAC Eui64
  *@param  eui64
  *@retval status
  */
uint32_t MAC_GetEui64(uint8_t *eui64)
{
  uint32_t status;
  uint8_t cmd[4];
  uint8_t rsp[12];

  cmd[0] = NWKCOP_COMMAND_PACKET;
  cmd[1] = 0x01;
  cmd[2] = NWKCOP_MAC_GET_EUI64;
  cmd[3] = NWKCOP_END_PACKET;

  status = sendHostCmd(cmd);
  if (status == SUCCESS)
    status = waitRspCmd(rsp);
  if (status == SUCCESS)
    memcpy(&status, &rsp[2], 4);
  if (status == SUCCESS) {
    status = receiveHostCmd(rsp);
    if (status == SUCCESS) {
      memcpy(eui64, &rsp[3], 8);
    }
  }
  return status;  
}

/**@brief  set MAC Eui64 identifier
  *@param  eui64
  *@retval status
  */
uint32_t MAC_SetEui64(uint8_t *eui64)
{
  uint32_t status;
  uint8_t cmd[12];
  uint8_t rsp[7];

  cmd[0] = NWKCOP_COMMAND_PACKET;
  cmd[1] = 0x09;
  cmd[2] = NWKCOP_MAC_SET_EUI64;
  memcpy(&cmd[3], eui64, 8);
  cmd[11] = NWKCOP_END_PACKET;

  status = sendHostCmd(cmd);
  if (status == SUCCESS)
    status = waitRspCmd(rsp);
  if (status == SUCCESS)
    memcpy(&status, &rsp[2], 4);

  return status;  
}

/**@brief  GEt network cop formware version
  *@param  nwkCop_ver
  *@param  rf4ce_ver
  *@retval status
  */
uint32_t nwkCop_FirmwareVersion(uint8_t *nwkCop_ver, uint8_t *rf4ce_ver)
{
  uint32_t status;
  uint8_t cmd[4];
  uint8_t rsp[10];

  cmd[0] = NWKCOP_COMMAND_PACKET;
  cmd[1] = 0x01;
  cmd[2] = NWKCOP_FIRMWARE_VERSION;
  cmd[3] = NWKCOP_END_PACKET;

  status = sendHostCmd(cmd);
  if (status == SUCCESS)
    status = waitRspCmd(rsp);
  if (status == SUCCESS) 
    memcpy(&status, &rsp[2], 4);
  if (status == SUCCESS)
    status = receiveHostCmd(rsp);
  memcpy(nwkCop_ver, &rsp[3], 3);
  memcpy(rf4ce_ver, &rsp[6], 3);

  return status;  
}

/**@brief  Ebnable power saving
  *@param  power save type
  *@retval status
  */
uint32_t nwkCop_enablePowerSave(uint8_t powerSaveType)
{
  uint32_t status;
  uint8_t cmd[5];
  uint8_t rsp[7];
  
  cmd[0] = NWKCOP_COMMAND_PACKET;
  cmd[1] = 0x02;
  cmd[2] = NWKCOP_POWER_DOWN;
  cmd[3] = powerSaveType;
  cmd[4] = NWKCOP_END_PACKET;

  status = sendHostCmd(cmd);
  if (status == SUCCESS)
    status = waitRspCmd(rsp);
  if (status == SUCCESS)
    memcpy(&status, &rsp[2], 4);

  return status;
}

/**@brief  Disable power saving
  *@param  None
  *@retval status
  */
uint32_t nwkCop_disablePowerSave(void)
{
  uint32_t status;
  uint8_t cmd[4];
  uint8_t rsp[7];
  
  cmd[0] = NWKCOP_COMMAND_PACKET;
  cmd[1] = 0x01;
  cmd[2] = NWKCOP_POWER_UP;
  cmd[3] = NWKCOP_END_PACKET;

  status = sendHostCmd(cmd);
  if (status == SUCCESS)
    status = waitRspCmd(rsp);
  if (status == SUCCESS)
    memcpy(&status, &rsp[2], 4);

  return status;
}

/**@brief  Erase NVM nwkCop
  *@param  None
  *@retval status
  */
uint32_t nwkCop_NVM_Erase(void)
{
  uint32_t status;
  uint8_t cmd[4];
  uint8_t rsp[7];
  
  cmd[0] = NWKCOP_COMMAND_PACKET;
  cmd[1] = 0x01;
  cmd[2] = NWKCOP_NVM_ERASE;
  cmd[3] = NWKCOP_END_PACKET;

  status = sendHostCmd(cmd);
  if (status == SUCCESS)
    status = waitRspCmd(rsp);
  if (status == SUCCESS)
    memcpy(&status, &rsp[2], 4);

  return status;
}

/**@brief  NWK configuration
  *@param  parameter identifier
  *@param  parameter value  
  *@retval status
  */
uint32_t NWK_Config(uint32_t parameterID, void *parameterValue)
{
  uint32_t status;
  uint8_t cmd[16];
  uint8_t rsp[7];
  
  cmd[0] = NWKCOP_COMMAND_PACKET;
  cmd[2] = NWKCOP_NWK_CONFIG;  
  memcpy(&cmd[3], &parameterID, 4);
  if (parameterID == nwkcVendorIdentifier_ID) {
    cmd[1] = 0x07;
    memcpy(&cmd[7], parameterValue, 2);
    cmd[9] = NWKCOP_END_PACKET;
  } else {
    if (parameterID == nwkcVendorString_ID) {
      cmd[1] = 13;
      memcpy(&cmd[7], parameterValue, 8);
      cmd[15] = NWKCOP_END_PACKET;
    }
  }

  status = sendHostCmd(cmd);
  if (status == SUCCESS)
    status = waitRspCmd(rsp);
  if (status == SUCCESS)
    memcpy(&status, &rsp[2], 4);

  return status;
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
