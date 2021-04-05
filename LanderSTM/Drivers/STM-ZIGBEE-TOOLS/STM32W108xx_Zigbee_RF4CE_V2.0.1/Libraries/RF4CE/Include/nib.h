/**
  ******************************************************************************
  * @file    nib.h
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   nib header file
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
  * @addtogroup nib
  * See nib.h for source code.
  * @{
  */

#ifndef NIB_H
#define NIB_H

/* Includes ------------------------------------------------------------------*/
#include "rf4ce_type.h"

/**
  * @name NWK constants
  * @{
  */


/* Private define ------------------------------------------------------------*/  

/**
  * @brief The channel mask to use in all scanning operations.
  */
#define nwkcChannelMask 0x02108000

/**
  * @brief The amount that needs to be
  * added to the frame counter if
  * a device is reset.The channel mask to use in all scanning operations.
  */
#define nwkcFrameCounterWindow 1024


/**
  * @brief The length, in octets, of the
  * MAC beacon payload field, as
  * used by the ZigBee RF4CE
  * protocol.
  */
#define nwkcMACBeaconPayloadLength 2

/**
  * @brief The maximum number of entries supported in the node descriptors list generated
  * through the discovery process.
  * Implementation specific but >=
  * nwkcMinNodeDescListSize
  */
#define nwkcMaxNodeDescListSize 3

/**
  * @brief The maximum duty cycle in MAC symbols, permitted for a power saving device.
  */
#define nwkcMaxDutyCycle 62500 /* (1s) */

/**
  * @brief The maximum time, in MAC symbols, to wait for each security link key seed
  * exchange.
  */
#define nwkcMaxKeySeedWaitTime 3750 /* (60ms) */

/**
  * @brief The maximum number of entries supported in the
  * pairing table.
  */
#define nwkcMaxPairingTableEntries 5 /* Implementation specific */

/**
  * @brief The maximum acceptable power, in dBm, at which commands relating to security
  * should be sent.
  */
#define nwkcMaxSecCmdTxPower -15

/**
  * @brief The minimum receiver on time, in MAC symbols, permitted for a power saving
  * device.
  */
#define nwkcMinActivePeriod 1050 /* (16.8ms) */

/**
  * @brief The minimum number of pairing table entries that a controller device shall
  * support.
  */
#define nwkcMinControllerPairingTableSize 1

/**
  * @brief The minimum number of entries that must be supported in the node descriptor list
  * generated through the discovery process.
  */
#define nwkcMinNodeDescListSize 3

/**
  * @brief The minimum overhead the NWK layer adds to an application payload
  */
#define nwkcMinNWKHeaderOverhead 5

/**
  * @brief The minimum number of pairing table entries that a target device shall support.
  */
#define nwkcMinTargetPairingTableSize 5

/**
  * @brief The capabilities of this node. Kept in NVM
  */
#define nwkcNodeCapabilities_NVM  /* Implementation specific and stored in flash */

/**
  * @brief The identifier of the NWK layer protocol being used by this device.
  */
#define nwkcProtocolIdentifier 0xce

/**
  * @brief The version of the ZigBee RF4CE protocol implemented on this device.
  */
#define nwkcProtocolVersion    0x01

/**
  * @brief The manufacturer specific vendor identifier for this node. Kept in NVM
  */
#define nwkcVendorIdentifier_ID          0x90

/**
  * @brief The manufacturer specific identification string for this node. Kept in NVM
  */
#define nwkcVendorString_ID              0x91


/**
 * @}
 */   /* END NWK constants */

/**
  * @name NWK NIB attributes
  *@{
  */

/**
  * @brief (NIB identifier) The active period of a device in MAC symbols.
  */
#define nwkActivePeriod_ID      0x60

/**
  * @brief (Default value) The active period of a device in
  * MAC symbols. Valid range nwkcMinActivePeriod - 0xffffff
  */
#define nwkActivePeriod_Default 0xffffff /* (268.44s)*/
#define nwkActivePeriod_Type    NVM_TYPE_U32

/**
  * @brief (NIB identifier) The logical channel that was chosen during device initialization.
  */
#define nwkBaseChannel_ID 0x61
/**
  * @brief (Default value) The logical channel that was chosen during device
  * initialization.
  * Valid range 15, 20 or 25
  */
#define nwkBaseChannel_Default 15
#define nwkBaseChannel_Type    NVM_TYPE_U8

/**
  * @brief (NIB identifier) The LQI threshold below which discovery requests will be
  * rejected.
  */
