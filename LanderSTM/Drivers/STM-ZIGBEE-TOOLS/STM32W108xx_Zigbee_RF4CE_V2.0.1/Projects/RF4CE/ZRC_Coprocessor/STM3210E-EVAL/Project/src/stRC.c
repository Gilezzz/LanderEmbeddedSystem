/**
  ******************************************************************************
  * @file    stRC.c
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   stRC program body
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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "rf4ce.h"
#include "stm32_sc.h"
#include "cerc_rf4ce.h"
#include "cerc_constants.h"
#include "serial_utils.h"
#include "stm32_eval.h"
#include "stRC.h"


#define NUMBER_OF_COMMANDS (sizeof(cmdTable) / sizeof(cmdTableType))

/* Private define ------------------------------------------------------------*/
/* Button Management */
/* Time (ms) to wait with the button S1 pressed to erase the NVM device */
#define WAIT_TIME_BUTTON_PRESS 1500
#define WAIT_BOUNCE_TIME 50
#define RELEASED       0x0
#define PRESSED        0x1
#define WAIT_RELEASE   0x2
#define RELEASED_SHORT 0x3
#define PRESSED_LONG   0x4
#define BUTTON_INIT    0x5
uint8_t buttonRead(void);

/* Led Management */
#define HEART_BEAT_TIME 500
#define BLINKING_TIME 100
#define LED_OFF      0x0
#define LED_ON       0x1
#define LED_BLINKING 0x2
#define LED_INIT     0x3
#define LED_DUMMY    0x4

void heartBeatLed(void);
void statusLed(uint8_t status);

/* Value Dummy for GPIO during power down/up */
#define DUMMY_GPIO_STATE_DOWN GPIO_Mode_OUT_OD /* GPIO_Mode_OUT_OD: output, open-drain */
#define DUMMY_GPIO_STATE_UP GPIO_Mode_OUT_OD
/* define the dummy gpio output value for Power Down */
#define DUMMY_GPIO_VALUE_DOWN  0
/* define the dummy gpio output value for Power Up */
#define DUMMY_GPIO_VALUE_UP  0 

/* Private typedef -----------------------------------------------------------*/

/* Added enum type to determine node type */
typedef enum {
  UNCONFIG = -1,
  CONTROLLER = 0,
  TARGET = 1
} NodeType;

/* Circular buffer to store the command key received */
#define MAX_CMD_BUFFER_SIZE 100
typedef struct cmdBuffer_S {
  uint8_t top;
  uint8_t bottom;
  uint8_t cmd[MAX_CMD_BUFFER_SIZE];
} cmdBuffer_Type;

typedef struct cmdTableStruct {
  uint8_t cmdCode;
  void (*cmdFunction)(void *);
  void * cmdParams;
  char *helpString;
} cmdTableType;

typedef struct cfgDeviceParamsStruct {
  bool target;
  bool forceColdStart;
} cfgDeviceParamsType;

static const bool powerSaveEn = TRUE;
static const bool powerSaveDis = FALSE; 
static const cfgDeviceParamsType cfgDeviceActionTarget = {TRUE};
static const cfgDeviceParamsType cfgDeviceActionController = {FALSE};
static const uint8_t volumeUp    = RF4CE_CERC_VOLUME_UP;
static const uint8_t volumeDown  = RF4CE_CERC_VOLUME_DOWN;
static const uint8_t channelUp   = RF4CE_CERC_CHANNEL_UP;
static const uint8_t channelDown = RF4CE_CERC_CHANNEL_DOWN;
static const uint8_t numbers[] = {
  RF4CE_CERC_NUM_0,
  RF4CE_CERC_NUM_1,
  RF4CE_CERC_NUM_2,
  RF4CE_CERC_NUM_3,
  RF4CE_CERC_NUM_4,
  RF4CE_CERC_NUM_5,
  RF4CE_CERC_NUM_6,
  RF4CE_CERC_NUM_7,
  RF4CE_CERC_NUM_8,
  RF4CE_CERC_NUM_9
};

