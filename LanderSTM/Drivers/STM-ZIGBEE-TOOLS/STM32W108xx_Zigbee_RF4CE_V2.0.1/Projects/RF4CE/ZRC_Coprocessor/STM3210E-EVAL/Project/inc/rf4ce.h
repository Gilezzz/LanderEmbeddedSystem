/**
  ******************************************************************************
  * @file    rf4ce.h
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   rf4ce header file
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
  * @addtogroup rf4ce
  *
  * See rf4ce.h for source code.
  * @{
  */

#ifndef RF4CE_H
#define RF4CE_H

/* Includes ------------------------------------------------------------------*/
#include  "rf4ce_type.h"
#include  "nib.h"

/* Private define ------------------------------------------------------------*/ 

/**
  * @name RF4CE version defines
  *@{
  */


/**
  * @brief Version major number
  */
#define RF4CE_VERSION_MAJOR 1

/**
  * @brief Version minor number
  */
#define RF4CE_VERSION_MINOR 0

/**
  * @brief Version patch number
  */
#define RF4CE_VERSION_PATCH 3

/**
  * @brief Version qualifier
  */
#define RF4CE_VERSION_QUAL  ""

#define _QUOTEME(a) #a
#define QUOTEME(a) _QUOTEME(a)

/**
  * @brief Version string
  */
#define RF4CE_VERSION_STRING QUOTEME(RF4CE_VERSION_MAJOR) "." QUOTEME(RF4CE_VERSION_MINOR) "." QUOTEME(RF4CE_VERSION_PATCH) RF4CE_VERSION_QUAL

/* @} \\END RF4CE version defines */



/**
  * @name NWK enumerations description
  *@{
  */


/**
  * @brief The requested operation was completed successfully.
  */
#define SUCCESS 0x00

/**
  * @brief A pairing link cannot be established since the
  * originator node has reached its maximum
  * number of entries in its pairing table.
  */
#define NO_ORG_CAPACITY 0xB0

/**
  * @brief A pairing link cannot be established since the
  * recipient node has reached its maximum
  * number of entries in its pairing table.
  */
#define NO_REC_CAPACITY 0xB1

/**
  * @brief A pairing table entry could not be found that corresponds to the supplied pairing
  * reference.
  */
#define NO_PAIRING 0xB2

/**
  * @brief A response frame was not received within nwkResponseWaitTime.
  */
#define NO_RESPONSE 0xB3 

/**
  * @brief A pairing request was denied by the recipient node or an attempt to
  * update a security link key was not possible due to one or more nodes not supporting security.
  */
#define NOT_PERMITTED 0xB4

/**
  * @brief A duplicate pairing table entry was detected following the receipt of a pairing request
  * command frame.
  */
#define DUPLICATE_PAIRING 0xB5

/**
  * @brief The frame counter has reached its maximum value.
  */
#define FRAME_COUNTER_EXPIRED 0xB6

/**
  * @brief Too many unique matched discovery request or valid response command frames were
  * received than requested.
  */
#define DISCOVERY_ERROR 0xB7

/**
  * @brief No discovery request or response command frames were received during discovery.
  */
#define DISCOVERY_TIMEOUT 0xB8

/**
  * @brief The security link key exchange or recovery procedure did not complete within the
  * required time.
  */
#define SECURITY_TIMEOUT 0xB9

/**
  * @brief A security link key was not successfully established between both ends of a pairing
  * link.
  */
#define SECURITY_FAILURE 0xBA

/**
  * @brief A parameter in the primitive is either not
  * supported or is out of the valid range.
  */
#define INVALID_PARAMETER  0xE8

/**
  * @brief A SET/GET request was issued with the identifier of a NIB attribute that is not
  * supported.
  */
#define UNSUPPORTED_ATTRIBUTE  0xF4

/**
  * @brief An attempt to write to a NIB attribute that is in a table failed because the specified table
  * index was out of range.
  */
#define INVALID_INDEX  0xF9

/* @} \\END NWK enumerations */

/**
  * @name NWK command frame identifier
  *@{
  */


/**
  * @brief Discovery request
  */
#define RF4CE_DISCOVERY_REQUEST 0x01

/**
  * @brief Discovery response
  */
#define RF4CE_DISCOVERY_RESPONSE 0x02

/**
  * @brief Pair request
  */
#define RF4CE_PAIR_REQUEST 0x03

/**
  * @brief Pair response
  */
#define RF4CE_PAIR_RESPONSE 0x04

/**
  * @brief Unpair request
  */
#define RF4CE_UNPAIR_REQUEST 0x05

/**
  * @brief Key seed
  */
#define RF4CE_KEY_SEED 0x06

/**
  * @brief Ping request
  */
#define RF4CE_PING_REQUEST 0x07

/**
  * @brief Ping response
  */
#define RF4CE_PING_RESPONSE 0x08

//@} \\END NWK command frame identifier

/**
  * @name Implementation specific status
  *@{
  */

/**
  * @brief The request is pending and the status will be communicated through a callback
  */
#define RF4CE_SAP_PENDING 0x100

/**
  * @brief An internal error has occurred
  */
#define RF4CE_INTERNAL_ERROR 0x101
/**
  * @brief The NVM data is not valid and a warm startup cannot be performed
  */
#define RF4CE_NVM_DATA_INVALID 0x102
/**
  * @brief The RF4CE is still completing a previous operation it cannot perform the requested operation
  */
