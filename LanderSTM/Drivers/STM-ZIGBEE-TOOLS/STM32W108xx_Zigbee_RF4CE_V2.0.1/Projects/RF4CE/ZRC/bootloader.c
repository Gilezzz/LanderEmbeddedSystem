/**
  ******************************************************************************
  * @file    bootloader.c
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012
  * @brief   bootloader
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
#include "board.h"
#include <stdio.h>
#include "error.h"
#include "hal.h"
#include "timer.h"
#include "rf4ce.h"
#include "command-interpreter2.h"
#include "cerc_rf4ce.h"
#include "bl.h"
#include "iap_bootloader.h"

#ifdef __CC_ARM
uint32_t *APPLICATION_IMAGE_START = APPLICATIONIMAGE_START;
uint32_t *APPLICATION_IMAGE_END  = APPLICATIONIMAGE_END ;
#endif
/** @addtogroup ZRC_demos
  * @{
  */

/** @addtogroup bootloader
  * @{
  */

/* Private defines -----------------------------------------------------------*/
#define GET         0x00
#define GET_VERSION 0x01
#define GET_ID      0x02
#define ERASE       0x43
#define WRITE       0x31
#define READ        0x11
#define GO          0x21

/* Private typedef -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

enum txStatus {
  TX_SUCCESS = 0,
  TX_PENDING = 1,
  TX_FAILED = 2
};
#pragma pack(1)
typedef struct txBufferTypeStruct {
  IEEEAddr destEui64;
  uint8_t payload[128];
  __IO uint8_t status;
  uint8_t pointer;
  uint8_t channel;
  uint16_t panID;
  bool broadcast;
} txBufferType;
typedef struct rxBufferTypeStruct {
  uint8_t payload[128];
  uint16_t lastSequenceNumber;
  __IO uint8_t packetReceived;
  uint8_t payloadLength;
  uint8_t payloadPointer;
  IEEEAddr srcEui64;
} rxBufferType;
#pragma pack()


ALIGN_VAR(txBufferType txBuffer, 2);
rxBufferType rxBuffer;

static uint8_t transmitByte(uint8_t *data, uint8_t lastByte);
static uint8_t receiveByte(uint8_t *data);
static void txBufferInit(void);
static void rxBufferInit(void);
void blDemoInit(void);

static char *commandName(uint8_t cmdCode)
{
  char *returnValue = "Unknown";
  switch (cmdCode) {
  case GET: returnValue = "GET";
    break;
  case GET_VERSION: returnValue = "GET_VERSION";
    break;
  case GET_ID: returnValue = "GET_ID";
    break;
  case ERASE: returnValue = "ERASE";
    break;
  case WRITE: returnValue = "WRITE";
    break;
  case READ: returnValue = "READ";
    break;
  case GO: returnValue = "GO";
    break;
  default: 
    break;
  }

  return returnValue;
}

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
extern void MAC_SetRadioChannel(uint8_t channel);
extern void MAC_SetPanId(uint16_t panID);		

/**
  * @brief  Set Destination EUI64 action
  * @param  None
  * @retval None
  */
void setDestEui64Action (void)
{
  IEEEAddr destEui64;
  copyStringArgument(0, (uint8_t *) &destEui64, 8, 0);
  { int i; for (i = 7; i >= 0; i--) {
      txBuffer.destEui64[i] = destEui64[7-i];
    }
  }
  printf("{SetDestEui64 call... OK}\r\n");
}

/**
  * @brief  Get Destination EUI64 action
  * @param  None
  * @retval None
  */
void getDestEui64Action (void)
{
  printf("{getEui64 call... OK, {destEui64:");
  { int i; for (i = 7; i >= 0; i--) {
      printf("%02x",(txBuffer.destEui64)[i]);
    }
    printf("}");
  }
  printf("}\r\n");
}

/**
  * @brief  Long Image
  * @param  None
  * @retval None
  */
