/**
  ******************************************************************************
  * @file    cerc_rf4ce.h
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012
  * @brief   cerc_rf4ce profile header file 
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
 * @addtogroup cerc_rf4ce
 *
 * See cerc_rf4ce.h for source code.
 * @{
 */

/* Includes ------------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

#ifndef CERC_RF4CE_H
#define CERC_RF4CE_H

/**
  * @brief CERC command code: User control pressed
  */
#define CERC_USER_CONTROL_PRESSED       0x01

/**
  * @brief CERC command code: User control repeated
  */
#define CERC_USER_CONTROL_REPEATED      0x02

/**
  * @brief CERC command code: User control released
  */
#define CERC_USER_CONTROL_RELEASED      0x03

/**
  * @brief CERC command code: Discovery request
  */
#define CERC_COMMAND_DISCOVERY_REQUEST  0x04

/**
  * @brief CERC command code: Discovery response
  */
#define CERC_COMMAND_DISCOVERY_RESPONSE 0x05

/**
  * @brief CERC error code: Error code raised if the
  * application attempts to execute a command while 
  * the local device waits an answer from 
  * the remote device (implementation specific)
  */
#define CERC_INVALID_STATE 0xFC01

/**
  * @brief CERC error code: Error code raised 
  * on receipt of a successful confirmation 
  * of discovery from the network layer 
  * with more than one node descriptor
  * (implementation specific)
  */
#define CERC_MORE_ONE_NODE 0xFC02

/**
  * @brief CERC return code: Return code raised 
  * on receipt of a queued confirmation 
  * for a data message to sent
  * (implementation specific)
  */
#define CERC_DATA_QUEUED   0xFC03

/**
  * @brief CERC return code: Return code raised 
  * on receipt of an unsuccessful queued
  * for a data message to sent
  * (implementation specific)
  */
#define CERC_DATA_DISCARDED 0xFC04

/**
  * @brief CERC return code: Return code
  * on receipt of a successful auto discovery
  * confirmation but no pair indication arrived
  * from the controller
  * (implementation specific)
  */
#define CERC_NO_PAIR_INDICATION 0xFC05

/**
  * @brief CERC callback define: To signal
  * the user if a discovery confirm is
  * received from the device
  */
#define CERC_DSC_CONFIRM             0x01

/**
  * @brief CERC callback define: To signal
  * the user if a pair confirm is
  * received from the device
  */
#define CERC_PAIR_CONFIRM            0x02

/**
  * @brief CERC callback define: To signal
  * the user if a auto discovery confirm is
  * received from the device
  */
#define CERC_AUTO_DSC_CONFIRM        0x03

/**
  * @brief CERC callback define: To signal
  * the user if a pair indication is
  * received from the device
  */
#define CERC_PAIR_INDICATION         0x04

/**
  * @brief CERC callback define: To signal
  * the user if a comm status indication is
  * received from the device
  */
#define CERC_RF4CE_COMM_STATUS_INDICATION  0x05

/**
  * @brief CERC callback define: To signal
  * the user if a data confirm is
  * received from the device
  */
#define CERC_DATA_CONFIRM         0x06

/**
  * @brief CERC callback define: To signal
  * the user if a unpair indication is
  * received from the device
  */
#define CERC_UNPAIR_INDICATION         0x08

/**
  * @brief CERC callback define: To signal
  * the user if a unpair confirm is
  * received from the device
  */
#define CERC_UNPAIR_CONFIRM         0x09

/**
  * @brief CERC callback define: To signal
  * the user if a data indication is
  * received from the device
  */
#define CERC_DATA_INDICATION         0x07

/**
  * @brief CERC profile constant: 
  * The maximum time between
  * consecutive user control
  * repeated command frame
  * transmissions.
  * Default value 100ms
  */
#define aplcMaxKeyRepeatInterval 0x186A 

/**
  * @brief CERC profile constant:
  * The minimum number of key exchange 
  * transmissions used to generate 
  * a security key.
  */
#define aplcMinKeyExchangeTransferCount 0x03 //??? not defined in cerc-errata-specification file (my assumption)