#define RF4CE_BUSY 0x103

/**
  * @brief The RF4CE stack is not allowed to run on the part
  */
#define RF4CE_INVALID_PART 0x104

/* @} \\END Implementation specific status */

/**
  * @name Generic Types
  *@{
  */

#ifndef NULL
/**
  * @brief The null pointer.
  */
#define NULL ((void *)0)
#endif

//@} \\END Generic Types

/**
  * @brief This bit indicate that the network layer is performing some operation
  */
#define  RF4CE_STATE_BUSY               (1 << 0)
/**
  * @brief This bit indicate that the NVM cache need to be flushed in NVM
  */
#define  RF4CE_STATE_NEED_CACHE_FLUSH   (1 << 1)
  

#define MAX_DEV_TYPE_LIST_SIZE    (1 << 2)
#define MAX_PROFILE_ID_LIST_SIZE  (1 << 3)

#define RF4CE_DEFAULT_TX_POWER 0
#define RF4CE_QUITE_TX_POWER   nwkcMaxSecCmdTxPower

#define RF4CE_DATA_REQ_TX_MODE                0x01
#define RF4CE_DATA_REQ_DEST_MODE              0x02
#define RF4CE_DATA_REQ_ACK_MODE               0x04
#define RF4CE_DATA_REQ_SEC_MODE               0x08
#define RF4CE_DATA_REQ_CHANNEL_AGILITY_MODE   0x10
#define RF4CE_DATA_REQ_CHANNEL_NORM_MODE      0x20
#define RF4CE_DATA_REQ_CHANNEL_PAYLOAD_MODE   0x40

#define RF4CE_USER_STRING_SIZE                16
#define RF4CE_VENDOR_STRING_SIZE              8

/* Private typedef -----------------------------------------------------------*/

/**
  * @name SAP Types
  *@{
  */

/**
  * @brief NLME_PAIR_request parameters
  */
typedef struct NLME_PAIR_REQUEST_S {
  /** The logical channel of the device with
   *  which to pair.
   */
  uint8_t LogicalChannel;
  /** The PAN identifier of the device with
   *  which to pair.
   */
  uint16_t DstPANId;
  /** The IEEE address of the device with
   *  which to pair.
   */
  IEEEAddr DstIEEEAddr;
  /** Node capabilities (8 bit mask)
   */
  uint8_t OrgAppCapabilities;
  /** List of device types supported by the
   *  node issuing this primitive (actual length is in OrgAppCapabilities).
   */
  uint8_t OrgDevTypeList[MAX_DEV_TYPE_LIST_SIZE];
  /** List of profile identifiers supported
   *  by the node issuing this primitive (actual length is in OrgAppCapabilities).
   */
  uint8_t OrgProfileIdList[MAX_PROFILE_ID_LIST_SIZE];
  /** The number of transfers the target should
    *  use to exchange the link key with the
    *  pairing originator.
    */
  uint8_t KeyExTransferCount;
} NLME_PAIR_REQUEST_Type;

/**
 * @brief NLME_PAIR_confirm parameters
 */
typedef struct NLME_PAIR_CONFIRM_S {
  /** The status of the pair  attempt.
   */
  uint32_t Status;

  /** The pairing table reference for this pairing link. If the Status parameter
   *  is not equal to SUCCESS, this value will be equal to 0xff.
   */
  uint8_t PairingRef;

  /** The vendor identifier of the originator of the pair response.
   */
  uint16_t RecVendorId;

  /** The vendor string of  the originator of the pair response.
   */
  uint8_t RecVendorString[RF4CE_VENDOR_STRING_SIZE];

  /** The application capabilities of the originator of the pair response.
   */
  uint8_t RecAppCapabilities;

  /** The user defined identification string of the originator of the pair response.
   */
  uint8_t RecUserString[RF4CE_USER_STRING_SIZE];

  /** List of device types supported by the originator of the pair response (actual length is in RecAppCapabilities).
   */
  uint8_t RecDevTypeList[MAX_DEV_TYPE_LIST_SIZE];

  /** List of profile identifiers supported
   *   by the
   *  originator of the
   *  pair response (actual length is in RecAppCapabilities).
   */
  uint8_t RecProfileIdList[MAX_PROFILE_ID_LIST_SIZE];
} NLME_PAIR_CONFIRM_Type;

/**
 * @brief NLME_UNPAIR_confirm parameters
 */
typedef struct NLME_UNPAIR_CONFIRM_S {
  /** he status of the unpair attempt. Valid values are:
   *  - SUCCESS
   *  - a status value from the MCPS-DATA.confirm primitive.
   */
  uint32_t Status;

  /** The pairing table reference for this pairing link.
   */
  uint8_t PairingRef;

} NLME_UNPAIR_CONFIRM_Type;

/**
 * @brief NLME_Data_request parameter
 */
typedef struct NLDE_DATA_REQUESTS {
  /** Reference into the pairing table which contains the information required to transmit the NSDU.
   *  This parameter is ignored if the TxOptions parameter specifies a broadcast transmission.
   */
  uint8_t PairingRef;
  /** The identifier of the profile indicating the format of the transmitted data. */
  uint8_t ProfileId;
  /** If the TxOptions parameter specifies that the data is vendor specific, this parameter
   *  specifies the vendor identifier. If this parameter is equal to 0x0000, the vendor
   *  identifier shall be set to nwkcVendorIdentifier. If the TxOptions parameter specifies that
   *  the data is not vendor specific this parameter is ignored.
   */
  uint16_t VendorId;
  /** The number of octets contained in the NSDU to be transmitted by the NLDE. */
  uint8_t nsduLength;
  /** The set of octets forming the NSDU to be transmitted by the NLDE. */
  uint8_t *nsdu;
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
} NLDE_DATA_REQUEST_Type; 