static const cmdTableType cmdTable[] = {
  {'T', cfgDeviceAction, (void *) &cfgDeviceActionTarget, "Configure the node as Target"},
  {'C', cfgDeviceAction, (void *) &cfgDeviceActionController, "Configure the node as Controller"},
  {'D', dscPairAction, NULL, "Discovery & Pair procedure"},
  {'Z', dscPairTTAction, NULL, "Discovery & Pair procedure from a TARGET"},
  {'p', dumpPairingTableAction, NULL, "Dump Pairing Table"},
  {'R', rebootAction, NULL, "Device software reset"},
  {'e', erasePairingTableAction, NULL, "Clear Pairing Table"},
  {'E', eraseNVMAction, NULL, "Erase NVM storage"},
  {'S', enablePowerSaveAction, (void *) &powerSaveEn, "Enable power save"},
  {'A', enablePowerSaveAction, (void *) &powerSaveDis, "Disable power save"},
  {'?', cmdMenuAction, NULL, "print help Menu"},
  {'+', sendDataAction, (void *) &volumeUp,   "Send Command Key (PRESSED): Volume Up"},
  {'-', sendDataAction, (void *) &volumeDown, "Send Command Key (PRESSED): Volume Down"},
  {'>', sendDataAction, (void *) &channelUp,  "Send Command Key (PRESSED): Channel Up"},
  {'<', sendDataAction, (void *) &channelDown,"Send Command Key (PRESSED): Channel Down"},
  {'0', sendDataAction, (void *) &numbers[0], "*0-9: Send Command Key (PRESSED): Channel 0-9"},
  {'1', sendDataAction, (void *) &numbers[1], NULL},
  {'2', sendDataAction, (void *) &numbers[2], NULL},
  {'3', sendDataAction, (void *) &numbers[3], NULL},
  {'4', sendDataAction, (void *) &numbers[4], NULL},
  {'5', sendDataAction, (void *) &numbers[5], NULL},
  {'6', sendDataAction, (void *) &numbers[6], NULL},
  {'7', sendDataAction, (void *) &numbers[7], NULL},
  {'8', sendDataAction, (void *) &numbers[8], NULL},
  {'9', sendDataAction, (void *) &numbers[9], NULL},
  {0x1b, NULL, NULL, "*ESC: Enter string command mode"}
};
  
typedef struct cercToIrMapStruct {
  uint8_t cercCode;
  uint8_t irCode;
} cercToIrMapType;

const cercToIrMapType cercToIrMap[] = {
  {RF4CE_CERC_NUM_0,                    9},
  {RF4CE_CERC_NUM_1,                    0},
  {RF4CE_CERC_NUM_2,                    1},
  {RF4CE_CERC_NUM_3,                    2},
  {RF4CE_CERC_NUM_4,                    3},
  {RF4CE_CERC_NUM_5,                    4},
  {RF4CE_CERC_NUM_6,                    5},
  {RF4CE_CERC_NUM_7,                    6},
  {RF4CE_CERC_NUM_8,                    7},
  {RF4CE_CERC_NUM_9,                    8},
  {RF4CE_CERC_CHANNEL_UP,               0x10},
  {RF4CE_CERC_CHANNEL_DOWN,             0x11},
  {RF4CE_CERC_VOLUME_UP,                0x12},
  {RF4CE_CERC_VOLUME_DOWN,              0x13},
  {RF4CE_CERC_MUTE,                     0x14},
  {RF4CE_CERC_POWER,                    0x15},
  {RF4CE_CERC_SELECT,                   101},
  {RF4CE_CERC_UP,                       116},
  {RF4CE_CERC_DOWN,                     117},
  {RF4CE_CERC_LEFT,                     52},
  {RF4CE_CERC_RIGHT,                    51},
  {RF4CE_CERC_PLAY,                     0x1a},
  {RF4CE_CERC_PAUSE,                    0x19},
  {RF4CE_CERC_SELECT_AV_INPUT_FUNCTION, 37},
  {RF4CE_CERC_DISPLAY_INFORMATION,      58},
  {RF4CE_CERC_ROOT_MENU,                96}
};

/* Private macro -------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/

uint8_t interactive;
uint8_t keyCommandMode = TRUE;
NodeType nodeType=UNCONFIG;
uint8_t cmdPayload[3];
int8_t pairRef = -1;
cmdBuffer_Type cmdCode;
static uint32_t startTime=0;

#define MAX_COMMAND_LENGTH 255

#define MAX_WAIT_TO_RELEASE 90 //90 ms

enum {
  MAC_RADIO_POWER_MODE_RX_ON,
  MAC_RADIO_POWER_MODE_OFF
};
bool powerSaveEnabled = FALSE;

uint8_t len;
uint8_t buff[MAX_COMMAND_LENGTH];
uint8_t currIndex = 0;

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

__IO int timeTick1_ms=0;

/**@brief  set the current time
  *@param  None
  *@retval current tick in ms
  */
static int TIME_CurrentTime(void)
{
  return timeTick1_ms;
}

/**@brief  get the elapsed time
  *@param  time
  *@retval elapsed time in ms
  */
static int TIME_ELAPSED(int time)
{
  return (timeTick1_ms - time);
}

/**@brief  reboot the device
  *@param  None
  *@retval None
  */
void halReboot(void)
{
  NVIC_SystemReset();
}

/**@brief  manage powe saving
  *@param  None
  *@retval None
  */
void powerSaveManagement(void)
{
  return;
}

/**@brief  Execute the command.
  *@param  command code
  *@retval None
  */
