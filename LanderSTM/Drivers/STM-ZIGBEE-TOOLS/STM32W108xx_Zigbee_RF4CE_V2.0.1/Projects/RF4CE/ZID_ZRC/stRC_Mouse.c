/**
  ******************************************************************************
  * @file    stRCMouse.c 
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   stRCMouse program body
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

/** @addtogroup demos
 * ZID RF4CE mems mouse demo \see stMouse.c for documentation.
 *
 *@{
 */

/** @} */
/** \cond DOXYGEN_SHOULD_SKIP_THIS
 */

/* Includes ------------------------------------------------------------------*/
#include PLATFORM_HEADER
#include "board.h"
#include "hal.h"
#include "error.h"

#ifndef USE_MB951
#include "mbxxx_i2c_lis302dl.h"
#endif /* USE_MB951 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rf4ce.h"
#include "timer.h"
#include "zid_zrc.h"
#include <math.h>

/* Private typedef -----------------------------------------------------------*/

typedef struct statusLedS {
  uint8_t state;
  uint32_t blinkInterval;
} statusLed_Type;

typedef enum {
  UNCONFIG = -1,
  DEVICE   = 0,
  HOST     = 1
} NodeType;

typedef struct statusDeviceS {
  NodeType nodeType;
  int8_t pairRef;
} statusDevice_Type;

/* Private define ------------------------------------------------------------*/
#define HEART_BEAT_LED_TIME 500000 /* 0.5 sec */
#define CONFIGURATION_BLINK_LED_TIME 100000 /* 100 ms */
#define START_BLINK_LED_TIME 500000 /* 0.5 sec */
#define LED_OFF      0
#define LED_ON       1
#define LED_BLINKING 2

#define WAIT_TIME_BUTTON_PRESS 1500000
#define WAIT_BOUNCE_TIME 50000
#define RELEASED       0x0
#define PRESSED        0x1
#define WAIT_RELEASE   0x2
#define RELEASED_SHORT 0x3
#define PRESSED_LONG   0x4
#define BUTTON_INIT    0x5

#define STEADY_THRESHOLD 8

#define BUTTON_STATE_IDLE         0
#define BUTTON_STATE_DOWN         1
#define BUTTON_STATE_PRESSED      2
#define BUTTON_STATE_WAIT_RELEASE 3
#define BUTTON_STATE_CLICKED      4


#define BUTTON_LEFT_MASK         ((1 << 0) | (1 << 1))
#define BUTTON_RIGHT_MASK        ((1 << 2) | (1 << 3))

#define BUTTON_LEFT_BIT          0
#define BUTTON_RIGHT_BIT         1

#define NO_MOVEMENT_TIMEOUT (10000) /* in milliseconds */
#define X_THRESHOLD 5
#define Y_THRESHOLD 5

/* ZRC Buttons management */
#define BUTTON_RC_SWITCH_MASK   1 << 4
#define ZRC_VOLUME_UP_MASK      1 << 0
#define ZRC_VOLUME_DOWN_MASK    1 << 1
#define ZRC_CHANNEL_UP_MASK     1 << 3
#define ZRC_CHANNEL_DOWN_MASK   1 << 2


#define GUI_DEMO 1 

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static statusLed_Type statusLed;
static statusDevice_Type statusDev;
static bool buttonsRCenabled;

/* Private function prototypes -----------------------------------------------*/
extern int NVM_Erase(void);
static void heartBeatLed(void);
static void configureLed(statusLed_Type led);
static void statusLed_Tick(void);
static uint32_t cfgDevice(bool host, bool coldStart);
static void warmStart(void);
static int8_t firstValidPairingTableEntry (void);
static uint8_t configurationButtonStateMachine(void);
static void button_Tick(void);
static void erasePairingTable(void);
static void eraseNVM(void);
static void pairingDevice(void);
static void mouseConfiguration(void);
static void DeviceStart(void);
static void flashLed(uint8_t num);
static void appEnablePowerSave(void);


#ifndef USE_MB951
static uint8_t mouseButtonStateMachine(Button_TypeDef button, uint8_t state, uint32_t thisMillisecondTick, uint32_t *lastMillisecondsTick);
static boolean mouseSteady(int8_t x, int8_t y);
static void getMouseButtonsStatus(uint8_t *buttonsMask, uint8_t *buttonsStatus);
static void getPitchAndRoll (int8_t x, int8_t y, int8_t z, int16_t *Pitch_Angle, int16_t *Roll_Angle);
static bool processBoardInfo(void);
#endif /* USE_MB951 */

/* Public variables ----------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**@brief  heartbeat LEDs.
  *@param  None
  *@retval None
  */
static void heartBeatLed(void) {
  static uint32_t startHBTime=0;

  if (TIME_ELAPSED(startHBTime) >= HEART_BEAT_LED_TIME) {
    STM_EVAL_LEDToggle(LED3);
    startHBTime = TIME_CurrentTime();
  }
}

/**@brief  Configures leds.
  *@param  led type
  *@retval None
  */