/**
 * @brief NLDE_DATA_confirm parameters
 */
typedef struct NLDE_DATA_CONFIRM_S {
  /** The status of the NSDU transmission. Valid values are:
   * - SUCCESS
   * - INVALID_PARAMETER
   * - NO_PAIRING
   * - FRAME_COUNTER_EXPIRED
   * - any status value from the MCPS-DATA.confirm
   */
  uint32_t Status;

  /** The pairing table reference for the NSDU being confirmed.
   */
  uint8_t PairingRef;

} NLDE_DATA_CONFIRM_Type;

/**
 * @brief NLME_DATA_indication parameters
 */
typedef struct NLDE_DATA_INDICATION_S {
  /** Reference into the pairing table which matched the information 
   *                   contained in the received NPDU.
   */
  uint8_t PairingRef;
  /** The identifier of the profile indicating the format of the received data */
  uint8_t ProfileId; 
  /** If the RxFlags parameter specifies that the data is vendor specific, 
   *  this parameter specifies the vendor identifier. If the RxFlags parameter 
   *  specifies that the data is not vendor specific this parameter is ignored.
   */
  uint16_t VendorId;
  /** The number of octets contained in the NSDU received by the NLDE */
  uint8_t nsduLength; 
  /** The set of octets forming the NSDU received by the NLDE. */
  uint8_t *nsdu;
  /** LQI value measured during the reception of the NPDU. 
   *  Lower values represent lower LQI.
   */
  uint8_t RxLinkQuality;
  /** Reception indication flags for this NSDU. 
   *  - For bB0B (reception mode):
   *    1 = received as broadcast
   *    0 = received as unicast
   *  - For bB1B (security mode):
   *    1 = received with security
   *    0 = received without security
   *  - For bB2B (payload mode):
   *    1 = data is vendor specific
   *    0 = data is not vendor specific
   */
  uint8_t RxFlags;
} NLDE_DATA_INDICATION_Type;


/**
 * @brief Node Descriptor for discovery
 */
typedef struct NodeDescS
{
  /** Status of the discovery request */
  uint8_t Status;
  /** Logical channel of the responding device */
  uint8_t LogicalChannel;
  /** PAN identifier of the responding device */
  uint16_t PANId;
  /** IEEE address of the responding node */  
  IEEEAddr longAddr;
  /** Capabilities of the responding device */
  uint8_t NodeCapabilities;
  /** Vendor identifier of the responding node */
  uint16_t VendorId;
  /** Vendor string of the responding node */
  uint8_t VendorString[RF4CE_VENDOR_STRING_SIZE];
  /** Application capabilities of the responding node */
  uint8_t AppCapabilities;
  /** User defined identification string of the responding node*/
  uint8_t UserString[RF4CE_USER_STRING_SIZE];
  /** List of device types supported by the
   *  responding node (actual length is in AppCapabilities).
   */
  uint8_t DevTypeList[MAX_DEV_TYPE_LIST_SIZE];
  /** List of profile identifiers supported
   *  by the responding node (actual length is in AppCapabilities).
   */
  uint8_t ProfileIdList[MAX_PROFILE_ID_LIST_SIZE];
  /** LQI of the discovery request command frame reported bye
   *  the responding node
   */
  uint8_t DiscReqLQI;
} NodeDescType;

/**
 * @brief NLME_AUTO_DISCOVERY_confirm parameters
 */
typedef struct NLME_AUTO_DISCOVERY_CONFIRM_S {
  /** The status of the auto discovery response mode. Valid values are:
   * - SUCCESS
   * - DISCOVERY_ERROR
   * - DISCOVERY_TIMEOUT
   * - anything returned from the MCPS-DATA.confirm primitive
   */
  uint32_t Status;

  /** The IEEE address to which the discovery response was sent.
   */
  IEEEAddr Addr;

} NLME_AUTO_DISCOVERY_CONFIRM_Type;

typedef struct NLME_DISCOVERY_CONFIRM_S {
  /** The status of the network discovery attempt.
   * - SUCCESS
   * - DISCOVERY_ERROR
   * - DISCOVERY_TIMEOUT
   * - anything returned from the MCPS-DATA.confirm primitive
   */
  uint32_t Status;
  
  /** The number of discovered nodes in the NodeDescList parameter. 
   * Valid range is: 0x00 - nwkcMaxNodeDescListSize
   */
  uint8_t NumNodes;

  /** The list of node descriptors discovered. */
  NodeDescType NodeDescList[nwkcMaxNodeDescListSize];
} NLME_DISCOVERY_CONFIRM_Type;

/**
 * @brief NLME_PAIR_response parameters
 */