static void executeCommand (uint8_t cmdCode)
{
  uint8_t i;
  uint8_t found = FALSE;
  for (i = 0; i < NUMBER_OF_COMMANDS; i++)
    if (cmdTable[i].cmdCode == cmdCode) {
      cmdTable[i].cmdFunction(cmdTable[i].cmdParams);
      found = TRUE;
      break;
    }

  if (! found) {
    printf("Unknwon command %c (%02x)\r\n", cmdCode, cmdCode);
  }
}

/**@brief  Execute command string.
  *@param  command
  *@param  lenght  
  *@retval None
  */
static void executeCommandString (uint8_t *cmd)
{
  uint8_t commandOK = FALSE;
  uint8_t len = strlen((const char *) cmd);

  if ((cmd[0] == 'S') && (cmd[1] >= '0') && (cmd[1] <= '9'))  {
    setPairRef(cmd[1] - 48);
    commandOK = TRUE;
  } else  if ((cmd[0] == '{') && (cmd[len - 1] == '}')) {
    uint8_t i = 1;
    uint8_t code = 0;
    uint32_t factor = 1;
    commandOK = TRUE;
    for (i = len - 2; i >= 1; i--) {
      if ((cmd[i] >= '0') && (cmd[i] <= '9')) {
	code = code + ((cmd[i] - 48) * factor);
	factor = factor * 10;
      } else {
	commandOK = FALSE;
	break;
      }
    }
    if (commandOK) {
      if (sendData(CERC_USER_CONTROL_PRESSED, &code) == CERC_DATA_QUEUED)
	startTime = TIME_CurrentTime();
    }
  }
    
  if ((!commandOK) && (len > 0)) {
    printf("Unknwon string command %s\r\n", cmd);
  }
}

/**@brief  processing the command input.
  *@param  None
  *@retval boolean value
  */
bool processCmdInput (uint8_t interactive)
{
  uint8_t byte, buttonState;
   
  if ((startTime != 0) && (TIME_ELAPSED(startTime) > MAX_WAIT_TO_RELEASE)) {
    sendData(CERC_USER_CONTROL_RELEASED, 0);
    startTime = 0;
  }

  statusLed(LED_DUMMY);
  buttonState = buttonRead();

  switch (buttonState) {
  case PRESSED_LONG:
    executeCommand('E');
    break;
  case RELEASED_SHORT:
    if (nodeType == UNCONFIG)
      executeCommand('T');
    else
      if (nodeType != UNCONFIG)
	executeCommand('D');
    break;
  default:
    break;
  }

  if (interactive) {
    if (keyCommandMode == FALSE) {
      if (serialReadPartialLine((char *)buff, MAX_COMMAND_LENGTH, &currIndex)) {

	len = 0;
	/* search foward looking for first CR, LF, or NULL to determine length of the string */
	while((buff[len]!='\n') && (buff[len] !='\r') && (buff[len]!='\0')) {
	  len++;
	}
	buff[len] = '\0';
	currIndex = 0;

	executeCommandString(buff);
	keyCommandMode = TRUE;
	return TRUE;
      } else {
	return FALSE;
      }
      
    } else {
      if (serialReadByte(&byte)) {
	if ((byte == '\r') || (byte == '\n')) {
	  printf("\r\n");
	} else {
	  if (byte == 0x1b) {
	    printf("\r\n String command>");
	    keyCommandMode = FALSE;
	    return FALSE;
	  } else {
	    printf("%c\r\n", byte);
	    executeCommand(byte);
	  }
	}
	return TRUE;
      }
    }
  }
  return FALSE;
}

/**@brief  Print prompt.
  *@param  None
  *@retval None
  */
void printPrompt(void)
{
  switch(nodeType) {
  case  CONTROLLER:
    printf("Controller(%d)>", pairRef);
      break;
  case  TARGET:
      printf("Target(%d)>", pairRef);
      break;
  default:
       printf("Unconfig>");
  }
}

/**@brief  configure device action
  *@param  params
  *@retval None
  */
void cfgDeviceAction (void *voidParams)
{
  cfgDeviceParamsType *params = (cfgDeviceParamsType *) voidParams;

  if ((cfgDevice(params->target, FALSE) != SUCCESS)) {
    cfgDevice(params->target, TRUE);
  }
  if (params->target) {
    nodeType = TARGET;
    cmdCode.top = 0;
    cmdCode.bottom = 0;
    statusLed(LED_INIT);
  } else {
    nodeType = CONTROLLER;
    statusLed(LED_OFF);
  }
}

/**@brief  configure the device.
  *@param  target
  *@param  force cold start  
  *@retval unsigned value
  */