void loadImage (void)
{
  uint32_t bytesWritten;
  uint32_t state;
  uint32_t startTime, totalTime = 0;
  uint8_t  failed = 0;
  imageDescType imageDesc;
    
  imageDesc.imagePtr = (uint8_t *)APPLICATION_IMAGE_START;
  imageDesc.imageSize =((APPLICATION_IMAGE_END - APPLICATION_IMAGE_START) + 1) & 0xFFFFFFFE;
#ifdef STM32W108CB
  imageDesc.imageOffset = IAP_BOOTLOADER_APPLICATION_ADDRESS - MFB_BOTTOM;
#else
  imageDesc.imageOffset = 0;
#endif

  printf ("Loading image on remote node with EUI64: ");
  { int i; for (i = 7; i >= 0; i--) {
      printf("%02X",(txBuffer.destEui64)[i]);
    }
  }
  printf ("\r\n");
  
  txBufferInit();
  rxBufferInit();
  blInit(&imageDesc,
	 transmitByte, 
	 receiveByte);

  while (TRUE) {
    startTime = TIME_CurrentTime();
    state = blStateMachine(&bytesWritten);
    totalTime += TIME_CurrentTime() - startTime;
    if (state == BL_STATE_FAILURE)
      failed = 1;
    printf ("Written %05lu/%05lu\r", bytesWritten, imageDesc.imageSize);
    CERC_Tick();
    if (state == BL_STATE_IDLE) {
      printf ("\r\n%s\r\n", failed ? "Failed" : "OK");
      break;
    }
  }
  if (!failed)
    printf ("Total time (not including screen messages) = %lu.%06lu sec.\r\n", (totalTime)/1000000, (totalTime) % 1000000);
}

/**
  * @brief  Command execution
  * @param  cmdCode command code
  * @param  *args pointer on command argument type
  * @param  replyLength reply length
  * @param  *reply pointer on reply
  * @retval None
  */
static void cmdExecution(uint8_t cmdCode, btlCommandArgType *args, uint8_t replyLength, uint8_t *reply)
{
  uint8_t status;
  char *command;

  txBufferInit();
  rxBufferInit();

  status = bootloadCommand(cmdCode, args, replyLength, reply);

  command = commandName(cmdCode);

  printf("{%s Bootload command... status %02x \r\n", command, status);
  printf("{reply:");
  {
    uint8_t i;
    for (i = 0; i < replyLength; i++) {
      printf("%02x", reply[i]);
    }
    printf("}");
  }
  printf ("}\r\n");
}

/**
  * @brief  Get Command Action
  * @param  None
  * @retval None
  */
void getCommandAction(void)
{
  uint8_t reply[10];

  cmdExecution(GET, NULL, sizeof(reply), reply); 
}

/**
  * @brief  Get Id Command Action
  * @param  None
  * @retval None
  */
void getIdCommandAction(void)
{
  uint8_t reply[3];

  cmdExecution(GET_ID, NULL, sizeof(reply), reply); 
}

/**
  * @brief  Get version Command Action
  * @param  None
  * @retval None
  */
void getVersionCommandAction(void)
{
  uint8_t reply[3];

  cmdExecution(GET_VERSION, NULL, sizeof(reply), reply); 
}

/**
  * @brief  Read Command Action
  * @param  None
  * @retval None
  */
void readCommandAction(void)
{
  uint8_t reply[128];
  uint32_t address;
  uint32_t bigEndianAddress;
  uint8_t bytes;
  btlCommandArgType args[3];  

  address = unsignedCommandArgument(0);
  args[0].length = 4;
  args[0].data = (uint8_t *) &bigEndianAddress;

  /* Address must be sent big endian */
  {
    uint8_t i;
    uint8_t *addr = (uint8_t *) &address;
    uint8_t *BEaddr = (uint8_t *) &bigEndianAddress;
    for (i = 0; i < 4; i++)
      BEaddr[3-i] = addr[i];
  }

  bytes = unsignedCommandArgument(1) - 1;
  args[1].length = 1;
  args[1].data = &bytes;

  /* Terminate args list */
  args[2].length = 0;

  cmdExecution(READ, args, bytes + 1, reply); 
}

/**
  * @brief  Write Command Action
  * @param  None
  * @retval None
  */