typedef struct NLME_PAIR_RESPONSE_S {
  /** The status of the pairing
   *  request.
   */
  uint32_t Status;
  /** The PAN identifier of the
   *  device requesting the pair.
   */
  uint16_t DstPANId;
  /** The IEEE address of the
   *  device requesting the pair.
   */
  IEEEAddr DstIEEEAddr;
  /** The application capabilities
   *  of the node issuing this
   *  primitive. (Bit mask)
   */
  uint8_t RecAppCapabilities;
  /** The list of device types
   *  supported by the node
   *  issuing this primitive.
   */
  uint8_t RecDevTypeList[MAX_DEV_TYPE_LIST_SIZE];
  /** The list of profile
   *  identifiers supported by the
   *  node issuing this primitive.
   */
  uint8_t RecProfileIdList[MAX_PROFILE_ID_LIST_SIZE];
  /** The reference to the
   *  provisional pairing entry if
   *  the pair was accepted or
   *  0xff otherwise.
   */
  uint8_t ProvPairingRef;
} NLME_PAIR_RESPONSE_Type;

/**
 * @brief NLME_PAIR_indication parameters
 */
typedef struct NLME_PAIR_INDICATION_S {
  /** The status of the
   *  provisional pairing. Valid range is:
   *  - SUCCESS
   *  - NO_REC_CAPACITY
   *  - DUPLICATE_PAIRING
   */
  uint32_t Status;

  /** PAN identifier A valid PAN identifier The PAN identifier of
   *  the device requesting
   *  the pair.
   */
  uint16_t SrcPANId;
 
  /** IEEE address A valid IEEE address The IEEE address of
   *  the device requesting
   *  the pair.
   */
  IEEEAddr SrcIEEEAddr; 
  /** The capabilities of the
   *  originator of the pair
   *  request. (Bitmap)
   */
  uint8_t OrgNodeCapabilities;
  /** Vendor ID: a valid vendor identifier The vendor identifier
   *  of the originator of the
   *  pair request.
   */
  uint16_t OrgVendorId;
  /** The vendor string of
   *  the originator of the
   *  pair request.
   */
  uint8_t OrgVendorString[RF4CE_VENDOR_STRING_SIZE];
  /** The application
   *  capabilities of the
   *  originator of the pair
   *  request.
   */
  uint8_t OrgAppCapabilities;
  /** The vendor string of
   *  the originator of the
   *  pair request.
   */
  uint8_t OrgUserString[RF4CE_USER_STRING_SIZE];  
  /** The list of device
   *  types supported by the
   *  originator of the pair
   *  request. (0x00 terminated)
   */
  uint8_t OrgDevTypeList[MAX_DEV_TYPE_LIST_SIZE];
  /** The list of profile
   *  identifiers supported
   *  by the originator of
   *  the pair request.
   */
  uint8_t OrgProfileIdList[MAX_PROFILE_ID_LIST_SIZE];
  /** The pairing reference
   *  that will be used if
   *  this pairing request is
   *  successful or 0xff if
   *  there is no further
   *  capacity in the pairing
   *  table.
   */
  uint8_t ProvPairingRef;
} NLME_PAIR_INDICATION_Type;


/**
 * @brief NLME_COMM_STATUS_indication parameters
 */
typedef struct NLME_COMM_STATUS_INDICATION_S {
  /** Reference into the pairing table
   *  indicating the recipient node.
   *  A value of 0xff indicates that a
   *  discovery response command frame was
   *  sent.
   */
  uint8_t PairingRef;
  /** The PAN identifier of the destination
   *  device.
   */
  uint16_t DstPANId;
  /** The addressing mode used in the
   *  DstAddr parameter.
   *  1 = DstAddr is 64-bit IEEE address
   *  0 = DstAddr is 16-bit network address
   */
  uint8_t DstAddrMode;
  /** The address of the destination device.
   *  According to the DstAddrMode parameter
   */
  union DstAddr_U {
  /** If DstAddrMode = 0, this is the short address
   */
    uint16_t ShortAddress;
  /** If DstAddrMode = 1, this is the IEEE address
   */
    IEEEAddr IEEEAddress;
  } DstAddr;
  /** The status of the transmission.
   *  Valid range is:
   *  - SUCCESS
   *  - SECURITY_TIMEOUT
   *  - SECURITY_FAILURE
   *  - anything from the MCPS-DATA.confirm primitive
   */
  uint32_t Status;
} NLME_COMM_STATUS_INDICATION_Type;

/**
 * @brief NLME_AUTO_DISCOVERY_request parameters
 */
typedef struct NLME_AUTO_DISCOVERY_REQUEST_S {
  /** Application capabilities of the node issuing this primitive */
  uint8_t RecAppCapabilities;
  /** List of device types supported by the
   *  node issuing this primitive (actual length is in RecAppCapabilities).
   */
  uint8_t RecDevTypeList[MAX_DEV_TYPE_LIST_SIZE];
  /** List of profile identifiers supported
   *  by the node issuing this primitive (actual length is in RecAppCapabilities).
   */
  uint8_t RecProfileIdList[MAX_PROFILE_ID_LIST_SIZE];
  /** Duration of the auto discovery mode (used only 24 bit) */
  uint32_t AutoDiscDuration;
}  NLME_AUTO_DISCOVERY_REQUEST_Type;

/**
 * @brief NLME_DISCOVERY_request parameters
 */