uint32_t cfgDevice(bool target, bool forceColdStart)
{
  CERC_Init_Type param;
  uint32_t status;

  param.coldStart = forceColdStart;
  if (target)
    param.nodeCap = target + 6; /* 6 = Security capable and main powered */
  else
    param.nodeCap = 4; /* 4 = Security capable */
  param.activePeriod = 0xffffff;
  param.discoveryLQIThreshold = 1;
  param.dutyCycle = 0;
  status = CERC_Init(&param);
  if (target && param.coldStart) {
    printf("<CERC_INIT>\r\nState = %08X\r\n", status);
    statusLed(LED_INIT);
    pairRef = -1;
  } else {
    printf("<CERC_INIT>\r\nStatus = %08X\r\n<CERC_INIT_END>\r\n", status);
  }

  if (status == SUCCESS) {
    pairRef = firstValidPairingTableEntry();
  } else {
    pairRef = -1;
  }

  return status;
}

/**@brief  Erase NVM action
  *@param  pointer to the function
  *@retval None
  */
void eraseNVMAction(void *none)
{
  nwkCop_NVM_Erase();
  nodeType=UNCONFIG;
  printf("NVM erased, rebooting..\r\n");
  halReboot();
  statusLed(LED_OFF);
}

/**@brief  Erase erase pairing table.
  *@param  pointer to the function
  *@retval None
  */
void erasePairingTableAction(void *none)
{
  uint8_t i;
  PairingTableEntryType pairT;
  NLME_SET_Type param;
 
  memset(&pairT, 0xff, sizeof(pairT));
  param.NIBAttribute = nwkPairingTable_ID;
  param.NIBAttributeValue = (uint8_t*)&pairT;

  for (i = 0; i < nwkcMaxPairingTableEntries; i++) {
    param.NIBAttributeIndex = i;
    NLME_Set(&param);
  }
  printf("Pairing table erased\r\n");
  pairRef = -1;

}

/**@brief  discovery pairing action.
  *@param  pointer to the function
  *@retval None
  */
void dscPairAction(void *none)
{
  uint32_t status;
  CERC_DscPair_Type param;

  if (nodeType) {
    param.devType = 0x02;/* 0x0e; //IR extender //0x02; //TV */
  } else {
    param.devType = 0x01; /* RC */
    param.searchDevType = 0x02; /* 0x0e; //IR extender  //0x02; //TV */
    param.numberOfKey = 0;
  }
  statusLed(LED_BLINKING);
  param.pairTT = FALSE;
  status = CERC_DscPair(&param);
  printf("<CERC_DSC_PAIR>\r\nStatus = %08X\r\n<CERC_DSC_PAIR_END>\r\n", status);
}

/**@brief  discovery pairing table action.
  *@param  pointer to the function
  *@retval None
  */
void dscPairTTAction(void *none)
{
  uint32_t status;
  CERC_DscPair_Type param;

  if (nodeType != TARGET) {
    printf("Command not supported \r\n");
    return;
  }
  statusLed(LED_BLINKING);
  param.devType = 0x02; /* TV */
  param.pairTT = TRUE;
  param.searchDevType = 0x02; /* 0x0e; //IR extender  //0x02; //TV */
  param.numberOfKey = 0;
  status = CERC_DscPair(&param);
  printf("<CERC_DSC_PAIR>\r\nStatus = %08X\r\n<CERC_DSC_PAIR_END>\r\n", status);
}

/**@brief  send data action.
  *@param  parameter
  *@retval None
  */
void sendDataAction (void *voidParams)
{
  uint8_t *key = (uint8_t *)voidParams;

  if (sendData(CERC_USER_CONTROL_PRESSED, key) == CERC_DATA_QUEUED)
    startTime = TIME_CurrentTime();
}

/**@brief  send data.
  *@param  command identity
  *@param  command key  
  *@retval None
  */
uint32_t sendData(uint8_t cmdId, uint8_t *cmdkey)
{
  PairingTableEntryType pairT;
  NLME_GET_Type par;
  CERC_SendCommand_Type param;
  uint32_t status = CERC_DATA_DISCARDED;

  switch(cmdId) {
  case CERC_USER_CONTROL_PRESSED:
    cmdPayload[0] = CERC_USER_CONTROL_PRESSED;
    cmdPayload[1] = *cmdkey;
    param.commandLength = 2;    
    break;
  case CERC_USER_CONTROL_RELEASED:
    cmdPayload[0] = CERC_USER_CONTROL_RELEASED;
    param.commandLength = 1;
    break;
  }

  if (pairRef >= 0) {
    par.NIBAttribute = nwkPairingTable_ID;
    par.NIBAttributeValue = (uint8_t*)&pairT;
    par.NIBAttributeIndex = pairRef;
    status = NLME_Get(&par);
    if (status != SUCCESS)
      return status;
    param.commandCode = cmdPayload;
    param.pairingRef = pairRef;
    if (pairT.status == 0x02) {
      status = CERC_SendCommand(&param);
    } else {
      printf("Destination Device not paired\r\n");
    }
  } else {
    printf("Invalid pairing reference\r\n");
  }
  return status;
}