/**
  * @brief (CERC profile attribute) The interval in ms
  * at which user
  * command repeat
  * frames will be
  * transmitted.
  */
#define aplKeyRepeatInterval_ID 0x80

/**
  * @brief (Default value)  The interval in ms
  * at which user
  * command repeat
  * frames will be
  * transmitted. Valid range 0 - aplcMaxKeyRepeatInterval
  */
#define aplKeyRepeatInterval_Default 0xC35 /* 50ms */

/**
  * @brief (CERC profile attribute) The duration that a
  * recipient of a user
  * control repeated
  * command frame
  * waits before
  * terminating a
  * repeated operation.
  */
#define aplKeyRepeatWaitTime_ID 0x81

/**
  * @brief (Default value) The duration that a
  * recipient of a user
  * control repeated
  * command frame
  * waits before
  * terminating a
  * repeated operation. Valid range >= aplcMaxKeyRepeatInterval
  */
#define aplKeyRepeatWaitTime_Default aplcMaxKeyRepeatInterval /* 100ms */

/**
  * @brief (CERC profile attribute)
  * The number of key exchange 
  * transmissions used to generate 
  * a security key.
  * Valid range aplcMinKeyExchangeTransferCount – 0xff
  */
#define aplKeyExchangeTransferCount_ID 0x82

/**
  * @brief (Default value) The number 
  * of key exchange transmissions 
  * used to generate a security key.
  * Valid range aplcMinKeyExchangeTransferCount – 0xff
  */
#define aplKeyExchangeTransferCount_Default aplcMinKeyExchangeTransferCount

/* Private macro -------------------------------------------------------------*/
/* Public variables ---------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/**
  * @name CERC Types
  *@{
  */

/**
  * @brief CERC_Init parameters
  */
typedef struct CERC_Init_S {
  /** To indicate if the NWK should be
    *  should be initialized using cold
    *  warm start
    */
  bool coldStart;

  /** The node capabilities as defined 
    *  by RF4CE standard
    */
  uint8_t nodeCap;

  /** The active period of a device in
    *  MAC symbols.
    */
  uint32_t activePeriod;

  /** The LQI threshold below which discovery
    *  requests will be rejected.
    */
  uint8_t discoveryLQIThreshold;

  /** The duty cycle of
    *  a device in MAC
    *  of 0x000000
    *  indicates the
    *  device is not
    *  using power
    *  saving.
    */
  uint32_t dutyCycle;
} CERC_Init_Type;

/**
  * @brief CERC_DscPair parameters
  */
typedef struct CERC_DscPair_S {  
  /** Flag to signal
    *  if the discovery and Pair
    *  procedure is executed
    *  between two TARGETs.
    *  If the flag is set the Target send
    *  a discovery and pair (like a Controller) 
    */
  bool pairTT;

  /** Device type supported
    *  by the node issuing 
    *  this primitive.
    */
  uint8_t devType;

  /** The device type to discover. 
    *  This value can be set to 0xff 
    *  to indicate a wildcard. 
    *  (Only for Discovery request)
    */
  uint8_t searchDevType;

  /** Number of Key
    *  exchange (target to controller)
    *  secured pairing
    */
  uint8_t numberOfKey;
} CERC_DscPair_Type;

/**
  * @brief CERC_DscPairCallback parameters
  */
typedef struct CERC_DscPairCallback_S {
  /** Which type of RF4CE
    *  primitive calls the
    *  callback
    */
  uint8_t rf4cePrimitive;
  
  union DscPair_U {
    /** Discovery confirm structure to pass to DscPairCallback */
    NLME_DISCOVERY_CONFIRM_Type *dscConfirm;
    /** Auto Discovery confirm structure to pass to DscPairCallback */
    NLME_AUTO_DISCOVERY_CONFIRM_Type *autoDscConfirm;
    /** Pair Confirm structure to pass to DscPairCallback */
    NLME_PAIR_CONFIRM_Type *pairConfirm;
    /** Pair Indication structure to pass to DscPairCallback */ 
    NLME_PAIR_INDICATION_Type *pairIndication;
    /** Comm Status Indication structure to pass to DscPairCallback */
    NLME_COMM_STATUS_INDICATION_Type *commStatusIndication;
  } DscPair;
} CERC_DscPairCallback_Type;