typedef struct NLME_DISCOVERY_REQUEST_S {
  /** Destination PAN identifier 0xffff to indicate wildcard */
  uint16_t DstPANId;
  /** Destination network address. 0xffff to indicate wildcard */
  uint16_t DstNwkAddr;
  /** The application capabilities of the node issuing this primitive */
  uint8_t OrgAppCapabilities;
  /** List of device types supported by the
   *  node issuing this primitive (actual length is in OrgAppCapabilities).
   */
  uint8_t OrgDevTypeList[MAX_DEV_TYPE_LIST_SIZE];
  /** List of profile identifiers disclosed supported
   *  by the node issuing this primitive (actual length is in OrgAppCapabilities).
   */
  uint8_t OrgProfileIdList[MAX_PROFILE_ID_LIST_SIZE];
  /** Device type to discover. 0xff to indicate a wildcard */
  uint8_t SearchDevType;
  /** Number of profile identifiers contained in the DiscProfileIdList */
  uint8_t DiscProfileIdListSize;
  /** List of profile identifiers against which profile
   *  identifiers contained in received discovery response command frames
   *  will be matched for acceptance (actual length is in DiscProfileIdListSize).
   */
  uint8_t DiscProfileIdList[MAX_PROFILE_ID_LIST_SIZE];
  /** Maximum number of MAC symbols to wait for discovery responses
   *  to be sent back from ipotetical target nodes on each channel.
   *  Only 24 bits are used.
   */
  uint32_t DiscDuration;
} NLME_DISCOVERY_REQUEST_Type;

/**
 * @brief NLME_Discovery_indication parameter
 */
typedef struct NLME_DISCOVERY_INDICATION_S {
  /** Status of pairing table */
  uint8_t Status;
  /** IEEE address of the device requesting the discovery */
  IEEEAddr SrcIEEEAddr;
  /** Capabilities of the origiantor of the discovery request */
  uint8_t OrgNodeCapabilities;
  /** Vendor identifier of the originator of the discovery request */
  uint16_t OrgVendorId;
  /** Vendor string of the originator of the discovery request */
  uint8_t OrgVendorString[RF4CE_VENDOR_STRING_SIZE];
  /** Application capabilities of the originator of the discovery request */
  uint8_t OrgAppCapabilities;
  /** User defined identification string of the originator */
  uint8_t OrgUserString[RF4CE_USER_STRING_SIZE];
  /** List of device types supported by the originator (actual length is in OrgAppCapabilities) */
  uint8_t OrgDevTypeList[MAX_DEV_TYPE_LIST_SIZE];
  /** List of profile identifiers supported by the originator 
      (actual length is in OrgAppCapabilities) */
  uint8_t OrgProfileIdList[MAX_PROFILE_ID_LIST_SIZE];
  /** Device type being discovered */
  uint8_t SearchDevType;
  /** LQI value, as passed via the MAC sub-layer, of the discovery request */
  uint8_t RxLinkQuality;
} NLME_DISCOVERY_INDICATION_Type;

/**
 * @brief NLME_Discovery_response parameter
 */
typedef struct NLME_DISCOVERY_RESPONSE_S {
  /** Status of the discovery request */
  uint8_t Status;
  /** IEEE address of the device requesting discovery */
  IEEEAddr DstIEEEAddr;
  /** Application capabilities of the node issuing this primitive */
  uint8_t RecAppCapabilities;
  /** List of the device types supported by the node
   * issuing this primitive (actual length is in RecAppCapabilities).
   */
  uint8_t RecDevTypeList[MAX_DEV_TYPE_LIST_SIZE];
  /** List of the profile identifiers supported
   * by the node issuing this primitive (actual length is in RecAppCapabilities).
   */
  uint8_t RecProfileIdList[MAX_PROFILE_ID_LIST_SIZE];
  /** LQI value from the associated discovery indication */
  uint8_t DiscReqLQI;
} NLME_DISCOVERY_RESPONSE_Type;

/**
 * @brief NLME_Get_request parameter
 */
typedef struct NLME_GETS {
  /** The identifier of the NIB attribute to read */
  uint8_t NIBAttribute;
  /** The index within the table or array of the specified NIB 
   *  attribute to read. This parameter is valid only for NIB 
   *  attributes that are tables or arrays 
   */
  uint8_t NIBAttributeIndex;
  /** The value of the NIB attribute that was read.
   *  This value has a zero length when the return 
   *  function is not equal to SUCCESS. Reserved
   *  the max length between the NIB attributes for 
   *  the return value
   */
  uint8_t *NIBAttributeValue;
} NLME_GET_Type;

/**
 * @brief NLME_Set_request parameter
 */
typedef struct NLME_SETS {
  /** The identifier of the NIB attribute to write.*/
  uint8_t NIBAttribute;
  /** The index within the table or array of the
   *  specified NIB attribute to write. This
   *  parameter is valid only for NIB attributes that
   *  are tables or arrays. Reserved
   *  the max length between the NIB attributes
   */
  uint8_t NIBAttributeIndex;
  /** The value of the indicated attribute to write. */
  uint8_t *NIBAttributeValue;  
} NLME_SET_Type;

/* @} \\END SAP Types */

/**
 * @brief Bitmask to describe rf4ce network state, bit mask values are RF4CE_STATE_BUSY, RF4CE_STATE_NEED_CACHE_FLUSH
 */
typedef uint32_t RF4CE_STATE_Type;

/* Private macro -------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/**@brief  The NLDE-DATA.request primitive requests the transfer of a data APDU (i.e. NSDU) from a local
  *        application entity to a peer application entity.
  *@param  param The structure containing the SAP parameters
  *@retval The status of the data request attempt. Valid values are:
  *        - SUCCESS
  *        - RF4CE_BUSY
  *        - RF4CE_SAP_PENDING
  */