/**@brief  pairing table entry.
  *@param  None
  *@retval signed value
  */
int8_t firstValidPairingTableEntry (void)
{
  int8_t i;
  PairingTableEntryType pairT;
  NLME_GET_Type param;
  int8_t retVal = -1;
 
  param.NIBAttribute = nwkPairingTable_ID;
  param.NIBAttributeValue = (uint8_t*)&pairT;

  for (i = 0; i < nwkcMaxPairingTableEntries; i++) {
    param.NIBAttributeIndex = i;
    NLME_Get(&param);
    if (pairT.status == 0x02) {
      retVal = i;
      break;
    }
  }
  return retVal;
}

/**@brief  set pairing reference.
  *@param  index
  *@retval None
  */
void setPairRef (uint8_t index)
{
  PairingTableEntryType pairT;
  NLME_GET_Type param;

  printf("<CERC_SET_ACTIVE_PAIRING_INDEX>\r\n");

  if (index >= nwkcMaxPairingTableEntries) {
    /* Invalid entry */
    printf("Out of range, index: %d, leaving unchanged\r\n", index);
  } else {
    param.NIBAttribute = nwkPairingTable_ID;
    param.NIBAttributeValue = (uint8_t*)&pairT;
    param.NIBAttributeIndex = index;
    NLME_Get(&param);
    if (pairT.status == 0x02) {
      pairRef = index;
    } else {
      printf("Invalid index: %d, leaving unchanged\r\n", index);
    }
  }
  printf("Active pairing index: %d\r\n", pairRef);
  printf("<CERC_SET_ACTIVE_PAIRING_INDEX_END>\r\n");

  return;
}

/**@brief  dump pairing table entry.
  *@param  pointer to the function
  *@retval None
  */
void dumpPairingTableAction (void *none)
{
  int8_t i, j;
  uint8_t recCap;
  PairingTableEntryType pairT;
  NLME_GET_Type param;
 
  param.NIBAttribute = nwkPairingTable_ID;
  param.NIBAttributeValue = (uint8_t*)&pairT;

  printf("<CERC_PAIRING_TABLE>\r\n");
  printf("c status sNWK CH dstIEEEAddr      dPAN dNWK frmcnt   cp linkKey\r\n");
  for (i = 0; i < nwkcMaxPairingTableEntries; i++) {
    param.NIBAttributeIndex = i;
    NLME_Get(&param);
    
    recCap = pairT.recipientCapabilities.nodeType << 7 | 
      pairT.recipientCapabilities.powerSource << 6 |
      pairT.recipientCapabilities.securityCapable << 5 | 
      pairT.recipientCapabilities.channelNormalizationCapable << 4 |
      pairT.recipientCapabilities.reserved;

    printf("%d", i);
    printf(" %02X    ", pairT.status);
    printf(" %04X", pairT.sourceNetworkAddress);
    printf(" %02X", pairT.destinationLogicalChannel);
    printf(" ");
    { for (j = 7; j >= 0; j--) {printf("%02X", pairT.destinationIEEEAddress[j]);}}
    printf(" %04X", pairT.destPANIdentifier);
    printf(" %04X", pairT.destinationNetworkAddress);
    printf(" %08X", pairT.recipientFrameCounter);
    printf(" %02X", recCap);
    printf(" ");
    { for (j = 0; j < 16; j++) {printf("%02X", pairT.securityLinkKey[j]);}}
    printf("\r\n");
  }
  printf("<CERC_PAIRING_TABLE_END>\r\n");
}

/**@brief  reboot the device.
  *@param  pointer to the function
  *@retval None
  */
void rebootAction(void *none)
{
  halReboot();
}

/**@brief  warm start.
  *@param  None
  *@retval None
  */
void warmStart(void)
{
  if (cfgDevice((bool)CONTROLLER, FALSE) == SUCCESS) {
    nodeType = CONTROLLER;
  } else {
    if (cfgDevice((bool)TARGET, FALSE) == SUCCESS) {
      nodeType = TARGET;
      cmdCode.top = 0;
      cmdCode.bottom = 0;
    }
    else 
      nodeType = UNCONFIG;
  }
}

/**@brief  command menu action.
  *@param  pointer to the function
  *@retval None
  */
void cmdMenuAction(void *none)
{
  uint8_t i;
  printf("************** Command Menu **************\r\n");
  for (i = 0; i < NUMBER_OF_COMMANDS; i++)
    if (cmdTable[i].helpString != NULL) {
      if (cmdTable[i].helpString[0] != '*')
	printf("%c: %s\r\n", cmdTable[i].cmdCode, cmdTable[i].helpString);
      else {
	printf("%s\r\n", cmdTable[i].helpString + 1);
      }
    }
}