#define nwkDiscoveryLQIThreshold_ID 0x62
/**
  * @brief (Default value) The LQI threshold belowwhich discoveryrequests will be rejected.
  * Valid range 0x00 – 0xff
  */
#define nwkDiscoveryLQIThreshold_Default 0xFF
#define nwkDiscoveryLQIThreshold_Type    NVM_TYPE_U8

/**
  * @brief (NIB identifier) The interval, in MAC symbols, at which discovery attempts are made
  * on all channels.
  */
#define nwkDiscoveryRepetitionInterval_ID 0x63
/**
  * @brief (Default value) The interval, in MAC symbols, at which discovery
  * attempts are made on all channels. Valid range 0x000000 - 0xffffff
  */
#define nwkDiscoveryRepetitionInterval_Default 0x0030d4 /* (200 ms) */
#define nwkDiscoveryRepetitionInterval_Type    NVM_TYPE_U32

/**
  * @brief (NIB identifier) The duty cycle of
  * a device in MAC
  * symbols. A value
  * of 0x000000
  * indicates the
  * device is not
  * using power
  * saving.
  */
#define nwkDutyCycle_ID 0x64
/**
  * @brief (Default value) he duty cycle of a device in MAC symbols. A value
  * of 0x000000 indicates the device is not using power saving.
  * Valid range 0x000000 - nwkcMaxDutyCycle
  */
#define nwkDutyCycle_Default 0x000000
#define nwkDutyCycle_Type    NVM_TYPE_U32

/**
  * @brief (NIB identifier) The frame
  * counter added to
  * the transmitted
  * NPDU.
  */
#define nwkFrameCounter_ID 0x65
/**
  * @brief (Default value) The frame
  * counter added to
  * the transmitted
  * NPDU.
  * Valid range 0x00000001 - 0xffffffff
  */
#define nwkFrameCounter_Default 0x00000001
#define nwkFrameCounter_Type    NVM_TYPE_U32

/**
  * @brief (NIB identifier) Indicates whether
  * the NLME
  * indicates the
  * reception of
  * discovery request
  * command frames
  * to the application.
  * TRUE indicates
  * that the NLME
  * notifies the
  * application.
  */
#define nwkIndicateDiscoveryRequests_ID 0x66
/**
  * @brief (Default value) Indicates whether
  * the NLME
  * indicates the
  * reception of
  * discovery request
  * command frames
  * to the application.
  * TRUE indicates
  * that the NLME
  * notifies the
  * application.
  * Valid range TRUE or FALSE
  */
#define nwkIndicateDiscoveryRequests_Default FALSE
#define nwkIndicateDiscoveryRequests_Type    NVM_TYPE_U8


/**
  * @brief (NIB identifier) The power save
  * mode of the node.
  * TRUE indicates
  * that the device is
  * operating in
  * power save mode.
  */
#define nwkInPowerSave_ID 0x67
/**
  * @brief (Default value) The power save
  * mode of the node.
  * TRUE indicates
  * that the device is
  * operating in
  * power save mode.
  * Valid range TRUE or FALSE
  */
#define nwkInPowerSave_Default FALSE
#define nwkInPowerSave_Type    NVM_TYPE_U8


/**
  * @brief (NIB identifier) The pairing table
  * managed by the
  * device.
  */
#define nwkPairingTable_ID 0x68
/**
  * @brief (Default value) The pairing table
  * managed by the
  * device.
  * Valid range List of pairing table entries
  */
#define nwkPairingTable_Default NULL

/**
  * @brief (NIB identifier) The maximum
  * number of
  * discovery
  * attempts made at
  * the nwkDiscoveryRepetitionInterval
  * rate.
  */
#define nwkMaxDiscoveryRepetitions_ID 0x69
/**
  * @brief (Default value) The maximum
  * number of
  * discovery
  * attempts made at
  * the nwkDiscoveryRepetitionInterval
  * rate.
  * Valid range 0x01 - 0xff
  */
#define nwkMaxDiscoveryRepetitions_Default 0x01
#define nwkMaxDiscoveryRepetitions_Type    NVM_TYPE_U8

/**
  * @brief (NIB identifier) The maximum number of backoffs the MAC CSMA-CA
  * algorithm will attempt before declaring a channel access failure for the first transmission
  * attempt.
  */
#define nwkMaxFirstAttemptCSMABackoffs_ID 0x6A
/**
  * @brief (Default value) The maximum number of backoffs the MAC CSMA-CA
  * algorithm will attempt before declaring a channel access failure for the
  * first transmission attempt.
  * Valid range 0-5
  */