uint32_t NLDE_DATA_request(NLDE_DATA_REQUEST_Type *param);

/**@brief  The NLD-DATA.confirm primitive is generated by the NWK layer entity in response 
  *        to an NLDE-DATA.request primitive. The NLDE-DATA.confirm primitive returns a status 
  *        of either SUCCESS, indicating that the request to transmit was successful, 
  *        or the appropriate error code
  *@note   This function is a callback and must be provided by the application
  *@param  param The structure containing the SAP parameters
  *@retval None
  */
extern void NLDE_DATA_confirm(NLDE_DATA_CONFIRM_Type *param);


/**@brief  The NLDE-DATA.indication primitive is generated by the NLDE and issued 
  *        to the application on receipt of a data frame at the local NWK layer entity.
  *@note   This function is a callback and must be provided by the application
  *@param  param The structure containing the SAP parameters
  *@retval None
  */
extern void NLDE_DATA_indication(NLDE_DATA_INDICATION_Type *param);

/**@brief  The NLME-GET primitive is generated by the application and issued to the NLME 
  *        to obtain information from the NIB.
  *@param  param The structure containing the SAP parameters
  *@retval The result of the request. Valid values are:
  *        - SUCCESS
  *        - UNSUPPORTED_ATTRIBUTE
  *        - INVALID_INDEX
  */
uint32_t NLME_Get(NLME_GET_Type *param);

/**@brief  The NLME-SET primitive is generated by the application and issued to the NLME 
  *        to write the indicated NIB attribute.
  *
  *@param  param The structure containing the SAP parameters
  *
  *@retval The result of the request. Valid values are:
  *        - SUCCESS
  *        - UNSUPPORTED_ATTRIBUTE
  *        - INVALID_INDEX
  */
uint32_t NLME_Set(NLME_SET_Type *param);

/**@brief  The NLME-RX-ENABLE.request primitive allows the application to request that the receiver is either
  *        enabled (for a finite period or until further notice) or disabled.
  *@param  RxOnDuration The number of MAC symbols for which the
  *        receiver is to be enabled. For power saving
  *        mode, this value should correspond to the
  *        value of nwkActivePeriod.
  *        If this parameter is equal to 0x000000, the
  *        receiver is to be disabled until further
  *        notice, allowing the node to enter dormant
  *        power save mode.
  *        If this parameter is equal to 0xffffff, the
  *        receiver is to be enabled until further notice,
  *        allowing the node to come out of power
  *        save modeSpecification of the new network.
  *@retval The result of the request to enable or
  *        disable the receiver provided by .confirm primitive
  */

uint32_t NLME_RX_ENABLE_request (uint32_t RxOnDuration);



/**@brief The NLME-RESET.request primitive allows the application entity to request a reset of the NWK layer.
  *@param  SetDefaultPIB If TRUE, the NWK layer is reset and all NIB
  *        attributes are set to their default values. If
  *        FALSE, the NWK layer is reset but all NIB
  *        attributes retain their values prior to the
  *        generation of the NLME-RESET.request
  *        primitive.
  *@note   This function is called internally by NWK_Init where appropriate, as such is provided for completeness
  *        and it may be removed in future implementations
  *        See also ::NWK_init.
  *@retval The status of the reset request provided by .confirm primitive
  */
uint32_t NLME_RESET_request(bool SetDefaultPIB);

/**@brief  The NLME-START.request primitive allows the application to request the NLME start a network.
  *@note   This function is called internally by NWK_Init where appropriate, as such is provided for completeness
  *        and it may be removed in future implementations
  *@retval Internal error or RF4CE_SAP_PENDING.
  *        See also ::NLME_START_confirm, ::NWK_init.
  */
uint32_t NLME_START_request(void);


/**@brief  The NLME-START.confirm primitive allows the NLME to notify the application of the status of its
  *        request to start a network.
  *@param  status Pointer to the status of the start attempt. Valid range is: SUCCESS or any error status values
  *        returned from the MLMESCAN.confirm primitives.
  *@note   This function is a callback and must be provided by the application
  *@retval None
  *        See also ::NLME_START_request
  */
extern void NLME_START_confirm(uint32_t *status);

/**@brief  The NLME-UPDATE-KEY.request primitive allows the application to request the NLME change the
  *        security link key of an entry in the pairing table.
  *        Note that this primitive provides no network layer support to the application, such as informing the
  *        other node of the key update. This primitive should, therefore, only be used to update the key in the
  *        pairing table following an application defined key exchange mechanism. Such a mechanism is out of
  *        the scope of this specification.
  *@param  PairingRef The reference into the local pairing table of the entry whose key is to be updated.
  *@param  NewLinkKey The security link key to replace the key in the pairing table.
  *@retval The status of the request to update
  *        the security link key. Valid values are:
  *        - SUCCESS
  *        - NO_PAIRING
  *        - NOT_PERMITTED
  */
uint32_t NLME_UPDATE_KEY_request(uint8_t PairingRef, linkKeyType NewLinkKey);

/**@brief  The NLME-AUTO-DISCOVERY.request primitive allows the application to request the NLME
  *        automatically handles the receipt of discovery request command frames. 
  *        Note that during this auto discovery response mode, the NLME does not inform the application 
  *        of the arrival of discovery request command frames via the NLME-DISCOVERY.indication primitive.
  *@param  param The structure containing the SAP parameters
  *@retval The status of the auto discovery attempt. Valid values are:
  *        - SUCCESS
  *        - RF4CE_BUSY
  *        - RF4CE_SAP_PENDING
  */