static void configureLed(statusLed_Type led)
{
  memcpy(&statusLed, &led, sizeof(statusLed));
}

/**@brief  Gives the status of LEDs.
  *@param  None
  *@retval None
  */
static void statusLed_Tick(void)
{
  static uint32_t startTime = 0;

  if ((statusLed.state == LED_BLINKING) && (startTime == 0)) {
    startTime = TIME_CurrentTime();
  }

  if ((statusLed.state == LED_OFF) && !buttonsRCenabled) {
    STM_EVAL_LEDOff(LED1);
    startTime = 0;
  }

  if (statusLed.state == LED_ON) {
    STM_EVAL_LEDOn(LED1);
    startTime = 0;
  }

  if (statusLed.state == LED_BLINKING) {
    if (TIME_ELAPSED(startTime) >= statusLed.blinkInterval) {
      startTime = TIME_CurrentTime();
      STM_EVAL_LEDToggle(LED1);
    }
  } 
}

/**@brief  Configures the device.
  *@param  boolean value of the host
  *@param  boolean coldstart
  *@retval None
  */
static uint32_t cfgDevice(bool host, bool coldStart)
{
  uint32_t status;
  statusLed_Type led;

  led.state = LED_OFF;
  
  status = ZID_DeviceInit(host, coldStart, (ZID_APP_PROFILE|ZRC_APP_PROFILE));
  printf("ZID_DeviceInit ... status 0x%08lx\r\n", status);

  if (host && coldStart && (status == RF4CE_SAP_PENDING)) {
    led.state = LED_BLINKING;
    led.blinkInterval = START_BLINK_LED_TIME; 
  }
  configureLed(led);

  if (status == RF4CE_SAP_PENDING) {
    statusDev.pairRef = -1;
    if (host)
      statusDev.nodeType = HOST;
    else
      statusDev.nodeType = DEVICE;
  }

  return status;
}

/**@brief  warm start.
  *@param  None
  *@retval None
  */