void writeCommandAction(void)
{
  uint8_t data[128];
  uint32_t address;
  uint32_t bigEndianAddress;
  btlCommandArgType args[3];  

  address = unsignedCommandArgument(0);
  args[0].length = 4;
  args[0].data = (uint8_t *) &bigEndianAddress;

  /* Address must be sent big endian */
  {
    uint8_t i;
    uint8_t *addr = (uint8_t *) &address;
    uint8_t *BEaddr = (uint8_t *) &bigEndianAddress;
    for (i = 0; i < 4; i++)
      BEaddr[3-i] = addr[i];
  }

  data[0] = unsignedCommandArgument(1) - 1;
  copyStringArgument(2, (uint8_t *) data+1, data[0] + 1, 0);

  args[1].length = data[0] + 2;
  args[1].data = data;

  /* Terminate args list */
  args[2].length = 0;

  cmdExecution(WRITE, args, 0, NULL); 
}

/**
  * @brief  Erase Command Action
  * @param  None
  * @retval None
  */
void eraseCommandAction(void)
{
  uint8_t data[128];
  btlCommandArgType args[2];  

  data[0] = unsignedCommandArgument(0) - 1;
  copyStringArgument(1, (uint8_t *) data+1, data[0] + 1, 0);

  args[0].length = data[0] + 2;
  args[0].data = data;

  /* Terminate args list */
  args[1].length = 0;

  cmdExecution(ERASE, args, 0, NULL); 
}

/**
  * @brief  Go Address Command Action
  * @param  None
  * @retval None
  */
void goAddressCommandAction(void)
{
  uint32_t address = unsignedCommandArgument(0);
  uint32_t bigEndianAddress;
  btlCommandArgType args[2];  

  args[0].length = 4;
  args[0].data = (uint8_t *) &bigEndianAddress;
  args[1].length = 0;
  /* Address must be sent big endian */
  {
    uint8_t i;
    uint8_t *addr = (uint8_t *) &address;
    uint8_t *BEaddr = (uint8_t *) &bigEndianAddress;
    for (i = 0; i < 4; i++)
      BEaddr[3-i] = addr[i];
  }
  cmdExecution(GO, args, 0, NULL); 
}

/**
  * @brief  Bootloader Mode UART Action
  * @param  None
  * @retval None
  */
void blModeUartAction(void)
{
  printf ("Entering Uart bootloader mode ...\r\n");
  fflush(stdout);
  halBootloaderStart(BOOTLOADER_MODE_UART, IAP_BOOTLOADER_DEFAULT_CHANNEL, IAP_BOOTLOADER_PAN_ID);
  printf ("Node does not contain IAP bootloader\r\n");
}

/**
  * @brief  Bootloader Mode OTA Action
  * @param  None
  * @retval None
  */
void blModeOTAAction(void)
{
  PairingTableEntryType pairT;
  NLME_GET_Type param;
  uint8_t pairingRef = unsignedCommandArgument(0);

  pairT.status = 0;

  if (pairingRef < nwkcMaxPairingTableEntries) {
    /* Retreive pairing table info */
    param.NIBAttribute = nwkPairingTable_ID;
    param.NIBAttributeValue = (uint8_t*)&pairT;
    param.NIBAttributeIndex = pairingRef;
    NLME_Get(&param);
  }

  if (pairingRef == 0xFF) {
    pairT.destinationLogicalChannel = IAP_BOOTLOADER_DEFAULT_CHANNEL;
    pairT.destPANIdentifier = IAP_BOOTLOADER_PAN_ID;
  } else {
    if (pairT.status != 0x02) {
      printf ("Invalid pairing table entry %d\r\n", pairingRef);
      return;
    }
  }

  printf ("Entering RF bootloader mode ...\r\n");
  fflush(stdout);
  halBootloaderStart(BOOTLOADER_MODE_OTA, pairT.destinationLogicalChannel, pairT.destPANIdentifier);
  printf ("Node does not contain IAP bootloader\r\n");

}

/**
  * @brief  Find Bootloader Node
  * @param  *eui64
  * @retval None
  */
