/**
  ******************************************************************************
  * @file    zid_zrc_constants.h
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012
  * @brief   ZID ZRC RF4CE constants header file
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

/**
  * @addtogroup zid_zrc_contsants ZID ZRC RF4CE CONSTANTS
  *
  * See zid_zrc_constants.h for source code.
  * @{
  */

#ifndef ZID_ZRC_CONSTANTS_H
#define ZID_ZRC_CONSTANTS_H

/**
  * @addtogroup RF4CE Application Profile Supported
  * @{
  */

/**
  * @brief ZRC profile ID
  */
#define ZRC_APP_PROFILE 0x01

/**
  *@brief ZID profile ID
  */
#define ZID_APP_PROFILE 0x02

/**
  * @}
  */  /*END RF4CE Application Profile Supported */

/**
  * @addtogroup ZID RF4CE Device Type
  * @{
  */

/**
  * @brief HID class device allows a compliant device to communicate with a host
  * via the standard HID class protocol by passing report descriptors containing
  * pertinent pieces of information which, when interpreted by the host, 
  * indicates the desired user interaction using the human interface device.
  */
#define HID_CLASS_DEVICE 0x00

/**
  * @brief HID adaptor serves as an interface between the RF4CE stack and the 
  * HID class driver. The HID adaptor is responsible for establishing RF4CE and 
  * HID connections between the host and remote RF4CE HID class devices. 
  * Once the RF4CE and HID connections are established, the HID adaptor 
  * processes RF4CE ZID profile commands arriving from the RF4CE stack and, 
  * as necessary, passing them to the HID class driver (possible via a wired 
  * transport) and vice versa.
  */
#define HID_ADAPTER 0x01

/**
  * @}
  */ /* END ZID RF4CE Device Type */

/**
  * @addtogroup GDP RF4CE constants used by the ZID profile
  * @{
  */

/**
  * @brief The length of time between completing the 
  * configuration phase of one profile and starting 
  * the next to allow the node to perform internal 
  * housekeeping tasks. 
  * Value is 100 ms. The time is in useconds.
  */
#define aplcConfigBlackoutTime 100000

/**
  * @brief The maximum amount of time a device waits after receiving a 
  * successful NLME-AUTO-DISCOVERY.confirm primitive for a pair indication 
  * to arrive from the pairing initiator.
  * Value 1.2 sec. The time is in useconds
  */
#define aplcMaxPairIndicationWaitTime 1200000

/**
  * @brief The maximum time a node shall leave its receiver on in order 
  * to receive either a response to a heartbeat command frame or data 
  * indicated via the data pending subfield of the frame control field 
  * of an incoming frame.
  * Value is 100 ms. The time is in useconds
  */
#define aplcMaxRxOnWaitTime 100000

/**
  * @brief The maximum time a device shall wait for a response command frame 
  * following a request command frame.
  * Value 200 msec. The time is in useconds
  */
#define aplcMaxResponseWaitTime 200000

/**
  * @brief The minimum value of the KeyExTransferCount parameter passed to 
  * the pair request primitive during the push button pairing procedure.
  */
#define aplcMinKeyExchangeTransferCount 3

/**
  * @}
  */  /* END GDP RF4CE constants used by the ZID profile */

/**
  * @addtogroup ZID RF4CE constants
  * @{
  */

/**
  * @brief The length of time the HID adaptor will repeat a report 
  * from a remote HID class device when its idle rate is non-zero, 
  * before a direct report command frame must be received from the 
  * remote HID class device.
  * Value is 1.5 s. The time is in useconds.
  */
#define aplcIdleRateGuardTime 1500000
/**
  * @brief The maximum time a HID class device shall take 
  * to construct a push attributes command frame and send 
  * it to the HID adaptor during its configuration state.
  * Value is 200 ms. The time is in useconds.
  */
#define aplcMaxConfigPrepTime 200000

/**
  * @brief The maximum time the HID adaptor shall wait 
  * to receive a push attributes command frame from a 
  * HID class device during its configuration state.
  * Value is 300 ms. The time is in useconds.
  */
#define aplcMaxConfigWaitTime 300000

/**
  * @brief The maximum number of non standard descriptors components
  * that can be supported on a HID class device. The HID adaptor 
  * shall be able to store this many non standard 
  * descriptors components for each paired HID class device.
  */
#define aplcMaxNonStdDescCompsPerHID 4

/**
  * @brief The maximum size of a single non standard descriptor 
  * component. The HID adaptor shall be able to store descriptor 
  * components of this size for each component stored on a device.
  * Value is 256 bytes
  */