uint32_t NLME_AUTO_DISCOVERY_request(NLME_AUTO_DISCOVERY_REQUEST_Type *param);

/**@brief  The NLME-AUTO-DISCOVERY.confirm primitive allows the NLME to notify the application of the
  *        status of its request to enter auto discovery response mode.
  *@note   This function is a callback and must be provided by the application 
  *@param  param The structure containing the SAP parameters                
  *@retval None
  */
extern void NLME_AUTO_DISCOVERY_confirm(NLME_AUTO_DISCOVERY_CONFIRM_Type *param);

/**@brief  The NLME-DISCOVERY.request primitive allows the application to request the NLME discover other 
  *        devices of interest operating in the POS of the device.
  *@param  param The structure containing the SAP parameters
  *@retval The status of the auto discovery attempt. Valid values are:
  *        - SUCCESS
  *        - RF4CE_BUSY
  *        - RF4CE_SAP_PENDING
  */
uint32_t NLME_DISCOVERY_request(NLME_DISCOVERY_REQUEST_Type *param);

/**@brief  The NLME-DISCOVERY.indication primitive allows the NLME to notify the application that a
  *        discovery request command has been received.
  *@param  param The structure containing the SAP parameters
  *@note   This function is a callback and must be provided by the application
  *@retval None
  */
extern  void NLME_DISCOVERY_indication(NLME_DISCOVERY_INDICATION_Type *param);
   
/**@brief  The NLME-DISCOVERY.response primitive allows the application to request that the NLME respond
  *        to the discovery request command.
  *@param  param The structure containing the SAP parameters
  *@retval The status of the request to send a response. Valid values are:
  *        - SUCCESS
  *        - RF4CE_BUSY
  *        - RF4CE_SAP_PENDING
  */
uint32_t NLME_DISCOVERY_response(NLME_DISCOVERY_RESPONSE_Type *param);

/**@brief  The NLME-DISCOVERY.confirm primitive allows the NLME to notify the application of the status of
  *        its request to perform a network discovery.
  *@param  param The structure containing the SAP parameters
  *@note   This function is a callback and must be provided by the application
  *@retval None
  */
extern void NLME_DISCOVERY_confirm (NLME_DISCOVERY_CONFIRM_Type *param);

/**@brief  The NLME-PAIR.request primitive allows the application to request the NLME pair with another
  *        device. This primitive would normally be issued following a discovery operation via the
  *        NLME-DISCOVERY.request primitive.
  *@param  param The structure containing the SAP parameters
  *@retval The status of the pair attempt. Valid values are:
  *        - SUCCESS,
  *        - NO_ORG_CAPACITY,
  *        - NO_REC_CAPACITY,
  *        - NO_RESPONSE,
  *        - NOT_PERMITTED,
  *        - SECURITY_FAILURE,
  *        - SECURITY_TIMEOUT
  *        - anything returned from the MCPS-DATA.confirm primitive.
  */
uint32_t NLME_PAIR_request (NLME_PAIR_REQUEST_Type *param);


/**@brief  The NLME-PAIR.confirm primitive allows the NLME to notify the application of the status of its
  *        request to pair with another device.
  *@note   This function is a callback and must be provided by the application
  *@param  params The structure containing the NLME_PAIR_CONFIRM_Type parameters
  *@retval None
  */
extern void NLME_PAIR_confirm(NLME_PAIR_CONFIRM_Type *params);


/**@brief  The NLME-PAIR.response The NLME-PAIR.response primitive allows the application to request
  *        that the NLME respond to a pairing request command.
  *@param  param The structure containing the SAP parameters
  *@retval The status of the pair attempt. Valid values are:
  *        - SUCCESS,
  *        - NO_REC_CAPACITY
  *        - NOT_PERMITTED
  */
uint32_t NLME_PAIR_response(NLME_PAIR_RESPONSE_Type *param);

/**@brief  The NLME-PAIR.indication primitive allows the NLME to notify the application of the reception of a
  *        pairing request command.
  *@note   This function is a callback and must be provided by the application
  *@param  params The structure containing the NLME-PAIR.indication parameters
  *@retval None
  */
extern void NLME_PAIR_indication (NLME_PAIR_INDICATION_Type *params);
   
/**@brief  The NLME-COMM-STATUS.indication primitive allows the NLME to notify the application of a
  *        communication status (only used during the pairing operations).
  *@note   This function is a callback and must be provided by the application
  *@param  params The structure containing the NLME_COMM_STATUS_indication parameters
  *@retval None
  */
extern void NLME_COMM_STATUS_indication (NLME_COMM_STATUS_INDICATION_Type *params);


/**@brief  The NLME-UNPAIR.request primitive allows the application to request the NLME removes a pairing
  *        link with another device both in the local and remote pairing tables.
  *@param  PairingRef The reference into the local
  *        pairing table of the entry that is to
  *        be removed.
  *@retval The status of the unpair attempt. Valid values are:
  *        - RF4CE_SAP_PENDING
  *        - NO_PAIRING
  */

uint32_t NLME_UNPAIR_request (uint8_t PairingRef);


/**@brief  The NLME-UNPAIR.confirm primitive allows the NLME to notify the application of the status of its
  *        request to remove a pair with another device.
  *@note   This function is a callback and must be provided by the application
  *@param  param The structure containing the SAP parameters
  *@retval None
  */