static void warmStart(void)
{
  if (cfgDevice(DEVICE, 0) == RF4CE_SAP_PENDING) {
    statusDev.nodeType = DEVICE;
  } else {
    if (cfgDevice(HOST, 0) == RF4CE_SAP_PENDING) {
      statusDev.nodeType = HOST;
    }
    else {
      statusDev.nodeType = UNCONFIG;
    }
  }
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

/**@brief  Configures the state machine related to th button.
  *@param  None
  *@retval None
  */
static uint8_t configurationButtonStateMachine(void)
{
  static uint32_t startTime=0, timeElapsed;
  static uint8_t buttonState=BUTTON_INIT;
  static bool flagPL=FALSE;

  switch (buttonState) {
  case BUTTON_INIT:
    if (STM_EVAL_PBGetState(BUTTON_S1) != 0x00) {
      if (TIME_ELAPSED(startTime) > WAIT_TIME_BUTTON_PRESS)
	buttonState = RELEASED;
    }
    break;
  case RELEASED:
    if ((STM_EVAL_PBGetState(BUTTON_S1) == 0x00) && !flagPL) {
      buttonState = PRESSED;
      startTime = TIME_CurrentTime();
      timeElapsed = 0;
    } else {
      if (STM_EVAL_PBGetState(BUTTON_S1) != 0x00) 
	flagPL = FALSE;
    }
    break;
  case PRESSED:
    if ((STM_EVAL_PBGetState(BUTTON_S1) == 0x00) && (TIME_ELAPSED(startTime) > WAIT_BOUNCE_TIME)) {
      buttonState = WAIT_RELEASE;
      flagPL = FALSE;
    }
    if (STM_EVAL_PBGetState(BUTTON_S1) != 0x00)
      buttonState = RELEASED;
    break;
  case WAIT_RELEASE:
    timeElapsed += TIME_ELAPSED(startTime);
    startTime = TIME_CurrentTime();
    if ((STM_EVAL_PBGetState(BUTTON_S1) == 0x00) && (timeElapsed >= WAIT_TIME_BUTTON_PRESS)) {
      buttonState = PRESSED_LONG;
    }
    if ((STM_EVAL_PBGetState(BUTTON_S1) != 0x00) && (timeElapsed < WAIT_TIME_BUTTON_PRESS)) {
      buttonState = RELEASED_SHORT;
    }
    break;
  case RELEASED_SHORT:
    buttonState = RELEASED;
    break;
  case PRESSED_LONG:
    buttonState = RELEASED;
    flagPL = TRUE;
    break;
  }

  return buttonState;
}

/**@brief  gives button tick.
  *@param  None
  *@retval None
  */
static void button_Tick(void) 
{
  uint8_t buttonState;

  buttonState = configurationButtonStateMachine();

  switch (buttonState) {
  case PRESSED_LONG:
    if (statusDev.nodeType == HOST)
      erasePairingTable();
    break;
  case RELEASED_SHORT:
    if (statusDev.nodeType == UNCONFIG) {
#if !defined (USE_MB851_REVC) && !defined (USE_MB851_REVD) && !defined (USE_MB954_REVB) && !defined (USE_MB954_REVC) && !defined (USE_MB950) && !defined (USE_MB951)
	while (1) {
	  printf("Error this application requires a board with STM32F as PC interface\r\n");
	  halCommonDelayMilliseconds(1000);
	}
#endif /* USE_MB851_REVC or USE_MB851_REVD or USE_MB954_REVB or USE_MB954_REVC or USE_MB950 or USE_MB951 */
      cfgDevice(HOST, 1);
    } else {
      if ((statusDev.nodeType == HOST) ||
	  ((statusDev.nodeType == DEVICE) && (statusDev.pairRef == -1)))
	pairingDevice();
    }
    break;
  default:
    break;
  }
}

/**@brief  Erases NVM.
  *@param  None
  *@retval None
  */
static void eraseNVM(void)
{
  statusLed_Type led;

  NVM_Erase();
  statusDev.nodeType = UNCONFIG;
  printf("NVM erased, rebooting..\r\n");
  halReboot();
  led.state = LED_OFF;
  configureLed(led);
}

/**@brief  Erase pairing table.
  *@param  None
  *@retval None
  */
static void erasePairingTable(void)
{
  uint8_t i;
  PairingTableEntryType pairT;
  NLME_SET_Type param;
  statusLed_Type led;
 
  memset(&pairT, 0xff, sizeof(pairT));
  param.NIBAttribute = nwkPairingTable_ID;
  param.NIBAttributeValue = (uint8_t*)&pairT;

  for (i = 0; i < nwkcMaxPairingTableEntries; i++) {
    param.NIBAttributeIndex = i;
    NLME_Set(&param);
  }
  printf("ZID RF4CE..... Pairing table erased\r\n");
  statusDev.pairRef = -1;
  led.state = LED_OFF;
  configureLed(led);
}

/**@brief  allows the pairing mode.
  *@param  None
  *@retval None
  */
static void pairingDevice(void)
{
  uint32_t status;
  ZID_Pairing_Type param;
  statusLed_Type led;

  param.pairTT = 0;
  if (statusDev.nodeType == HOST) 
    param.devType = 2; /* TV */
  else
    param.devType = 1; /* RC */
  param.searchDevType = 0xff;

  status = ZID_Pairing(&param);
  printf("ZID_Pairing call... status 0x%08lx\r\n", status);
  
  if (status == RF4CE_SAP_PENDING) {
    led.state = LED_BLINKING;
    led.blinkInterval = CONFIGURATION_BLINK_LED_TIME;
    configureLed(led);
  }
}

/**@brief  Configures the mouse.
  *@param  None
  *@retval None
  */
static void mouseConfiguration(void)
{
  ATTR_Status status;
  deviceAttributeInfo_Type val;
  uint8_t listVal;


  cfgDevice(DEVICE, 1);

  val.id = aplReportRepeatInterval_ID;
  val.proxyAttr = FALSE;
  val.length = 1;
  listVal = 0;
  val.value = &listVal;
  status = ZID_SetDeviceAttr(&val);
  printf("ZID_SetDeviceAttr call... Repeat Interval status 0x%02x\r\n", status);

  val.id = aplHIDNumStdDescComps_ID;
  val.proxyAttr = FALSE;
  val.length = 1;
  listVal = 1; /* # device to be configured */
  val.value = &listVal;
  status = ZID_SetDeviceAttr(&val);
  printf("ZID_SetDeviceAttr call... Num Std Desc status 0x%02x\r\n", status);

  val.id = aplHIDStdDescCompsList_ID;
  val.proxyAttr = FALSE;
  val.length = 1;
  listVal = HID_MOUSE_RPTDESC;
  val.value = &listVal;
  status = ZID_SetDeviceAttr(&val);
  printf("ZID_SetDeviceAttr call... Mouse Report ID status 0x%02x\r\n", status);

  printf("ZID Device Configured....\r\n");
}

#ifndef USE_MB951
/**@brief  Checks if the mouse is steady or not.
  *@param  x position
  *@param  y position
  *@retval boolean value
  */
static boolean mouseSteady(int8_t x, int8_t y)
{
  static int8_t xWindowMin = 127, yWindowMin = 127, xWindowMax = -128, yWindowMax = -128; 
  boolean returnValue = FALSE;

  if (x < xWindowMin)
    xWindowMin = x;
  if (x > xWindowMax)
    xWindowMax = x;

  if (y < yWindowMin)
    yWindowMin = y;
  if (y > yWindowMax)
    yWindowMax = y;

  if (((xWindowMax - xWindowMin) <= X_THRESHOLD) && ((yWindowMax - yWindowMin) <= Y_THRESHOLD)) {
    returnValue = TRUE;
  } else {
    xWindowMin = 127;
    yWindowMin = 127; 
    xWindowMax = -128;
    yWindowMax = -128; 
  }

  return returnValue;
}

/**@brief  gives the Mouse button status.
  *@param  pointer to the button mask
  *@param  pointer to the button status
  *@retval None
  */
static void getMouseButtonsStatus(uint8_t *buttonsMask, uint8_t *buttonsStatus)
{
  static uint8_t buttonStatus[5] = {BUTTON_STATE_IDLE, BUTTON_STATE_IDLE, BUTTON_STATE_IDLE, BUTTON_STATE_IDLE, BUTTON_STATE_IDLE};
  static uint32_t buttonStatusTime[5] = {0,0,0,0,0};
  uint32_t thisMillisecondTick = halCommonGetInt32uMillisecondTick();
  static Button_TypeDef buttonList[5];
  static uint8_t firstTime = 1;
  uint8_t i;

  if (firstTime) {
    firstTime = 0;
    buttonList[0] = BUTTON_S1;
#ifdef USE_MB950
    buttonList[1] = BUTTON_S2;
    buttonList[2] = BUTTON_S3;
    buttonList[3] = BUTTON_S4;
    buttonList[4] = BUTTON_S5;
#endif /* USE_MB950 */
  }

  *buttonsMask = 0;
  *buttonsStatus = 0;

  for (i = 0; i < BUTTONn; i++) {
    buttonStatus[i] = mouseButtonStateMachine(buttonList[i], buttonStatus[i], thisMillisecondTick, &buttonStatusTime[i]);
    if (buttonStatus[i] == BUTTON_STATE_PRESSED) {
      /* Button i pressed */
      *buttonsMask |= (1 << i);
      *buttonsStatus |= (1 << i);
    }

    if (buttonsRCenabled && (buttonStatus[i] == BUTTON_STATE_WAIT_RELEASE) && (i != 4)) { //???????? Vedere la condizione i!=4 e metterla meglio
      /* Button i pressed */
      *buttonsMask |= (1 << i);
      *buttonsStatus |= (1 << i);
    }

    if (buttonStatus[i] == BUTTON_STATE_CLICKED) {
      /* Button i released */
      *buttonsMask |= (1 << i);
    }
  }
  return;
}

/**@brief  gets pithc and roll.
  *@param  x position
  *@param  y position
  *@param  z position
  *@param  angle of pitching
  *@param  angle of rolling
  *@retval None
  */
static void getPitchAndRoll (int8_t x, int8_t y, int8_t z, int16_t *Pitch_Angle, int16_t *Roll_Angle)
{
    int16_t Longitudinal_axis, Lateral_axis, Vertical_axis;
    double inclination_tg_squere;

    /*************************************************** 
        ACCELEROMETER AXIS ORIENTATION
	****************************************************

		-------------------------
                |           TV          |
		-------------------------

       
                   + Longitudinal_axis
                            ^
                            |
                            |
                          -----
                          | R |
                          | E |
                          | M |
        + Lateral_axis <--| O |
                          | T |
                          | E |
                          -----
                           (.) + Vertical_axis
        
    ***************************************************/

    /* Set next parameter accordingly to LIS302DL orientation */
    Longitudinal_axis = y;        /* Add the right sign (+/-) to match figure! */
    Lateral_axis = x;             /* Add the right sign (+/-) to match figure! */
    Vertical_axis = - z;     /* Add the right sign (+/-) to match figure! */       
    
    /* MOUSE UP DOWN HAVE TO BE PROPORTIONAL TO PITCH ANGLE */
    inclination_tg_squere = (int32_t)(Longitudinal_axis * Longitudinal_axis);
    inclination_tg_squere/= (int32_t)((Lateral_axis * Lateral_axis)+(Vertical_axis * Vertical_axis));
    inclination_tg_squere = sqrt(inclination_tg_squere);
    
    *Pitch_Angle = (int16_t) ((atan(inclination_tg_squere) * 360/ (2 * 3.14)) * ((Longitudinal_axis < 0) ? 1 : -1));
    
    /* MOUSE UP DOWN HAVE TO BE PROPORTIONAL TO ROLL ANGLE */
    inclination_tg_squere = (int32_t)(Lateral_axis * Lateral_axis);
    inclination_tg_squere/= (int32_t)((Vertical_axis * Vertical_axis)+(Longitudinal_axis * Longitudinal_axis));
    inclination_tg_squere = sqrt(inclination_tg_squere);
    
    *Roll_Angle = (int16_t) ((atan(inclination_tg_squere) * 360/ (2 * 3.14)) * ((Lateral_axis < 0) ? -1 : 1));
}

/**@brief  gives button state machine.
  *@param  button
  *@param  current state of the button
  *@param  current time
  *@param  pointer to the last  time tick
  *@retval unsigned value
  */
static uint8_t mouseButtonStateMachine(Button_TypeDef button, uint8_t state, uint32_t thisMillisecondTick, uint32_t *lastMillisecondsTick)
{
  uint8_t nextState = state;

  nextState = state;
  switch (state) {
  case BUTTON_STATE_IDLE:
    if (STM_EVAL_PBGetState(button) == 0x00) {
      nextState = BUTTON_STATE_DOWN;
      *lastMillisecondsTick = halCommonGetInt32uMillisecondTick();
    }
    break;
  case BUTTON_STATE_DOWN:
    if (STM_EVAL_PBGetState(button) == 0x00) {
      if ((thisMillisecondTick - *lastMillisecondsTick) > 50) {
	nextState = BUTTON_STATE_PRESSED;
      }
    } else {
      nextState = BUTTON_STATE_IDLE;
    }
    break;
  case BUTTON_STATE_PRESSED:
    nextState = BUTTON_STATE_WAIT_RELEASE;
    break;
  case BUTTON_STATE_WAIT_RELEASE:
    if (STM_EVAL_PBGetState(button) != 0x00) {
      nextState = BUTTON_STATE_CLICKED;
    }
    break;
  case BUTTON_STATE_CLICKED:
    nextState = BUTTON_STATE_IDLE;
  default:
    break;
  }

  return nextState;
}

/**@brief  gives the board info.
  *@param  None
  *@retval boolean value
  */
static bool processBoardInfo(void)
{
  static uint32_t lastMillisecondTick = 0, lastMouseMovemement = 0;
  uint32_t thisMillisecondTick;
  double gain = 1.0;
  int16_t Pitch_Angle, Roll_Angle;
  ReportData_Type param;
  uint8_t value[6];
  uint8_t buttonsStatus, buttonsMask;
  uint8_t buttonReport = 0;
  static uint8_t buttonTimeout = 0;
  static uint8_t lastButtonReport = 0;
  bool boardActive = FALSE;

  getMouseButtonsStatus(&buttonsMask, &buttonsStatus);

  if (buttonsMask) {
    if (buttonsMask & BUTTON_RC_SWITCH_MASK) {
      uint8_t pressed = (buttonsStatus & buttonsMask & BUTTON_RC_SWITCH_MASK) ? 1 : 0;
      if (pressed) {
	buttonsRCenabled = !buttonsRCenabled;
      }
      buttonsMask = 0;
      if (buttonsRCenabled) {
	STM_EVAL_LEDOn(LED1);
      } else {
	STM_EVAL_LEDOff(LED1);
      }
    }
  }

  if (buttonsMask && buttonsRCenabled) {
    UserControlPressed_Type cmdPress;
    UserControlReleased_Type cmdReleased;
    bool released = FALSE;
    uint8_t cmdCode=0;
    uint32_t statusZRC;

    boardActive = TRUE;

    if (buttonsMask & ZRC_VOLUME_UP_MASK) {
      cmdCode = 0x41;
      if (buttonsStatus & ZRC_VOLUME_UP_MASK) {
	released = FALSE;
      } else {
	released = TRUE;
      }
    }
    if (buttonsMask & ZRC_VOLUME_DOWN_MASK) {
      cmdCode = 0x42;
      if (buttonsStatus & ZRC_VOLUME_DOWN_MASK) {
	released = FALSE;
      } else {
	released = TRUE;
      }
    }
    if (buttonsMask & ZRC_CHANNEL_UP_MASK) {
      cmdCode = 0x30;
      if (buttonsStatus & ZRC_CHANNEL_UP_MASK) {
	released = FALSE;
      } else {
	released = TRUE;
      }
    }
    if (buttonsMask & ZRC_CHANNEL_DOWN_MASK) {
      cmdCode = 0x31;
      if (buttonsStatus & ZRC_CHANNEL_DOWN_MASK) {
	released = FALSE;
      } else {
	released = TRUE;
      }
    }
    if (released) {
      cmdReleased.pairRef = statusDev.pairRef;
      cmdReleased.commandCode = cmdCode;
      cmdReleased.txOptions = 0x01;      
      statusZRC = ZRC_UserControlReleased(&cmdReleased);
      if (statusZRC != RF4CE_SAP_PENDING) 
	printf("Error during send the user control released 0x%02x status = 0x%08lx\r\n", cmdCode, statusZRC);
    } else {
      cmdPress.pairRef = statusDev.pairRef;
      cmdPress.commandCode = cmdCode;
      cmdPress.txOptions = 0x11;
      cmdPress.payloadLength = 0;
      statusZRC = ZRC_UserControlPressed(&cmdPress);
      if (statusZRC != RF4CE_SAP_PENDING) 
	printf("Error during send the user control pressed 0x%02x status = 0x%08lx\r\n", cmdCode, statusZRC);
    }
  }

  if (buttonsMask && !buttonsRCenabled) {
    /* Some bits are valid */
    if (buttonsMask & BUTTON_LEFT_MASK) {
      uint8_t pressed = (buttonsStatus & buttonsMask & BUTTON_LEFT_MASK) ? 1 : 0;
      /* Left button activity (pressed or released) */
      buttonReport |= (pressed << BUTTON_LEFT_BIT);
    } else {
      buttonReport |= lastButtonReport & (1 << BUTTON_LEFT_BIT);
    }

    if (buttonsMask & BUTTON_RIGHT_MASK) {
      uint8_t pressed = (buttonsStatus & buttonsMask & BUTTON_RIGHT_MASK) ? 1 : 0;
      /* Right button activity (pressed or released) */
      buttonReport |= (pressed << BUTTON_RIGHT_BIT);
    } else {
      buttonReport |= lastButtonReport & (1 << BUTTON_RIGHT_BIT);
    }

    boardActive = TRUE;
    buttonTimeout = 10;
    param.pairRef = statusDev.pairRef;
    param.txOptions = TX_INTERRUPT_PIPE_UNICAST;
    param.reportPayloadLength = 0x06;
    value[0] = 0x5; //Rpt size
    value[1] = 0x01; //Rpt Type
    value[2] = 0x01; //Rpt Id
    value[3] = buttonReport;
    value[4] = 0;
    value[5] = 0;
    param.reportDataRecords = value;
    ZID_ReportData(&param);
    lastButtonReport = buttonReport;
  }

  thisMillisecondTick = halCommonGetInt32uMillisecondTick();
  if (((thisMillisecondTick - lastMillisecondTick) >= 9) && 
      (LIS302DL_GetStatus() & (STATUS_REG_ZYXDA)) && !buttonsRCenabled) {
    int8_t MOUSE_X, MOUSE_Y;
    LIS302DL_DataTypeDef axyz;

    STM_EVAL_LEDOff(LED1);

    if (buttonTimeout > 0) {
      buttonTimeout--;
      return TRUE;
    }

    LIS302DL_ReadACC(&axyz); 
    lastMillisecondTick = halCommonGetInt32uMillisecondTick();

    getPitchAndRoll ((int8_t) axyz.outx_l,
		     (int8_t) axyz.outy_l,
		     (int8_t) axyz.outz_l,
		     &Pitch_Angle,
		     &Roll_Angle);

    if (abs(Pitch_Angle) < STEADY_THRESHOLD)
      Pitch_Angle = 0;
    else 
      Pitch_Angle += (Pitch_Angle < 0 ? 1: -1) * STEADY_THRESHOLD;

    if (abs(Roll_Angle) < STEADY_THRESHOLD)
      Roll_Angle = 0;
    else 
      Roll_Angle += (Roll_Angle < 0 ? 1: -1) * STEADY_THRESHOLD;

    MOUSE_X = (int8_t) (Pitch_Angle * gain);
    MOUSE_Y = (int8_t) (Roll_Angle * gain);

    /* Keep mouse movement */
    if (!mouseSteady(MOUSE_X, MOUSE_Y)) {
      lastMouseMovemement = halCommonGetInt32uMillisecondTick();
    }

    if (((MOUSE_X != 0) || (MOUSE_Y != 0)) &&
	((halCommonGetInt32uMillisecondTick() - lastMouseMovemement) < NO_MOVEMENT_TIMEOUT)) {
      boardActive = TRUE;
      STM_EVAL_LEDToggle(LED1);
      param.pairRef = statusDev.pairRef;
      param.txOptions = TX_INTERRUPT_PIPE_UNICAST;
      param.reportPayloadLength = 0x06;
      value[0] = 0x5; //Rpt size
      value[1] = 0x01; //Rpt Type
      value[2] = 0x01; //Rpt Id
      value[3] = lastButtonReport;
      value[4] = MOUSE_X;
      value[5] = MOUSE_Y;
      param.reportDataRecords = value;
      ZID_ReportData(&param);
    }
  }
  return boardActive;
}
#endif /* USE_MB951 */

/**@brief  initialize the device.
  *@param  None
  *@retval None
  */
static void DeviceStart(void)
{
#ifndef USE_MB951
LIS302DL_InitTypeDef  LIS302DL_InitStruct;
#endif /* USE_MB951 */

  bool coldS = FALSE;
  uint32_t startTime;
  bool cmdReceived = FALSE;

  startTime = halCommonGetInt32uMillisecondTick();
  if (STM_EVAL_PBGetState(BUTTON_S1) == 0x00) {
    while (STM_EVAL_PBGetState(BUTTON_S1) == 0x00) {
      if ((halCommonGetInt32uMillisecondTick() - startTime) > 5000) {
	if (!cmdReceived) {
	  flashLed(2);
	  cmdReceived = TRUE;
	}
	coldS = FALSE;
      } else {
	coldS = TRUE;
      }
      ZID_Tick();
      heartBeatLed();
    }
    if (coldS) {
      mouseConfiguration();
      flashLed(2); 
    } else {
      eraseNVM();
    }
  } else {
    warmStart();
  }

  if (statusDev.nodeType == DEVICE)  {
#if defined (USE_MB951)
      printf("Error this application requires a board with MEMS device\r\n");
      halCommonDelayMilliseconds(1000);
      eraseNVM();
#else
      /* Set configuration of LIS302DL*/
      LIS302DL_InitStruct.Power_Mode = LIS302DL_LOWPOWERMODE_ACTIVE;
      LIS302DL_InitStruct.Output_DataRate = LIS302DL_DATARATE_100;
      LIS302DL_InitStruct.Axes_Enable = LIS302DL_X_ENABLE | LIS302DL_Y_ENABLE | LIS302DL_Z_ENABLE;
      LIS302DL_InitStruct.Full_Scale = LIS302DL_FULLSCALE_2_3;
      LIS302DL_InitStruct.Self_Test = LIS302DL_SELFTEST_NORMAL;
      LIS302DL_Init(&LIS302DL_InitStruct);
#endif
  }
}

/**@brief  flash led.
  *@param  None
  *@retval None
  */
static void flashLed(uint8_t num)
{
  uint32_t startTime;
  uint8_t i, times;

  times = num + (num%2);
  STM_EVAL_LEDOn(LED1);
  startTime = halCommonGetInt32uMillisecondTick();
  for (i=0; i<(times*2); i++) {
    while((halCommonGetInt32uMillisecondTick() - startTime) < 100) {
      ZID_Tick();
      heartBeatLed();
    }
    STM_EVAL_LEDToggle(LED1);
    startTime = halCommonGetInt32uMillisecondTick();  
  }
  STM_EVAL_LEDOff(LED1);
}

/**@brief  Enable power save.
  *@param  None
  *@retval None
  */
static void appEnablePowerSave(void)
{
  fflush(stdout);
  NWK_PowerDown();
  ATOMIC(
#ifndef USE_MB951
	 LIS302DL_LowpowerCmd(LIS302DL_LOWPOWERMODE_POWERDOWN);
#endif /* USE_MB951 */
	 halPowerDown();
	 halSleepWithOptions(SLEEPMODE_NOTIMER,
#ifdef USE_MB950 
                            BUTTON_S5_WAKE_SOURCE|
                            BUTTON_S4_WAKE_SOURCE|
                            BUTTON_S3_WAKE_SOURCE|
                            BUTTON_S2_WAKE_SOURCE|
#endif /* USE_MB950 */
                            BUTTON_S1_WAKE_SOURCE);
	 halPowerUp();
	 );
}

/* Public functions ----------------------------------------------------------*/

/**@brief  Main function.
  *@param  None
  *@retval None
  */
int main (void)
{
  uint8_t flagActive = TRUE;
  uint32_t idleStartTime;

#ifndef USE_MB951
LIS302DL_InitTypeDef  LIS302DL_InitStruct;
#endif /* USE_MB951 */

  halInit();
  uartInit();
  INTERRUPTS_ON();  

  printf("RF4CE ZID ZRC application profile version %s (lib version:%s)\r\n", ZID_ZRC_VERSION_STRING, RF4CE_VERSION_STRING);

  if (STM_EVAL_PBGetState(BUTTON_S1) != 0x00)
    halCommonDelayMilliseconds(1000); /* This delay is needed to allow the STM32F to start the USB */
 
  DeviceStart();

  buttonsRCenabled = FALSE;
  idleStartTime = halCommonGetInt32uMillisecondTick();

  while(1) {
    ZID_Tick();
    heartBeatLed();
    statusLed_Tick();
    button_Tick();

#ifndef USE_MB951
    if ((statusDev.nodeType == DEVICE) && (statusDev.pairRef != -1)) {
      flagActive = processBoardInfo();
    }
#endif /* USE_MB951 */
    
    if (!ZID_EnablePowerSave() || flagActive)
      idleStartTime = halCommonGetInt32uMillisecondTick();

    if ((halCommonGetInt32uMillisecondTick() - idleStartTime) >= 5000)  {
      appEnablePowerSave();
      NWK_PowerUp(TRUE);
      uartInit();
#ifndef USE_MB951
      /* Set configuration of LIS302DL*/
      LIS302DL_InitStruct.Power_Mode = LIS302DL_LOWPOWERMODE_ACTIVE;
      LIS302DL_InitStruct.Output_DataRate = LIS302DL_DATARATE_100;
      LIS302DL_InitStruct.Axes_Enable = LIS302DL_X_ENABLE | LIS302DL_Y_ENABLE | LIS302DL_Z_ENABLE;
      LIS302DL_InitStruct.Full_Scale = LIS302DL_FULLSCALE_2_3;
      LIS302DL_InitStruct.Self_Test = LIS302DL_SELFTEST_NORMAL;
      LIS302DL_Init(&LIS302DL_InitStruct);
#endif /* USE_MB951 */
      if (buttonsRCenabled)
        STM_EVAL_LEDOn(LED1);
      halCommonDelayMilliseconds(20);      
      idleStartTime = halCommonGetInt32uMillisecondTick();
    }
  }
}

/**@brief  Device initialization callback.
  *@param  status of the led
  *@retval None
  */
void ZID_DeviceInitCallback(uint32_t *status)
{
  statusLed_Type led;

  if (*status == SUCCESS) {
    statusDev.pairRef = firstValidPairingTableEntry();
    if (statusDev.nodeType == HOST) {
      led.state = LED_ON;
    } else {
      led.state = LED_OFF;
    }
  } else {
    led.state = LED_OFF;
    statusDev.nodeType = UNCONFIG;
  }
  configureLed(led);
  printf("ZID_DeviceInitCallback ... DeviceType 0x%02x status 0x%08lx\r\n", statusDev.nodeType, *status);

}

/**@brief  Pairing callback.
  *@param  pointer to the pairing callback type
  *@retval None
  */
void ZID_PairingCallback(ZID_PairingCallback_Type *param)
{
  statusLed_Type led;

  led.state = LED_OFF;

  switch(param->rf4cePrimitive) {
  case ZID_DSC_CONFIRM:
    printf("ZID_DSC_CONFIRM call... status 0x%08lx\r\n", param->Pairing.dscConfirm->Status);
    break;
  case ZID_PAIR_CONFIRM:
    printf("ZID_PAIR_CONFIRM call... status 0x%08lx\r\n", param->Pairing.pairConfirm->Status);
    if (statusDev.nodeType == DEVICE) {
      statusDev.pairRef = param->Pairing.pairConfirm->PairingRef;
    }
    break;
  case ZID_AUTO_DSC_CONFIRM:
    printf("ZID_AUTO_DSC_CONFIRM call... status 0x%08lx\r\n", param->Pairing.autoDscConfirm->Status);
    break;
  case ZID_PAIR_INDICATION:
    printf("ZID_PAIR_INDICATION call... status 0x%08lx\r\n", param->Pairing.pairIndication->Status);
    break;
  case ZID_COMM_STATUS_INDICATION:
    printf("ZID_COMM_STATUS_INDICATION call... status 0x%08lx\r\n", param->Pairing.commStatusIndication->Status);
    break;
  default:
    printf("Error!!!!!\r\n");
    break;
  }
  if (statusDev.nodeType == HOST) {
    led.state = LED_ON;
  } else {
    led.state = LED_OFF;
  }

  configureLed(led);
}

/**@brief  Network frame callback.
  *@param  pointer to the network frame callback type
  *@retval None
  */
void ZID_NwkFrameCallback(ZID_NwkFrameCallback_Type *param)
{
}

/**@brief  gives the command status.
  *@param  pointer to the command type
  *@retval None
  */
void ZID_CommandDeliveryStatus(CommandDeliveryStatus_Type *param)
{
  printf("DELIVERY STATUS... CommandID = 0x%02x Status = 0x%08lx PairRef = 0x%02x\r\n",
	 param->commandCode, param->status, param->pairingRef);
}

/**@brief  ZRC command notification.
  *@param  command code
  *@param  parameter  
  *@retval None
  */
void ZRC_CommandNotification(uint8_t cmdCode, void *param)
{
  switch(cmdCode) {
  case USER_CONTROL_PRESSED_CODE:
    {
      uint8_t i;
      UserControlPressed_Type *press;

      press = (UserControlPressed_Type *)param;

      printf("{ZRC_UserControlPressed API callback...\r\n");
      printf("    pairRef 0x%02x\r\n", press->pairRef);
      printf("    commandCode 0x%02x", press->commandCode);
      if (press->payloadLength > 0) {
	printf("\r\n    payloadLength 0x%02x\r\n", press->payloadLength);
	printf("    {payload: ");
	for (i=0; i<press->payloadLength; i++) {
	 printf("%02x", press->commandPayload);
	}
	printf("}}\r\n");
      } else  {
	printf("}\r\n");
      }
    }
    break;
  case USER_CONTROL_REPEATED_CODE:
    {
      uint8_t i;
      UserControlPressed_Type *press;

      press = (UserControlPressed_Type *)param;

      printf("{ZRC_UserControlRepeated API callback...\r\n");
      printf("    pairRef 0x%02x\r\n", press->pairRef);
      printf("    commandCode 0x%02x", press->commandCode);
      if (press->payloadLength > 0) {
	printf("\r\n    payloadLength 0x%02x\r\n", press->payloadLength);
	printf("    {payload: ");
	for (i=0; i<press->payloadLength; i++) {
	 printf("0x%02x", press->commandPayload[i]);
	}
	printf("}}\r\n");
      } else {
	printf("}\r\n");
      }
    }
    break;
  case USER_CONTROL_RELEASED_CODE:
    {
      UserControlReleased_Type *release;

      release = (UserControlReleased_Type *)param;

      printf("{ZRC_UserControlReleased API callback...\r\n");
      printf("    pairRef 0x%02x\r\n", release->pairRef);
      printf("    commandCode 0x%02x}\r\n", release->commandCode);
    }
    break;
  case COMMAND_DISCOVERY_RESPONSE_CODE:
    {
      CommandDiscoveryResponse_Type *dscRsp;
      uint8_t i;

      dscRsp = (CommandDiscoveryResponse_Type *)param;
      printf("{ZRC_CommandDiscoveryResponse API callback...\r\n");
      printf("    pairRef 0x%02x\r\n", dscRsp->pairRef);
      printf("    {Command Supported: ");
      for (i=0; i<32; i++) {
	printf("0x%02x",dscRsp->commandSupported[i]);
      }
      printf("}}\r\n");
    }
    break;
  }
}

/**
  * @brief  MEMS accelerometre management of the timeout situation.
  * @param  None.
  * @retval None.
  */
uint32_t LIS302DL_TIMEOUT_UserCallback(void)
{
  /* MEMS Accelerometer Timeout error occured */
  while (1)
  {   
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
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