/**@brief  ir command interpreter.
  *@param  command
  *@retval None
  */
void irCmdInterpreter(uint8_t *cmd)
{
  static uint8_t code=0xff;

  /* Nothing to do -Command Release- */
  if (cmd[0] == 0x03) {
    uint8_t i;
    if (code != 0xff) {
      for (i = 0; i < 2; i++) {
	cmdCode.cmd[cmdCode.top] = code;
	cmdCode.top = (cmdCode.top + 1) % MAX_CMD_BUFFER_SIZE; 
      }
    }
    return;
  }

  if (cmd[0] != 0x02) {
    uint8_t i;
    for (i = 0; i < sizeof(cercToIrMap) / sizeof (cercToIrMapType); i++) {
      if (cercToIrMap[i].cercCode == cmd[1]) {
	code = cercToIrMap[i].irCode;
	break;
      }
    }
  }
  if (code != 0xff) {
    cmdCode.cmd[cmdCode.top] = code;
    cmdCode.top = (cmdCode.top + 1) % MAX_CMD_BUFFER_SIZE;
  }
}

/**@brief  enable power saving action.
  *@param  pointer to the value
  *@retval None
  */
void enablePowerSaveAction(void *val)
{
  bool *enable = (bool*)val;
 
  if (nodeType != CONTROLLER) {
    printf("Power Save not supported for this device\r\n");
    return;
  }
  if (*enable) {
    powerSaveEnabled = TRUE;
    printf("POWER SAVE enabled\r\n");
  } else {
    powerSaveEnabled = FALSE;
    printf("POWER SAVE disabled\r\n");
  }

}

/**@brief  NVIC configuration
  *@param  None
  *@retval None
  */
void NVIC_Configuration(void)
{
   NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the USARTz Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**@brief  Initialize the board
  *@param  None
  *@retval None
  */
void boardInit(void)
{
  USART_InitTypeDef USART_InitStructure;
  
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);

  /* USARTx configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  NVIC_Configuration();
  STM_EVAL_COMInit(COM1, &USART_InitStructure);
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

  /* Turn on leds available on STM3210X-EVAL **********************************/
  STM_EVAL_LEDOff(LED1);
  STM_EVAL_LEDOff(LED2);
  STM_EVAL_LEDOff(LED3);
  STM_EVAL_LEDOff(LED4);
}

uint8_t cmdCallBack[200];

/**@brief  main function
  *@param  None
  *@retval None
  */
int main (void)
{
  interactive = 1;
  keyCommandMode = TRUE;
  uint32_t status;
#ifdef SC_SPI
  bool isSPI=TRUE;
#else
  bool isSPI=FALSE;
#endif
  
  hostSerialInit(isSPI);
    
  boardInit();

  printf("RF4CE CERC application (lib version:%s)\r\n", RF4CE_VERSION_STRING);

  STM_EVAL_PBInit(BUTTON_KEY, BUTTON_MODE_GPIO);
  
  STM_EVAL_LEDOn(LED1);
  
  warmStart();

  if (nodeType == TARGET)
    statusLed(LED_ON);
  else
    statusLed(LED_OFF);

  while(1) {
    heartBeatLed();
    /* Process input and print prompt if it returns TRUE. */
    if (processCmdInput(interactive)) {
      if (interactive) {
	printPrompt();
      }
    }
    
    CERC_Tick();

    if (startTime == 0) {
      if (CERC_EnablePowerSave(powerSaveEnabled)) {
	powerSaveManagement();
      }
    }

    if (!isHwakeHigh()) {
      status = receiveHostCmd(cmdCallBack);
      if (status != SUCCESS) {
	printf("Error during send the command status = 0x%08x\r\n", status);
      } else {
	RspCallback(cmdCallBack);
      }
    }
  }
}

/**@brief  cerc init callback.
  *@param  status
  *@retval None
  */
void CERC_InitCallback(uint32_t *status)
{
  printf("Status = %08X\r\n<CERC_INIT_END>\r\n", *status);
  if (*status == 0)
    statusLed(LED_ON);
  else
    statusLed(LED_OFF);
}

/**@brief  display information.
  *@param  CERC discovery pairing call back
  *@retval None
  */