extern void NLME_UNPAIR_confirm(NLME_UNPAIR_CONFIRM_Type *param);


/**@brief  The NLME-UNPAIR.indication primitive allows the NLME to notify the application of the removal of
  *        a pairing link by another device.
  *@note   This function is a callback and must be provided by the application
  *@param  PairingRef Pointer to the pairing table reference that has been removed from the pairing table.
  *@return None
  */
extern void NLME_UNPAIR_indication (uint8_t *PairingRef);

/**@brief  The NLME-UNPAIR.response primitive allows the application to notify the NLME that the pairing
  *        link indicated via the NLME-UNPAIR.indication primitive can be removed from the pairing table.
  *@param  PairingRef The reference into the local
  *        pairing table of the entry that is to
  *        be removed.
  *@retval The status of the unpair response. Valid values are:
  *        - SUCCESS
  */
uint32_t NLME_UNPAIR_response(uint8_t PairingRef);

/**@brief  A tick routine that should be called periodically in the application's main event loop to
  *        check events, advance state machine, invoke callbacks, manage power state, check sleep timer 
  *@retval bitmask to indicate the network state
  */
RF4CE_STATE_Type NWK_Tick(void);


/**@brief Network initialization function to start RF4CE network layer
  *@param nodeCap The node capabilities as defined by RF4CE standard
  *@param forceColdStart This parameters allow the network layer to perform a cold start (e.g. the first 
  *       startup outside the factory.
  *@note  This function calls internally the NLME-RESET and the NLME-START where appropriate.
  *@retval Status of the network init
  */
uint32_t NWK_Init(uint8_t nodeCap, bool forceColdStart);

/**@brief  Flush cache into NVM
  *@note   This function should be called in response to a NWK_Tick return value set to
  *        RF4CE_STATE_NEED_CACHE_FLUSH
  *@retval Status of the flush operation
  *        See also ::NWK_Tick
  */
uint32_t NWK_Flush(void);

/**@brief  Helper function to disable RF part of the device
  *@note   This function is equivalent to call NLME_RX_ENABLE_request(0) and its usage should be preferred
  *@retval Status of the operation
  *        See also ::NLME_RX_ENABLE_request, NWK_PowerUp
  */
uint32_t NWK_PowerDown(void);

/**@brief  Helper function to wakeup RF4CE network layer
  *@param  fromDeepSleep This flag must be set to TRUE when waking up from dppe sleep
  *@note   This function is simliar to call NLME_RX_ENABLE_request(0xFFFFFFF) and its usage should be preferred
  *        Its usage is also mandatory when exiting from deep sleep.
  *@retval Status of the operation
  *        See also ::NLME_RX_ENABLE_request, NWK_PowerDown
  */
uint32_t NWK_PowerUp(bool fromDeepSleep);

/**@brief  Helper function to configure RF4CE network layer prior to cold start initialization
  *@param  parameterID    This value identifies the RF4CE stack parameter to be changed
  *@param  parameterValue Opaque pointer to parameter value
  *@note   This function must be called prior to cold starting the sack with NWK_Init.
  *@retval Status of the operation
  */
uint32_t NWK_Config(uint32_t parameterID, void *parameterValue);

/**@brief  This function return the network coprocessor firmware version and the rf4ce 
  *        firmware version. The format version is MAJOR-MINOR-PATCH
  *@param  nwkCop_ver This return value identifies the network coprocessor firmware version
  *@param  rf4ce_ver This return value identifies the RF4CE firmware version
  *@retval Status of the operation
  */
uint32_t nwkCop_FirmwareVersion(uint8_t *nwkCop_ver, uint8_t *rf4ce_ver);

/**@brief  This function enables the power save on the STM32W
  *@param  powerSaveType This parameter can be:
  *        - 0 (SLEEPMODE_IDLE)
  *        - 1 (SLEEPMODE_NOTIMER)
  *@retval Status of the operation
  */
uint32_t nwkCop_enablePowerSave(uint8_t powerSaveType);

/**@brief  This function disables the power save on the STM32W
  *@param  None
  *@retval Status of the operation
  */
uint32_t nwkCop_disablePowerSave(void);

/**@brief  This support function erases the NVM memory 
  *        that contains the pairing table.
  *@param  None          
  *@retval Status of the operation
  */
uint32_t nwkCop_NVM_Erase(void);

/**@brief  This is a NOP function used to test the serial communication.
  *@param  None
  *@retval Status of the operation
  */
uint32_t nwkCop_NOP(void);

/**@brief  This function executes a demultiplexing of the rf4ce callback
  *        functions:
  *        - NLDE_DATA_INDICATION
  *        - NLDE_DATA_CONFIRM
  *        - NLME_AUTO_DISCOVERY_CONFIRM
  *        - NLME_COMM_STATUS_INDICATION
  *        - NLME_DISCOVERY_INDICATION
  *        - NLME_DISCOVERY_CONFIRM
  *        - NLME_PAIR_INDICATION
  *        - NLME_PAIR_CONFIRM
  *        - NLME_START_CONFIRM
  *        - NLME_UNPAIR_INDICATION
  *        - NLME_UNPAIR_CONFIRM   
  *@param  None         
  *@retval None
  */
void RspCallback(uint8_t *packet);

#endif /* RF4CE_H */

/**
  * @}
  */    /* END addtogroup */
 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