bool findBootloaderNode(uint8_t *eui64)
{
  uint8_t reply[10];
  uint8_t status;
  bool retVal=0;

  txBufferInit();
  rxBufferInit();
  blInit(NULL, transmitByte, receiveByte);

  txBuffer.broadcast = TRUE;

  status = bootloadCommand(GET, NULL, sizeof(reply), reply);

  if (status == BOOTLOAD_COMMAND_SUCCESS) {
    uint32_t startTime;
    halCommonMemCopy(eui64, &rxBuffer.srcEui64, sizeof(IEEEAddr));
    retVal = 1;

    startTime = TIME_CurrentTime();
    while (TIME_ELAPSED(startTime) < 1000000) {
      if (rxBuffer.packetReceived) {
	startTime = TIME_CurrentTime();
	rxBuffer.packetReceived = FALSE;
      }
    }
  } 

  return retVal;
}

/**
  * @brief Save context
  * @param  *panID PAN Id
  * @param  *channel Channel
  * @retval None
  */
void saveContext(uint16_t *panID, uint8_t *channel)
{
  NLME_GET_Type param;

  /* Get Default PANId */
  param.NIBAttribute = 0x50; //macPANId_ID;
  param.NIBAttributeValue = (uint8_t*)panID;
  param.NIBAttributeIndex = 0;
  NLME_Get(&param);

  /* Get Base channel */
  param.NIBAttribute = nwkBaseChannel_ID;
  param.NIBAttributeValue = (uint8_t*)channel;
  param.NIBAttributeIndex = 0;
  NLME_Get(&param);
}

/**
  * @brief Restore context
  * @param  *panID PAN Id
  * @param  *channel Channel
  * @retval None
  */
void restoreContext(uint16_t *panID, uint8_t *channel)
{
  NLME_SET_Type val;

  /* Restore PANId */
  val.NIBAttribute = 0x50; /* macPANId_ID;*/
  val.NIBAttributeIndex = 0;
  val.NIBAttributeValue = (uint8_t *)panID;
  NLME_Set(&val);
  
  /* Restore base channel */
  val.NIBAttribute = nwkBaseChannel_ID;
  val.NIBAttributeIndex = 0;
  val.NIBAttributeValue = (uint8_t *)channel;
  NLME_Set(&val);
}

/**
  * @brief clone Image
  * @param  pairingRef pairing reference
  * @param  isTarget Target state
  * @retval error status
  */
uint8_t cloneImage(int8_t pairingRef, uint8_t isTarget)
{
  uint16_t panId;
  uint8_t baseChannel;
  PairingTableEntryType pairT;
  NLME_GET_Type param;
  NLME_SET_Type val;
  uint8_t error = FALSE;

  pairT.status = 0;

  /* Save panId & baseChannel, because it could be modified */
  saveContext(&panId, &baseChannel);

  if (pairingRef != -1) {
    if (pairingRef < nwkcMaxPairingTableEntries) {
      /* Retreive pairing table info */
      param.NIBAttribute = nwkPairingTable_ID;
      param.NIBAttributeValue = (uint8_t*)&pairT;
      param.NIBAttributeIndex = pairingRef;
      NLME_Get(&param);
    }
    if (isTarget)
      /* Do not move from base channel since there is no guaranteed on which channel the
         controller is */
      pairT.destinationLogicalChannel = baseChannel;
    if (pairT.status != 0x02) {
      printf ("Invalid pairing table entry %d\r\n", pairingRef);
      error = TRUE;
    }
  } else {
    if (panId == 0xFFFF)
      pairT.destPANIdentifier = IAP_BOOTLOADER_PAN_ID;
    else
      pairT.destPANIdentifier = panId;
    pairT.destinationLogicalChannel = IAP_BOOTLOADER_DEFAULT_CHANNEL;
  }
  
  if (error == FALSE) {
    /* Set panId */
    val.NIBAttribute = 0x50; /* macPANId_ID; */
    val.NIBAttributeIndex = 0;
    {
      uint16_t tempPanId = pairT.destPANIdentifier;
      val.NIBAttributeValue = (uint8_t *)&tempPanId;
      NLME_Set(&val);
    }

#ifdef STM32W108CB
    txBuffer.panID = pairT.destPANIdentifier;
    txBuffer.channel = pairT.destinationLogicalChannel;
#else
    txBuffer.panID = IAP_BOOTLOADER_PAN_ID;
    txBuffer.channel = IAP_BOOTLOADER_DEFAULT_CHANNEL;
    MAC_SetRadioChannel(txBuffer.channel);
    MAC_SetPanId(txBuffer.panID);
#endif

  }
  /* Enter MAC Mode */
  if (NWK_EnableMACMode(1) != SUCCESS)
    printf("Error during enable MAC mode \r\n");

  if (pairingRef == -1) {
    /* Find the destination EUI64 */
    if (!findBootloaderNode (pairT.destinationIEEEAddress)) {
      printf("Error finding node in bootloader mode\r\n");
      error = TRUE;
    }
  }
  
  if (error == FALSE) {
    txBuffer.broadcast = FALSE;
    { int i; for (i = 7; i >= 0; i--) {
	txBuffer.destEui64[i] = pairT.destinationIEEEAddress[i];
      }
    }
    
    /* Load Image Action */
    loadImage();
  } 

  /* Exit from MAC Mode */
  if (NWK_EnableMACMode(0) != SUCCESS)
    printf("Error during disable MAC mode \r\n");    

  /* Restore original value of panID & baseChannel after bootloader mechanism */
  restoreContext(&panId, &baseChannel);

  return error;
}