#define aplcMaxNonStdDescCompSize 256

/**
  * @brief The maximum size of a non standard descriptor fragment 
  * that will fit in an over-the-air frame.
  */
#define aplcMaxNonStdDescFragmentSize 80

/**
  * @brief The maximum size of a NULL report. The HID adaptor shall
  * be able to store NULL reports of this size for each component 
  * stored on a device. Value is 16 bytes
  */
#define aplcMaxNullReportSize 16

/**
  * @brief The maximum time between consecutive report data transmissions.
  * Value is 100 ms.
  */
#define aplcMaxReportRepeatInterval 100

/**
  * @brief The maximum number of standard descriptor components
  * that can be supported on a HID class device. The HID adaptor shall 
  * be able to store this many standard descriptor components for each 
  * paired HID class device.
  */
#define aplcMaxStdDescCompsPerHID 12

/**
  * @brief Minimum window time permitted when using the interrupt 
  * pipe before a safe transmission is required.
  * Value is 50 ms.
  */
#define aplcMinIntPipeUnsafeTxWindowTime 50

/**
   * @}
   */  /* END ZID RF4CE constants */

/**
  * @addtogroup ZRC RF4CE constants
  * @{
  */

/**
  * @brief The maximum time between consecutive user control
  * repeated command frame transmissions.
  * Value is 100 ms. The time is in useconds.
  */
#define aplcMaxKeyRepeatInterval 100000

/**
  * @brief The maximum duration that the receiver is enabled 
  * on a controller after pairing to receive any command 
  * discovery request command frames.
  * Value is 200 ms. The time is in useconds.
  */
#define aplcMaxCmdDiscRxOnDuration 200000

/**
  * @brief The minimum amount of time a device must wait after 
  * a successful pairing attempt with a target before attempting
  * command discovery.
  * Value is 500 ms. The time is in useconds.
  */
#define aplcMinTargetBlackoutPeriod 500000

/**
  * @}
  */  /* END ZRC RF4CE constants */

/**
  * @addtogroup GDP RF4CE NIB attributes init values used by the ZID profile
  * @{
  */

/**
  * @brief ZID RF4CE active period of a device in MAC symbols (default value).
  */
#define nwkActivePeriod_InitValue 0xffffff /* Always ON  */

/**
  * @brief ZID RF4CE discovery LQI threshold below which discovery
  * requests will be rejected (default value).
  */
#define nwkDiscoveryLQIThreshold_InitValue 0xff

/**
  * @brief ZID RF4CE discovery repetition interval in MAC symbols, at
  * which discovery attempts are made on all channels (default value).
  */
#define nwkDiscoveryRepetitionInterval_InitValue 1000000 /* 1 sec 0x00f424  */

/**
  * @brief ZID RF4CE duty cycle in MAC symbols. A value of 0x000000
  * indicates the device is not using power saving (defualt value)
  */
#define nwkDutyCycle_InitValue 0x000000

/**
  * @brief ZID RF4CE maximum number of discovery attempts made at
  * the nwkDiscoveryRepetitionInterval rate.
  * Valid range 0x01 - 0xff
  */
#define nwkMaxDiscoveryRepetitions_InitValue 0x1e

/**
  * @brief ZID RF4CE maximum reported node descriptors that can be obtained
  * before reporting to the application.
  */
#define  nwkMaxReportedNodeDescriptors_InitValue 1

/**
  * @}
  */  /* END GDP RF4CE NIB attributes init values used by the ZID profile */

/**
  * @addtogroup ZID RF4CE TX Type
  * @{
  */

/**
  * @brief Control Pipe Unicast transmission enabled
  */
#define TX_CONTROL_PIPE_UNICAST   0x01

/**
  * @brief Control Pipe Broadcasr transmission enabled
  */
#define TX_CONTROL_PIPE_BROADCAST 0x02

/**
  * @brief Interrupt Pipe Unicast transmission enabled
  */
#define TX_INTERRUPT_PIPE_UNICAST 0x04

/**
  * @brief Transmission with security enabled
  */
#define TX_SECURITY_ENABLED       0x08

/**
  * @}
  */  /* END ZID RF4CE TX Type */

/**
  * @addtogroup ZRC RF4CE TX Type
  * @{
  */

/**
  * @brief Unicast - Ack - Multiple channel enabled
  */
#define ZRC_TX_ACK_UNICAST    0x01

/**
  * @brief Unicast - No Ack - Single channel enabled
  */
#define ZRC_TX_NO_ACK_UNICAST 0x02