#define nwkMaxFirstAttemptCSMABackoffs_Default 4
#define nwkMaxFirstAttemptCSMABackoffs_Type  NVM_TYPE_U8

/**
  * @brief (NIB identifier) The maximum number of MAC
  * retries allowed after a transmission failure for the first transmission attempt.
  */
#define nwkMaxFirstAttemptFrameRetries_ID 0x6B

/**
  * @brief (Default value) The maximum number of MAC retries allowed after a
  * transmission failure for the first transmission attempt.
  * Valid range 0-7
  */
#define nwkMaxFirstAttemptFrameRetries_Default 3
#define nwkMaxFirstAttemptFrameRetries_Type NVM_TYPE_U8

/**
  * @brief (NIB identifier) The maximum number of node descriptors that can
  * be obtained before reporting to the application.
  */
#define nwkMaxReportedNodeDescriptors_ID 0x6C

/**
  * @brief (Default value) The maximum number of node descriptors that
  * can be obtained before reporting to the application.
  * Valid range 0x00-nwkcMaxNodeDescListSize
  */
#define nwkMaxReportedNodeDescriptors_Default 0x03
#define nwkMaxReportedNodeDescriptors_Type NVM_TYPE_U8

/**
  * @brief (NIB identifier) The maximum time in MAC symbols, a device shall wait for a
  * response command frame following a request command frame.
  */
#define nwkResponseWaitTime_ID 0x6D
/**
  * @brief (Default value) The maximum time in MAC symbols, a device shall wait for a
  * response command frame following a request command frame.
  * Valid range 0x000000-0xffffff
  */
#define nwkResponseWaitTime_Default 0x00186a /* (100ms) */
#define nwkResponseWaitTime_Type    NVM_TYPE_U32

/**
  * @brief (NIB identifier) A measure of the duration of a scanning operation,
  * according to 802.15.4-2006.
  */
#define nwkScanDuration_ID 0x6E

/**
  * @brief (Default value) A measure of the duration of a scanning operation,
  * according to 802.15.4-2006.
  * Valid range 0-14
  */
#define nwkScanDuration_Default 6
#define nwkScanDuration_Type    NVM_TYPE_U8

/**
  * @brief (NIB identifier) The user defined
  * character string
  * used to identify
  * this node 
  */
#define nwkUserString_ID 0x6f
/**
  * @brief (Default value) The user defined
  * character string
  * used to identify
  * this node 
  * Valid range 15 chars 
  */
#define nwkUserString_Default "STUser"

/* Private typedef -----------------------------------------------------------*/

typedef uint8_t nwkUserString_Type[16];

/**
  * @}
  */    /* END NWK NIB attributes */
#pragma pack(1)
/**
  * @brief Pairing table entry
  */
typedef struct PairingTableEntryS {
  /** The network address to be assumed by
     the source device. Valid range: 0x0000-0xfffe */
  uint16_t sourceNetworkAddress;
  
  /** The expected channel of the destination
     device. Valid range: 15, 20 or 25 */
  uint8_t destinationLogicalChannel;

  /** The IEEE address of the destination device.
     Valid range: A valid IEEE address */
  IEEEAddr destinationIEEEAddress;

  /** The PAN identifier of the destination device.
     Valid range: 0x0000 - 0xfffe */
  uint16_t destPANIdentifier;

  /** The network address of the destination device.
     Valid range: 0x0000-0xfffe */
  uint16_t destinationNetworkAddress;

  struct BitmapS {
    /** 1 if target, 0 if controller */
    uint8_t nodeType : 1;
    /** 1 if power from alternating current mains, 0 otherwise */
    uint8_t powerSource : 1;
    /** 1 if security capable, 0 otherwise */
    uint8_t securityCapable : 1;
    /** 1 if the node will react to a channel change request through the channel designator sub-field of 
       the frame control field, 0 otherwise */
    uint8_t channelNormalizationCapable : 1;
    /** Reserved field set to zero */
    uint8_t reserved : 4;
  } recipientCapabilities;
  /** The frame counter last received from the
     recipient node. Valid range: 0x00000000-0xffffffff */
  uint32_t recipientFrameCounter;

  /** The link key to be used to secure this pairing link.
     Valid range: A valid 128-bit key */
  linkKeyType securityLinkKey;

  /** The status of the link key entry */
  uint8_t status;
} PairingTableEntryType;
#pragma pack()
/**
  * @brief Pairing table type definition
  */

typedef PairingTableEntryType nwkPairingTable_Type[nwkcMaxPairingTableEntries];

/**
  * @} 
  */   /* END addtogroup */
  

#endif /* NIB_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