/**
  * @brief clone Image Action
  * @param  None
  * @retval None
  */
void cloneImageAction(void)
{
  NLME_GET_Type param;
  uint8_t pairingRef = unsignedCommandArgument(0);
  uint8_t nodeCap;

  /* Retreive pairing table info */
  param.NIBAttribute = 0x92; /* nwkcNodeCapabilities_ID; */
  param.NIBAttributeValue = &nodeCap;
  param.NIBAttributeIndex = 0;
  NLME_Get(&param);

  cloneImage (pairingRef, nodeCap & 1);
}

/**
  * @brief Enable Bootloader Action
  * @param  None
  * @retval None
  */
void enableBootloaderAction(void)
{
  uint8_t param;
  uint32_t status;

  param = unsignedCommandArgument(0);

  if (param) {
    NLME_SET_Type val;
    uint16_t panID = IAP_BOOTLOADER_PAN_ID;
    
    /* Restore PANId */
    val.NIBAttribute = 0x50; /* macPANId_ID; */
    val.NIBAttributeIndex = 0;
    val.NIBAttributeValue = (uint8_t *)&panID;
    NLME_Set(&val);
    blDemoInit();
    txBuffer.panID = IAP_BOOTLOADER_PAN_ID;;
    txBuffer.channel = IAP_BOOTLOADER_DEFAULT_CHANNEL;
  }
  status = NWK_EnableMACMode(param);
  
  printf("NWK_EnableMACMode utility call... status %08lX\r\n", status);
}

/**
  * @brief Send RAW Packet Action
  * @param  None
  * @retval None
  */
void sendRawPacketAction(void)
{
  uint32_t status;
  macPacket_Type packet;
  uint8_t rawPayload[128];

  packet.channel = unsignedCommandArgument(0);
  packet.dstAddrMode = unsignedCommandArgument(1);
  if (packet.dstAddrMode == 2)
    copyStringArgument(2, (uint8_t *) &packet.macDstAddr.ShortAddress, 2, 0);
  else
    copyStringArgument(2, (uint8_t *) &packet.macDstAddr.IEEEAddress, 8, 0);
  packet.dstPANId = unsignedCommandArgument(3);
  packet.payloadLength = unsignedCommandArgument(4);
  copyStringArgument(5, rawPayload, packet.payloadLength, 0);
  packet.payload = rawPayload;

  status = NWK_SendMACPacket(&packet);
  printf("NWK_SendRawPacket utility call... status %08lX\r\n", status);
}

/**
  * @brief tarnsmit Byte
  * @param *data pointer on data to trnsmit
  * @param lastByte last Byte transmitted
  * @retval transmit status
  */