void printInfo(CERC_DscPairCallback_Type *param)
{
  switch (param->rf4cePrimitive) {
  
  case CERC_DSC_CONFIRM:
    printf("Status = %08X\r\n", param->DscPair.dscConfirm->Status);
    break;
  
  case CERC_PAIR_CONFIRM:
    printf("Status = %08X\r\n", param->DscPair.pairConfirm->Status);
    printf("PairingRef = %02X\r\n", param->DscPair.pairConfirm->PairingRef);
    printf("VendorId = %04X\r\n", param->DscPair.pairConfirm->RecVendorId);
    printf("VendorString = %s\r\n", param->DscPair.pairConfirm->RecVendorString);
    printf("AppCapabilities = %02X\r\n", param->DscPair.pairConfirm->RecAppCapabilities);
    if (param->DscPair.pairConfirm->RecAppCapabilities & 0x01)
      printf("UserString = %s \r\n", param->DscPair.pairConfirm->RecUserString);
    printf("DevTypeList = %02X \r\n", param->DscPair.pairConfirm->RecDevTypeList[0]);
    break;
  
  case CERC_AUTO_DSC_CONFIRM:    
    printf("Status = %08X\r\n", param->DscPair.autoDscConfirm->Status);
    break;
  
  case CERC_PAIR_INDICATION:
    printf("Status = %08X\r\n", param->DscPair.pairIndication->Status);
    printf("PairingRef = %02X\r\n", param->DscPair.pairIndication->ProvPairingRef);
    printf("VendorId = %04X\r\n", param->DscPair.pairIndication->OrgVendorId);
    printf("VendorString = %s\r\n", param->DscPair.pairIndication->OrgVendorString);
    printf("AppCapabilities = %02X\r\n", param->DscPair.pairIndication->OrgAppCapabilities);
    if (param->DscPair.pairIndication->OrgAppCapabilities & 0x01)
      printf("UserString = %s \r\n", param->DscPair.pairIndication->OrgUserString);
    printf("DevTypeList = %02X \r\n", param->DscPair.pairIndication->OrgDevTypeList[0]);
    printf("PANId = %04X \r\n", param->DscPair.pairIndication->SrcPANId);
    {
      int8_t i;
      printf("Address = ");
      for (i=7; i>=0; i--)
	printf("%02X", param->DscPair.pairIndication->SrcIEEEAddr[i]);
      printf("\r\n");
    }
    break;
  case CERC_RF4CE_COMM_STATUS_INDICATION:
    printf("Status = %08X\r\n", param->DscPair.commStatusIndication->Status);
    printf("PairingRef = %02X\r\n", param->DscPair.commStatusIndication->PairingRef);
    printf("PANId = %04X \r\n", param->DscPair.commStatusIndication->DstPANId);
    if (param->DscPair.commStatusIndication->DstAddrMode == 0)
      printf("Address = %04X\r\n", param->DscPair.commStatusIndication->DstAddr.ShortAddress);
    else {
      int8_t i;
      printf("Address = ");
      for (i=7; i>=0; i--)
	printf("%02X", param->DscPair.commStatusIndication->DstAddr.IEEEAddress[i]);
      printf("\r\n");
    }
    break;
  }
}

/**@brief  CERC discovery pairing call back.
  *@param  CERC discovery pair callback type
  *@retval boolean value
  */
bool CERC_DscPairCallback(CERC_DscPairCallback_Type *param)
{
  /* Start Tag */
  switch (param->rf4cePrimitive) {
  case CERC_DSC_CONFIRM:
    printf("<CERC_DSC_CONFIRM>\r\n");
    break;
  case CERC_PAIR_CONFIRM:
    printf("<CERC_PAIR_CONFIRM>\r\n");
    if ((pairRef == -1) && 
	(param->DscPair.pairConfirm->Status == SUCCESS))
      pairRef = param->DscPair.pairConfirm->PairingRef;
    break;
  case CERC_AUTO_DSC_CONFIRM:
    printf("<CERC_AUTO_DSC_CONFIRM>\r\n");
    break;
  case CERC_PAIR_INDICATION:
    printf("<CERC_PAIR_INDICATION>\r\n");
    break;
  case CERC_RF4CE_COMM_STATUS_INDICATION:
    printf("<CERC_RF4CE_COMM_STATUS_INDICATION>\r\n");
    if ((pairRef == -1) &&
	(param->DscPair.commStatusIndication->Status == SUCCESS)  && 
	(param->DscPair.commStatusIndication->PairingRef != 0xFF))
      pairRef = param->DscPair.commStatusIndication->PairingRef;
    break;
  }
  
  printInfo(param);

  /* END Tag */ 
  switch (param->rf4cePrimitive) {
  case CERC_DSC_CONFIRM:
    printf("<CERC_DSC_CONFIRM_END>\r\n");
    break;
  case CERC_PAIR_CONFIRM:
    printf("<CERC_PAIR_CONFIRM_END>\r\n");
    break;
  case CERC_AUTO_DSC_CONFIRM:
    printf("<CERC_AUTO_DSC_CONFIRM_END>\r\n");
    break;
  case CERC_PAIR_INDICATION:
    printf("<CERC_PAIR_INDICATION_END>\r\n");
    break;
  case CERC_RF4CE_COMM_STATUS_INDICATION:
    printf("<CERC_RF4CE_COMM_STATUS_INDICATION_END>\r\n");
    break;
  }

  if (nodeType == TARGET)
    statusLed(LED_ON);
  else
    statusLed(LED_OFF);


  /* The application shall decide if accept the pair request or not
   * In this example we accept always the pair request
   */
  if (param->rf4cePrimitive == CERC_PAIR_INDICATION)
    return TRUE;

  return FALSE;
}