/**
  * @brief Broadcasr
  */
#define ZRC_TX_BROADCAST      0x04

/**
  * @brief Transmission with security enabled
  */
#define ZRC_TX_SECURITY       0x08

/**
  * @brief Key repeat message enabled
  */
#define ZRC_TX_KEY_REPEAT     0x10

/**
  * @}
  */  /* END ZRC RF4CE TX Type */ 

/**
  * @addtogroup ZID RF4CE error codes
  * @{
  */

/**
  * @brief Error code on receipt of a successful 
  * confirmation of discovery from the network 
  * layer with more than one node descriptor
  */
#define ZID_MORE_ONE_NODE 0xFC01

/**
  * @brief Error code when a parameter in the 
  * primitive is either not supported or is 
  * out of the valid range.
  */
#define ZID_WRONG_PARAMETER 0xFC02

/**
  * @brief Error code when the HID Adaptor 
  * doesn't receive the pair indication
  */
#define ZID_NO_PAIR_INDICATION 0xFC03

/**
  * @}
  */  /* END ZID RF4CE error codes */

/**
  * @addtogroup GDP RF4CE command frame utilized by the ZID profile
  * @{
  */

/**
  * @brief Generic response command code
  */
#define GENERIC_RESPONSE_CODE         0x00

/**
  * @brief Configuration complete command code
  */
#define CONFIGURATION_COMPLETE_CODE   0x01

/**
  * @brief Heartbeat command code
  */
#define HEARTBEAT_CODE                0x02

/**
  * @brief Get attributes command code
  */
#define GET_ATTRIBUTES_CODE           0x03

/**
  * @brief Get attributes response command code
  */
#define GET_ATTRIBUTES_RESPONSE_CODE  0x04

/**
  * @brief Push attrbutes command code
  */
#define PUSH_ATTRIBUTES_CODE          0x05

/**
  * @}
  */  /* END GDP RF4CE command frame utilized by the ZID profile */

/**
  * @addtogroup ZID RF4CE command code 
  * @{
  */

/**
  * @brief Get report command code
  */
#define GET_REPORT_CODE 0x01

/**
  * @brief Report data command code
  */
#define REPORT_DATA_CODE 0x02

/**
  * @brief Set report command code
  */
#define SET_REPORT_CODE 0x03

/**
  * @}
  */  /* END ZID RF4CE command code */

/**
  * @addtogroup ZRC RF4CE command code
  * @{
  */

/**
  * @brief User control pressed command code
  */
#define USER_CONTROL_PRESSED_CODE 0x01

/**
  * @brief User control repeated command code
  */
#define USER_CONTROL_REPEATED_CODE 0x02

/**
  * @brief User control released command code
  */
#define USER_CONTROL_RELEASED_CODE 0x03

/**
  * @brief Command discovery request command code
  */
#define COMMAND_DISCOVERY_REQUEST_CODE 0x04

/**
  * @brief Command discovery response command code
  */
#define COMMAND_DISCOVERY_RESPONSE_CODE 0x05

/**
  * @}
  */  /* END ZRC RF4CE command code */

/**
  * @addtogroup Generic RF4CE command code utilized for ZID profile
  * @{
  */

/**
  * @brief ZID callback define to signal
  * the user if a discovery confirm is
  * received from the device
  */
#define ZID_DSC_CONFIRM             0x11

/**
  * @brief ZID callback define to signal
  * the user if a pair confirm is
  * received from the device
  */
#define ZID_PAIR_CONFIRM            0x12

/**
  * @brief ZID callback define to signal
  * the user if an auto discovery confirm is
  * received from the device
  */
#define ZID_AUTO_DSC_CONFIRM        0x13

/**
  * @brief ZID callback define to signal
  * the user if a pair indication is
  * received from the device
  */
#define ZID_PAIR_INDICATION         0x14

/**
  * @brief ZID callback define to signal
  * the user if a comm status indication is
  * received from the device
  */
#define ZID_COMM_STATUS_INDICATION  0x15

/**
  * @brief ZID callback define to signal
  * the user if a data confirm is
  * received from the device
  */
#define ZID_DATA_CONFIRM            0x16

/**
  * @brief ZID callback define to signal
  * the user if a data indication is
  * received from the device
  */
#define ZID_DATA_INDICATION         0x17

/**
  * @}
  */ /* END Generic RF4CE command code utilized for ZID profile */

#endif /* ZID_ZRC_CONTSTANTS_H */

/**
  * @}
  */ /* END addtogroup */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
