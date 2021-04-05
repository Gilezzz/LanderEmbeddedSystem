/**
  ******************************************************************************
  * @file    zid_zrc.h
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   ZID ZRC RF4CE header file
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
 * @addtogroup zid_zrc ZID ZRC RF4CE API
 *
 * See zid_zrc.h for source code.
 * @{
 */

#ifndef ZID_ZRC_H
#define ZID_ZRC_H

#include  "rf4ce.h"
#include  "nvm.h"
#include  "zid_zrc_constants.h"
#include  "zid_zrc_attributes.h"
#include  "zid_zrc_usb.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @name ZID ZRC RF4CE version defines
 *@{
 */


/**
 * @brief Version major number
 */
#define ZID_ZRC_VERSION_MAJOR 1

/**
 * @brief Version minor number
 */
#define ZID_ZRC_VERSION_MINOR 0

/**
 * @brief Version patch number
 */
#define ZID_ZRC_VERSION_PATCH 0

/**
 * @brief Version qualifier
 */
#define ZID_ZRC_VERSION_QUAL  ""

#define _QUOTEME(a) #a
#define QUOTEME(a) _QUOTEME(a)

/**
  * @brief Version string
  */
#define ZID_ZRC_VERSION_STRING QUOTEME(ZID_ZRC_VERSION_MAJOR) "." QUOTEME(ZID_ZRC_VERSION_MINOR) "." QUOTEME(ZID_ZRC_VERSION_PATCH) ZID_ZRC_VERSION_QUAL

//@} \\END ZID ZRC RF4CE version defines
#endif
/**
  * @name ZID ZRC RF4CE Types
  *@{
  */

/**
  * @brief ZID_Pairing parameters
  */
typedef struct ZID_Pairing_S {
  /** Flag to signal
    *  if the discovery and Pair
    *  procedure is executed
    *  between two TARGETs.
    *  If the flag is set the Target start a 
    *  a discovery and pair procedure (like a Controller) 
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
} ZID_Pairing_Type;

/**
  * @brief ZID_PairingCallback parameters
  */
typedef struct ZID_PairingCallback_S {
  /** Which type of RF4CE
    *  primitive calls the
    *  callback
    */
  uint8_t rf4cePrimitive;
  union Pairing_U {
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
  } Pairing;
} ZID_PairingCallback_Type;

/**
  * @brief ZID_NwkFrameCallback parameter
  */
typedef struct ZID_NwkFrameCallback_S {
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
}  ZID_NwkFrameCallback_Type;

/**
  * @brief ZID_NwkFrame parameter
  */
typedef struct ZID_NwkFrame_S {
  /** Reference into the pairing table which contains the information required to transmit the packet.
    *  This parameter is ignored if the TxOptions parameter specifies a broadcast transmission.
    */
  uint8_t pairingRef;
  /** Profile ID */
  uint8_t profileId;
  /** Transmission options for this NSDU. 
    *  - For bB0B (transmission mode):
    *    1 = broadcast transmission
    *    0 = unicast transmission
    *  - For bB1B (destination addressing mode):
    *    1 = use destination IEEE address
    *    0 = use destination network address
    *  - For bB2 B(acknowledgement mode):
    *    1 = acknowledged transmission
    *    0 = unacknowledged transmission
    *  - For bB3B (security mode):
    *    1 = transmit with security
    *    0 = transmit without security
    *  - For bB4B (channel agility mode):
    *    1 = use single channel operation
    *    0 = use multiple channel operation
    *  - For bB5B (channel normalization mode):
    *    1 = specify channel designator
    *    0 = do not specify channel designator
    *  - For bB6B (payload mode):
    *    1 = data is vendor specific
    *    0 = data is not vendor specific
    */
  uint8_t TxOptions;
  /** The length of the packet payload to send */
  uint8_t dataLen;
  /** The set of octets forming the paket payload to be transmitted by the nwk layer. */
  uint8_t *data;
} ZID_NwkFrame_Type;

typedef enum  {
  /**
   * @brief Generic Response Success
   */
  ZID_SUCCESS,
  /**
    * @brief Generic Response Unsupported request
    */
  ZID_UNSUPPORTED_REQUEST,
  /**
    * @brief Generic Response Invalid parameter
    */
  ZID_INVALID_PARAMETER,
  /**
    * @brief Generic Response Configuration failure
    */
  ZID_CONFIGURATION_FAILURE,
  /**
    * @brief Genric Response Invalid report ID
    */
  ZID_INVALID_REPORT_ID=0x40,
  /**
    * @brief Genric Response Missing report fragment
    */
  ZID_MISSING_FRAGMENT
} GRSP_Status;

/**
  * @brief Structure used for command delivery status
  */
typedef struct CommandDeliveryStatusS {
  /** ZID Command Code
    */
  uint8_t commandCode;
  /** Pairing Reference
    */
  uint8_t pairingRef;
  /** Delivery Status
    */
  uint32_t status;
} CommandDeliveryStatus_Type;

/**
  * @brief Structure used for generic response command
  */
typedef struct GenericResponseS {
  /** Pairing table reference 
    */
  uint8_t pairRef;
  /** Response code value
    */
  GRSP_Status rspCode;
  /** It specifies whether, at the completion 
    *  of the current command, the HID class device should 
    *  enable its receiver for a short period, giving another
    *  device the opportunity to send it a message.
    */
  bool dataPending;
} GenericResponse_Type;

/**
  * @brief Structure used for Configuration complete command
  */
typedef struct ConfigurationCompleteS {
  /** Pairing table reference 
    */
  uint8_t pairRef;
  /** Configuration status
    */
  GRSP_Status status;
} ConfigurationComplete_Type;

/**
  * @brief Structure used for heartbeat command
  */
typedef struct HeartBeatS {
  /** Pairing table reference 
    */
  uint8_t pairRef;
} HeartBeat_Type;

/**
  * @brief Structure used for get attributes command
  */
typedef struct GetAttributesS {
  /** Pairing table reference 
    */
  uint8_t pairRef;
  /** List of Attribute Identifiers
    */
  uint8_t *attrId;
  /** Length of the attribute list 
    */
  uint8_t attrListLength;
  /** It specifies whether, at the completion 
    *  of the current command, the HID class device should 
    *  enable its receiver for a short period, giving another
    *  device the opportunity to send it a message.
    */
  bool dataPending;
} GetAttributes_Type;

/**
  * @brief Structure used for get attributes response command
  */
typedef struct GetAttributesResponseS {
  /** Pairing table reference 
    */
  uint8_t pairRef;
  /** Length of the status records
    */
  uint8_t statusRecLength;
  /** List of attribute status record.
    *  Status record fields are:
    *  - Attribute Id (8 bit)
    *  - Attribute status (8 bit)
    *  - Attribute length (0/8 bit)
    *  - Attribute value (variable length)
    */
  uint8_t *statusRec;
  /** It specifies whether, at the completion 
    *  of the current command, the HID class device should 
    *  enable its receiver for a short period, giving another
    *  device the opportunity to send it a message.
    */
  bool dataPending;
} GetAttributesResponse_Type;

/**
  * @brief Structure used for push attributes command
  */
typedef struct PushAttributesS {
  /** Pairing table reference 
    */
  uint8_t pairRef;
  /** Length of the attributes record
    */
  uint8_t attributesRecLength;
  /** List of attribute record.
    *  Attribute record fields are:
    *  - Attribute Id (8 bit)
    *  - Attribute length (8 bit)
    *  - Attribute value (variable length)
    */
  uint8_t *attributesRec;
} PushAttributes_Type;

/**
  * @brief Structure used for get report command
  */
typedef struct GetReportS {
  /** Pairing table reference 
   */
  uint8_t pairRef;
  /** Report Type. Valid values are:
   *  - 0x01 Input
   *  - 0x02 Output
   *  - 0x03 Feature
   */
  uint8_t reportType;
  /** Report Identifier
   */
  uint8_t reportId;
  /** It specifies whether, at the completion 
   *  of the current command, the HID class device should 
   *  enable its receiver for a short period, giving another
   *  device the opportunity to send it a message.
   */
  bool dataPending;
} GetReport_Type;

/**
 * @brief Structure used for report data command
 */
typedef struct ReportDataS {
  /** Pairing table reference 
   */
  uint8_t pairRef;
   /** Transmission options. 
   *  - For bB0B (Control Pipe Unicast):
   *    1 = Control Pipe Unicast enabled
   *    0 = Control Pipe Unicast disabled
   *  - For bB1B (Control Pipe Broadcast):
   *    1 = Control Pipe Broadcast enabled
   *    0 = Control Pipe Broadcast disabled
   *  - For bB2B (Interrupt Pipe Unicast):
   *    1 = Interrupt Pipe Unicast enabled 
   *    0 = Interrupt Pipe Unicast disabled
   *  - For bB3B (Security):
   *    1 = Security enabled
   *    0 = Security disabled
   *  - For bB4B - bB7B (Reserved):
   *    0 = Reserved
   */
  uint8_t txOptions;
  /** Total report data payload
   */
  uint8_t reportPayloadLength;
  /** List of report data records.
   *  Report data record fields are:
   *  - Report size (8 bit)
   *  - Report Type (8 bit) (1 Input, 2 Output, 3 Feature)
   *  - Report Id (8 bit)
   *  - Report data (variable length)
   */
  uint8_t *reportDataRecords;
  /** It specifies whether, at the completion 
   *  of the current command, the HID class device should 
   *  enable its receiver for a short period, giving another
   *  device the opportunity to send it a message.
   */
  bool dataPending;
} ReportData_Type;

/**
 * @brief Structure used for set report command
 */
typedef struct SetReportS {
  /** Pairing table reference 
   */
  uint8_t pairRef;
  /** Report Type. Valid values are:
   *  - 0x01 Input
   *  - 0x02 Output
   *  - 0x03 Feature
   */
  uint8_t reportType;
  /** Report Identifier
   */
  uint8_t reportId;
  /** Report Data Length
   */
  uint8_t reportDataLen;
  /** Contents of the report
   */
  uint8_t *reportData;
  /** It specifies whether, at the completion 
   *  of the current command, the HID class device should 
   *  enable its receiver for a short period, giving another
   *  device the opportunity to send it a message.
   */
  bool dataPending;
} SetReport_Type;

/**
 * @brief Structure used for command supported discovery request
 */
typedef struct CommandDiscoveryRequestS {
  /** Pairing table reference 
   */
  uint8_t pairRef;
} CommandDiscoveryRequest_Type;

/**
 * @brief Structure used for command supported discovery request
 */
typedef struct CommandDiscoveryResponseS {
  /** Pairing table reference 
   */
  uint8_t pairRef;
  /** List of commandSupported (32 bytes)
   */
  uint8_t commandSupported[32];
} CommandDiscoveryResponse_Type;

/**
 * @brief Structure used for user control pressed frame
 */
typedef struct UserControlPressedS {
  /** Pairing table reference */
  uint8_t pairRef;
  /** RC Command code */
  uint8_t commandCode;
  /** RC command payload */
  uint8_t *commandPayload;
  /** RC command payload length */
  uint8_t payloadLength;
  /** Transmission options. 
   *  - For bB0B (Unicast-Ack-Muliple channel):
   *    1 = enabled
   *    0 = disabled
   *  - For bB1B (Unicast-No Ack-Single channel):):
   *    1 = enabled
   *    0 = disabled
   *  - For bB2B (Broadcast):
   *    1 = enabled 
   *    0 = disabled
   *  - For bB3B (Security):
   *    1 = enabled
   *    0 = disabled
   *  - For bB4B (Key repeat message):
   *    1 = enabled
   *    0 = disabled
   *  - For bB6B - bB7B (Reserved):
   *    0 = Reserved
   */
  uint8_t txOptions;
} UserControlPressed_Type;

/**
 * @brief Structure used for user control pressed frame
 */
typedef struct UserControlReleasedS {
  /** Pairing table reference */
  uint8_t pairRef;
  /** RC Command code */
  uint8_t commandCode;
  /** Transmission options. 
   *  - For bB0B (Unicast-Ack-Muliple channel):
   *    1 = enabled
   *    0 = disabled
   *  - For bB1B (Unicast-No Ack-Single channel):):
   *    1 = enabled
   *    0 = disabled
   *  - For bB2B (Broadcast):
   *    1 = enabled 
   *    0 = disabled
   *  - For bB3B (Security):
   *    1 = enabled
   *    0 = disabled
   *  - For bB4B - bB7B (Reserved):
   *    0 = Reserved
   */
  uint8_t txOptions;
} UserControlReleased_Type;

/**@brief  Device Init
  *@param  deviceType The HID device type. Valid device are:
  *         - HID_CLASS_DEVICE
  *         - HID_ADAPTOR
  *@param  coldStart This parameter allow the network layer to perform 
  *         a cold start (e.g. the first startup outside the factory).
  *
  *@retval The status of the device init attempt. Valid values are:
  *          - SUCCESS
  *          - RF4CE_BUSY
  *          - RF4CE_INVALID_PART
  *          - RF4CE_NVM_DATA_INVALID
  *          - RF4CE_SAP_PENDING
  */
uint32_t ZID_DeviceInit(uint8_t deviceType, bool coldStart, uint8_t profileSupported);

/**@brief  The ZID_DeviceInitCallback
  *        is a callback to notify the application of the status  
  *        of the network initalization procedure 
  *@param  status of the ZID_DeviceInit
  *@retval None
  */
extern void ZID_DeviceInitCallback(uint32_t *status);

/**@brief  The ZID_Pairing
  *        function manages the case of 
  *        discovery/pairing procedure between HID class device
  *        and HID adapter
  *@param  param The structure containing the 
  *        ZID_Pairing_Type parameters
  *@retval The status of the ZID pairing function call.
  */
uint32_t ZID_Pairing(ZID_Pairing_Type *param);

/**@brief  The ZID_PairingCallback
  *        is a callback to notify the application of the status  
  *        of the discovery/pairing procedure. If the procedure is ended with SUCCESS
  *        the callback returns all the information of the device paired, otherwise
  *        the callback returns the failure code and the SAP failed.
  *@param  param The structure containing the info of the device paired
  *        returned from NLME_DISCOVERY_confirm() or NLME_PAIR_confirm()
  *@retval None
  */
extern void ZID_PairingCallback(ZID_PairingCallback_Type *param);

/**@brief  This function sends a generic RF4CE network frame
  *@param  packet contains the network frame parameter
  *@retval Status of the function call
  */
uint32_t ZID_SendNwkFrame(ZID_NwkFrame_Type *packet);

/**@brief  The ZID_NwkFrameCallback
  *        is a callback to notify the application of the status  
  *        of the data delivery 
  *@param  param The structure containing the Nwk Frame callback parameters
  *@retval None
  */
extern void ZID_NwkFrameCallback(ZID_NwkFrameCallback_Type *param);

/**@brief  A tick routine that should be called periodically in the 
  *        application's main event loop to check events, advance state machine, 
  *        invoke callbacks, manage power state, check sleep timer
  *@retval None
  */
void ZID_Tick(void);

/**@brief  This callback is a delivery notification of the
  *        command execution requested
  *@param  param Structure used for command delivery status
  *@retval None
  */
extern void ZID_CommandDeliveryStatus(CommandDeliveryStatus_Type *param);

/**@brief  This function returns a flag to signal if the device 
  *        can go to power save or not
  *@param  None          
  *@retval TRUE device can go in power save, FALSE otherwise
  */
bool ZID_EnablePowerSave(void);

/**@brief  The generic response command frame allows a node to issue a response 
  *        to a command frame that does not have a corresponding response frame.
  *@param  param The structure containing the Generic Response parameters
  *@retval command result. Valid value are:
  *        - RF4CE_SAP_PENDING
  *        - RF4CE_BUSY
  */
uint32_t GDP_GenericResponse(GenericResponse_Type *param);

/**@brief  The configuration complete command frame allows a node 
  *        to indicate that the current profile configuration step is complete.
  *@param  param The structure containing the Configuration Complete parameters
  *@retval command result. Valid value are:
  *        - RF4CE_SAP_PENDING
  *        - RF4CE_BUSY
  *        - RF4CE_INVALID_CALL
  */
uint32_t GDP_ConfigurationComplete(ConfigurationComplete_Type *param); 

/**@brief  The heartbeat command frame allows a node, which sleeps most of the time,
  *        to check in with a target in case it wishes to send it a command.
  *@param  param The structure containing the Heart Beat parameters
  *@retval command result. Valid value are:
  *        - RF4CE_SAP_PENDING
  *        - RF4CE_BUSY
  *        - RF4CE_INVALID_CALL
  */
uint32_t GDP_HeartBeat(HeartBeat_Type *param);

/**@brief  The get attributes command frame allows a node to read one or more 
  *        profile attributes from a remote node.
  *@param  param The structure containing the Get Attributes parameters
  *@retval command result. Valid value are:
  *        - RF4CE_SAP_PENDING
  *        - RF4CE_BUSY
  *        - RF4CE_INVALID_CALL
  */
uint32_t GDP_GetAttributes(GetAttributes_Type *param); 

/**@brief  The get attributes response command frame allows a remote node to respond 
  *        to a get attribute command frame.
  *@param  param The structure containing the Get Attributes Response parameters
  *@retval command result. Valid value are:
  *        - RF4CE_SAP_PENDING
  *        - RF4CE_BUSY
  *        - RF4CE_INVALID_CALL
  */
uint32_t GDP_GetAttributesResponse(GetAttributesResponse_Type *param); 

/**@brief  The push attributes command frame allows a node to push attributes to 
  *        a remote node.
  *@param  param The structure containing the Push Attributes parameters
  *@retval command result. Valid value are:
  *        - RF4CE_SAP_PENDING
  *        - RF4CE_BUSY
  *        - RF4CE_INVALID_CALL
  */
uint32_t GDP_PushAttributes(PushAttributes_Type *param);

#ifdef ZID_FULL
/**@brief  The get report command frame allows the HID adaptor to request 
  *        a report from a HID class device.
  *@param  param The structure containing the Get Report parameters
  *@retval command result. Valid value are:
  *        - RF4CE_SAP_PENDING
  *        - RF4CE_BUSY
  *        - RF4CE_INVALID_CALL
  */
uint32_t ZID_GetReport(GetReport_Type *param); 
#endif

/**@brief  The report data command frame allows a remote node to respond 
  *        to a get report command frame or to send an unsolicited report.
  *@param  param The structure containing the Report Data parameters
  *@retval command result. Valid value are:
  *        - RF4CE_SAP_PENDING
  *        - RF4CE_BUSY
  */
uint32_t ZID_ReportData(ReportData_Type *param); 

/**@brief  The set report command frame allows a node to send 
  *        a report to a remote node.
  *@param  param The structure containing the Set Report parameters
  *@return command result. Valid value are:
  *        - RF4CE_SAP_PENDING
  *        - RF4CE_BUSY
  *        - RF4CE_INVALID_CALL
  */
uint32_t ZID_SetReport(SetReport_Type *param);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
/**@brief  The generic response command handler frame allows 
  *        a node to issue a response to a command frame that does 
  *        not have a corresponding response frame.
  *@param  param The structure containing the Generic Response parameters
  *@return Nothing
  */
extern void GDP_GenericResponseHandler(GenericResponse_Type *param);

#ifdef ZID_FULL
/**@brief  The configuration complete command handler frame allows a node 
  *        to indicate that the current profile configuration step is complete.
  *@param  param The structure containing the Configuration Complete parameters
  *@retval None
  */
extern void GDP_ConfigurationCompleteHandler(ConfigurationComplete_Type *param);

/**@brief  The heartbeat command handler frame allows the recipient the opportunity
  *        to send a command frame back to the originator. 
  *@param  param The structure containing the heart beat parameters
  *@retval None
  */
extern void GDP_HeartBeatHandler(HeartBeat_Type *param);
#endif

/**@brief  The get attributes command handler frame allows the recipient to process 
  *        each specified attribute identifier and respond with a get attributes response 
  *        command frame.
  *@param  param The structure containing the Get Attributes parameters
  *@retval None
  */
extern void GDP_GetAttributesHandler(GetAttributes_Type *param);

/**@brief  The get attributes response handler command frame allows the recipient 
  *        to notify the results of its original get attributes request.
  *@param  param The structure containing the Get Attributes Response parameters
  *@retval Nothing
  */
extern void GDP_GetAttributesResponseHandler(GetAttributesResponse_Type *param);

#ifdef ZID_FULL
/**@brief  The push attributes handler frame allows 
  *        a node to receive the attributes 
  *        from a remote node.
  *@param  param The structure containing the Push Attributes parameters
  *@retval Nothing
  */
extern void GDP_PushAttributesHandler(PushAttributes_Type *param);
#endif

/**@brief  The get report command handler frame allows a HID class device 
  *        to receive a report request from a HID adaptor.
  *@param  param The structure containing the Get Report parameters
  *@retval None
  */
extern void ZID_GetReportHandler(GetReport_Type *param);

/**@brief  The report data command handler frame allows a remote node 
  *        to receive the response to a get report command frame or 
  *        to receive an unsolicited report.
  *@param  param The structure containing the Report Data parameters
  *@retval None
  */
extern void ZID_ReportDataHandler(ReportData_Type *param);

/**@brief  The set report command handler frame allows a node to 
  *        receive a report from a remote node.
  *@param  param The structure containing the Set Report parameters
  *@retval None
  */
extern void ZID_SetReportHandler(SetReport_Type *param);
#endif

/**@brief  This function notifies the application the results of the GDP and ZID
  *        command received. Command notified are:
  *         - Generic Response
  *         - Heartbeat
  *         - Configuration Complete
  *         - Get Attributes Response
  *         - Get Report
  *         - Report Data
  *         - Set Report
  *@param  cmdCode GDP or ZID command code. For GDP command verify the sub-field bit
  *@param  param is the structure containing the status of the command notification
  *@retval None
  */
extern void ZID_CommandNotification(uint8_t cmdCode, void *param);

/**@brief  This function signals the HID class device
  *        that the data pending of the command received
  *        is set and it will enable the receiver at the 
  *        end of the command. If the device is a HID adaptor
  *        this function enables the heartbeat command notification
  *@param  value Is the value assumed from the data
  *        pending bit
  *@retval None
  */
void ZID_SetHDP(bool value);

/**@brief  This function enable/disable the radio sleep mechanism.
  *        If the function is called with the flag set to TRUE, when the HID 
  *        Class Device is in Idle state puts the radio in sleep mode.
  *        If the function is called with the flag set to FALSE, the HID Class
  *        Device doesn't put the radio in sleep.
  *@param  flagSleep if TRUE the flag signals the ZID state machine 
  *        if puts the HID Class Device in radio sleep during the IDLE state. 
  *        If FALSE the HID Class Device doesn't put the radio in sleep
  *@retval None
  */
void ZID_RadioSleepWhenIdle(bool flagSleep);

/**@brief  This function notifies the application the ZRC
  *        command received. Command notified are:
  *        - Command Discovery Response
  *        - User Control Pressed
  *        - User Control Repeated
  *        - User Control Released
  *@param  cmdCode ZRC command code.
  *@param  param is the structure containing the status of the command notification
  *@retval None
  */
extern void ZRC_CommandNotification(uint8_t cmdCode, void *param);

/**@brief  This function send the ZRC command discovery request to the remote node
  *@param  param the structure with the command discovery request parameters
  *@retval command result. Valid value are:
  *        - RF4CE_SAP_PENDING
  *        - RF4CE_BUSY
  */
uint32_t ZRC_CommandDiscoveryRequest(CommandDiscoveryRequest_Type *param);

/**@brief  This function send the ZRC command discovery response to the remote node
  *@param  param the structure with the command discovery response parameters
  *@retval command result. Valid value are:
  *        - RF4CE_SAP_PENDING
  *        - RF4CE_BUSY
  */
uint32_t ZRC_CommandDiscoveryResponse(CommandDiscoveryResponse_Type *param);

/**@brief  This function allows a node to request a remote node performs 
  *        the specified RC (HDMI CEC) command, using specified tx options.
  *@param  param the structure with the user control pressed parameters
  *@retval command result. Valid value are:
  *        - RF4CE_SAP_PENDING
  *        - RF4CE_BUSY
  */ 
uint32_t ZRC_UserControlPressed(UserControlPressed_Type *param);

/**@brief  This function communicates that the specified RC (HDMI CEC)
  *        command is released, using specified tx options. This command is optional,
  *        the application can decide if send it or not.
  *@param  param the structure with the user control released parameters
  *@retval command result. Valid value are:
  *        - RF4CE_SAP_PENDING
  *        - RF4CE_BUSY
  */ 
uint32_t ZRC_UserControlReleased(UserControlReleased_Type *param);

#endif /* ZID_ZRC_H */

/** @} // END addtogroup
  */
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