/**
  * @brief CERC_SendCommand parameters
  */
typedef struct CERC_SendCommand_S {
  /** Pairing table
    *  reference
    */
  uint8_t pairingRef;
  /** RC command code
    */
  uint8_t *commandCode;
  /** RC command length
    */
  uint8_t commandLength;
} CERC_SendCommand_Type;

/**
  * @brief CERC_CommandCallback parameter
  */
typedef struct CERC_CommandCallback_S {
  /** Which type of RF4CE
    *  primitive calls the
    *  callback
    */
  uint8_t rf4cePrimitive;

  union dataCmd_U {
    /** Data confirm structure to pass to CommandCallback */
    NLDE_DATA_CONFIRM_Type *dataConfirm;
    /** Data indication structure to pass to CommandCallback */
    NLDE_DATA_INDICATION_Type *dataIndication;
  } dataCmd;
}  CERC_CommandCallback_Type;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** 
  *@brief  The CERC_Init function initializes the NWK and
  *        it configures the NIB attibutes 
  *        according to CERC profile.
  *@param  param The structure containing the CERC_Init parameters
  *@retval The status of the CERC Init.
  */
uint32_t CERC_Init(CERC_Init_Type *param);

/** @brief The CERC_InitCallback
  *  is a callback to notify the application of the status  
  *  of the network initalization procedure 
  *
  *  @param status of the CERC_Init
  *
  *  @return  Nothing
  */
extern void CERC_InitCallback(uint32_t *status);

/**@brief  The CERC_DscPair function manages the case of 
  *        discovery/pairing procedure for Controller
  *        and Target
  *@param  param The structure containing the 
  *        CERC_DscPair_Type parameters
  *@retval The status of the CERC Discovery/Pair function call.
  */
uint32_t CERC_DscPair(CERC_DscPair_Type *param);

/**@brief  The CERC_DscPairCallback
  *        is a callback to notify the application of the status  
  *        of the discovery/pairing procedure 
  *@param  isPairConfirm Flag to signal the type of confirm
  *        to notify the application (0 Discovery - 1 Pair)
  *@param  param The structure containing the parameters
  *        returned from NLME_DISCOVERY_confirm() or NLME_PAIR_confirm()
  *@retval True if the application accepts the pair - False otherwise
  */
extern bool CERC_DscPairCallback(CERC_DscPairCallback_Type *param);

/**@brief  The CERC_Pair_response
  *        maps the RF4CE NLME_PAIR_response()
  *        primitive
  *@param  None    
  *@retval Status of the function call
  */
uint32_t CERC_PairRsp(void);

/**@brief  The CERC_SendCommand
  *        sends the CERC
  *        command frame
  *@param  Command frame param
  *@retval Status of the function call
  */
uint32_t CERC_SendCommand(CERC_SendCommand_Type *param);

/**@brief  This function sends a generic RF4CE network frame
  *@param  param the network frame parameter
  *@retval Status of the function call
  */
uint32_t CERC_SendNwkFrame(NLDE_DATA_REQUEST_Type *param);

/**@brief  The CERC_CommandCallback
  *        is a callback to notify the application of the status  
  *        of the data delivery 
  *@param  status Transmission status
  *@param  pairingRef Pairing table reference
  *@return None
  */
extern void CERC_CommandCallback(CERC_CommandCallback_Type *param);

/**@brief  The CERC_Tick
  *        A tick routine that should be called periodically in the application's main event loop to
  *        check events, advance state machine, invoke callbacks, manage power state, check sleep timer
  *@param  None
  *@return None
  */
void CERC_Tick(void);

/**@brief  The CERC_EnablePowerSave
  *        This function return TRUE if the device can enable the power save
  *        FALSE otherwise
  *@param  flag to signal if the board can check if go to power save
  *@retval TRUE if the device can enable power save, FALSE otherwise
  */
bool CERC_EnablePowerSave(bool enable);

#endif /* CERC_RF4CE_H */

/** @} // END addtogroup
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