/**@brief  CERC command call back.
  *@param  CERC command callback type
  *@retval None
  */
void CERC_CommandCallback(CERC_CommandCallback_Type *param)
{
  uint8_t i;

  if (param->rf4cePrimitive == CERC_DATA_INDICATION) {
    printf("<DATA_RECEIVED>\r\n");
    printf("Device = %d\r\n", param->dataCmd.dataIndication->PairingRef);
    printf("Payload = 0x");
    for (i = 0; i < param->dataCmd.dataIndication->nsduLength; i++)
      printf("%02X", param->dataCmd.dataIndication->nsdu[i]);
    printf("\r\n");
    printf("<DATA_RECEIVED_END>\r\n");
    if (nodeType == TARGET)
      irCmdInterpreter(param->dataCmd.dataIndication->nsdu);
  }
  if (param->rf4cePrimitive == CERC_DATA_CONFIRM) {
    printf("<DATA_CONFIRM>\r\n");
    printf("Status = %d\r\n", param->dataCmd.dataConfirm->Status);
    printf("PairingRef = %d\r\n", param->dataCmd.dataConfirm->PairingRef);
    printf("<DATA_CONFIRM_END>\r\n");
  }
}

/* Board button management */

/**@brief  button read.
  *@param  None
  *@retval button state
  */
uint8_t buttonRead(void)
{
  static uint32_t startTime=0, timeElapsed;
  static uint8_t buttonState=BUTTON_INIT;
  uint32_t state = STM_EVAL_PBGetState(BUTTON_KEY);

  switch (buttonState) {
  case BUTTON_INIT:
    if (state != 0) {
      if (TIME_ELAPSED(startTime) > WAIT_TIME_BUTTON_PRESS)
	buttonState = RELEASED;
    }
    break;
  case RELEASED:
    if (state == 0) { 
      buttonState = PRESSED;
      startTime = TIME_CurrentTime();
      timeElapsed = 0;
    }
    break;
  case PRESSED:
    if ((state == 0) && (TIME_ELAPSED(startTime) > WAIT_BOUNCE_TIME))
	buttonState = WAIT_RELEASE;
    if  (state != 0)
      buttonState = RELEASED;
    break;
  case WAIT_RELEASE:
    timeElapsed += TIME_ELAPSED(startTime);
    startTime = TIME_CurrentTime();
    if ((state == 0) && (timeElapsed >= WAIT_TIME_BUTTON_PRESS))
      buttonState = PRESSED_LONG;
    if ((state != 0) && (timeElapsed < WAIT_TIME_BUTTON_PRESS))
      buttonState = RELEASED_SHORT;
    break;
  case RELEASED_SHORT:
    buttonState = RELEASED;
    break;
  case PRESSED_LONG:
    buttonState = RELEASED;
    break;
  }

  return buttonState;
}

/**@brief  heart beat leds.
  *@param  None
  *@retval None
  */
void heartBeatLed(void)
{
  static uint32_t startTime=0;

  if (TIME_ELAPSED(startTime) >= HEART_BEAT_TIME) {
    startTime = TIME_CurrentTime();
    STM_EVAL_LEDToggle(LED3);
  }
}

/**@brief  get leds status.
  *@param  Status
  *@retval None
  */
void statusLed(uint8_t status)
{
  static uint32_t startTime=0;
  static bool blink=FALSE;
  static uint32_t blinkTime;

  if (status == LED_BLINKING) {
    blinkTime = BLINKING_TIME;
    blink = TRUE;
  }

  if (status == LED_OFF) {
    STM_EVAL_LEDOff(LED1);
    blink = FALSE;
  }
  if (status == LED_ON) {
    STM_EVAL_LEDOn(LED1);
    blink = FALSE;
  }
  if (status == LED_INIT) {
    blinkTime = HEART_BEAT_TIME;
    blink = TRUE;
  }
  if (blink) {
    if (TIME_ELAPSED(startTime) >= blinkTime) {
      startTime = TIME_CurrentTime();
      STM_EVAL_LEDToggle(LED1);
    }
  } 
}


/**
  *@brief  Retargets the C library printf function to the USART.
  *@param  None
  *@retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(EVAL_COM1, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_TC) == RESET)
  {}

  return ch;
}

#ifdef  USE_FULL_ASSERT

/**
  *@brief  Reports the name of the source file and the source line number
  *        where the assert_param error has occurred.
  *@param  file: pointer to the source file name
  *@param  line: assert_param error line source number
  *@retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