static uint8_t transmitByte(uint8_t *data, uint8_t lastByte)
{
  uint32_t startTime;

  uint8_t returnValue = TX_SUCCESS;;
  macPacket_Type pkt;

  txBuffer.payload[txBuffer.pointer++] = *data;
  if (lastByte) {
    txBuffer.status = TX_PENDING;
    
    pkt.channel = txBuffer.channel;
    pkt.srcAddrMode = 3;
    NWK_getEui64(pkt.macSrcAddr.IEEEAddress);
    if (txBuffer.broadcast) {
      pkt.dstAddrMode = 2;
      pkt.macDstAddr.ShortAddress = 0xffff;
      pkt.dstPANId = 0xffff; 
      pkt.ackRequired = FALSE;
    } else {
      pkt.dstAddrMode = 3;
      halCommonMemCopy (pkt.macDstAddr.IEEEAddress, txBuffer.destEui64, sizeof(IEEEAddr));
      pkt.dstPANId = txBuffer.panID; 
      pkt.ackRequired = TRUE;
    }
    pkt.srcPANId = txBuffer.panID;
    pkt.payloadLength = txBuffer.pointer;
    pkt.payload = txBuffer.payload;

    while (txBuffer.status == TX_PENDING) {
      if (NWK_SendMACPacket(&pkt) != RF4CE_SAP_PENDING) {
	txBuffer.status = TX_FAILED;
      } 
    
      /* Wait for transmission to complete */
      startTime = TIME_CurrentTime();
      while ((txBuffer.status == TX_PENDING) && (TIME_ELAPSED(startTime) < ((pkt.payloadLength+23+10) *16 + 900))) {
	CERC_Tick();
      }
    }

    returnValue = txBuffer.status;
    txBuffer.pointer = 0;
  }
  return returnValue;
}

/**
  * @brief receive Byte
  * @param *data pointer on data to receive
  * @retval receive status
  */
static uint8_t receiveByte(uint8_t *data)
{
  uint8_t returnValue = FALSE;
  if (rxBuffer.packetReceived) {
    *data = rxBuffer.payload[rxBuffer.payloadPointer++];
    if (rxBuffer.payloadPointer >= rxBuffer.payloadLength)
      rxBuffer.packetReceived = FALSE;
    returnValue = TRUE;
  }
  return returnValue;
}

/**
  * @brief Tx Buffer Initialization
  * @param None
  * @retval None
  */
static void txBufferInit(void)
{
  txBuffer.status = 0;
  txBuffer.pointer = 0;
  txBuffer.broadcast = FALSE;
}

/**
  * @brief Rx Buffer Initialization
  * @param None
  * @retval None
  */
static void rxBufferInit(void)
{
  halCommonMemSet (&rxBuffer, 0x00, sizeof(rxBuffer));

  rxBuffer.lastSequenceNumber = 0xffff; // Invalid value
}

/**
  * @brief bl demo Initialization
  * @param None
  * @retval None
  */
void blDemoInit(void)
{
  txBufferInit();
  rxBufferInit();
  blInit(NULL, transmitByte, receiveByte);
}

/********************** Bootloader callbacks functions ************************/
/**
  * @brief network incoming MAC message handler
  * @param *msg pointer on message to be handled
  * @retval None
  */
void NWK_IncomingMACMessageHandler(rawPacketReceivedType *msg)
{
  if (msg->SequenceNumber != rxBuffer.lastSequenceNumber) {
    if(rxBuffer.packetReceived == FALSE) {
      halCommonMemCopy(&rxBuffer.payload, msg->FramePayload, msg->FramePayloadLength);
      rxBuffer.packetReceived = TRUE;
      rxBuffer.payloadPointer = 0;
      rxBuffer.payloadLength = msg->FramePayloadLength;
      halCommonMemCopy(&rxBuffer.srcEui64, msg->rawSourceAddress.IEEEAddress, sizeof(IEEEAddr));
    } else {
      /* This should not happen according to the protocol definition */
    }
    rxBuffer.lastSequenceNumber = msg->SequenceNumber;
  }
}
/**
  * @brief network transmit MAC message handler
  * @param Transmit MAC message status
  * @retval None
  */
void NWK_TransmitMACMessageHandler(uint32_t status)
{
  if (status != 0) {
    txBuffer.status = TX_FAILED;
  } else {
    txBuffer.status = (uint8_t)status;
  }
}

/**
  * @}
  */

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
