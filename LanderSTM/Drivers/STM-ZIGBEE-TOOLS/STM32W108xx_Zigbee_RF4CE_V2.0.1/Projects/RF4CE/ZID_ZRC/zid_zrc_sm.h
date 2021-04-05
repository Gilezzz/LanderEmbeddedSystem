/**
  ******************************************************************************
  * @file    zid_zrc_sm.h
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   ZID ZRC RF4CE state machines  header file
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

#ifndef ZID_ZRC_SM_H
#define ZID_ZRC_SM_H

/* HID INIT states */
#define HID_STATE_INIT 0x00000000

/* HID IDLE states */
#define HID_STATE_IDLE 0x00010000

/* HID START states */
#define HID_STATE_START 0x00020000
#define HID_SUBSTATE_START_INIT       (HID_STATE_START | 0x0001)
#define HID_SUBSTATE_START_WAITING    (HID_STATE_START | 0x0002)
#define HID_SUBSTATE_START_DONE       (HID_STATE_START | 0x0003)

/* HID PAIRING states */
#define HID_STATE_PAIRING 0x00030000
#define HID_SUBSTATE_PAIRING_INIT                 (HID_STATE_PAIRING | 0X0001)
#define HID_SUBSTATE_PAIRING_DSC_REQ              (HID_STATE_PAIRING | 0X0002)
#define HID_SUBSTATE_PAIRING_DSC_WAITING_CONF     (HID_STATE_PAIRING | 0X0003)
#define HID_SUBSTATE_PAIRING_PAIR_REQ             (HID_STATE_PAIRING | 0X0004)
#define HID_SUBSTATE_PAIRING_PAIR_WAITING_CONF    (HID_STATE_PAIRING | 0X0005)
#ifdef ZID_FULL
#define HID_SUBSTATE_PAIRING_AUTODSC_REQ          (HID_STATE_PAIRING | 0X0006)
#define HID_SUBSTATE_PAIRING_AUTODSC_WAITING_CONF (HID_STATE_PAIRING | 0X0007)
#define HID_SUBSTATE_PAIRING_PAIR_IND             (HID_STATE_PAIRING | 0X0008)
#define HID_SUBSTATE_PAIRING_PAIR_RSP             (HID_STATE_PAIRING | 0X0009)
#define HID_SUBSTATE_PAIRING_WAITING_COMM_IND     (HID_STATE_PAIRING | 0X000a)
#endif
#define HID_SUBSTATE_PAIRING_DONE                 (HID_STATE_PAIRING | 0X000b)


/* HID CONFIGURATION states */
#define HID_STATE_CONFIGURATION 0x00040000
#define HID_SUBSTATE_CONFIGURATION_INIT                     (HID_STATE_CONFIGURATION | 0X0001)
#ifdef ZID_FULL
#define HID_SUBSTATE_CONFIGURATION_WAIT_GET_ATTR            (HID_STATE_CONFIGURATION | 0X0002)
#define HID_SUBSTATE_CONFIGURATION_SEND_GET_ATTR_RSP        (HID_STATE_CONFIGURATION | 0X0003)
#define HID_SUBSTATE_CONFIGURATION_WAIT_HID_ATTR            (HID_STATE_CONFIGURATION | 0X0004)
#define HID_SUBSTATE_CONFIGURATION_CHECK_ATTR               (HID_STATE_CONFIGURATION | 0X0005)
#define HID_SUBSTATE_CONFIGURATION_UNPAIR_DEVICE            (HID_STATE_CONFIGURATION | 0X0006)
#define HID_SUBSTATE_CONFIGURATION_WAIT_UNPAIR_CONFIRM      (HID_STATE_CONFIGURATION | 0X0007)
#endif
#define HID_SUBSTATE_CONFIGURATION_SEND_GET_ATTR            (HID_STATE_CONFIGURATION | 0X0008)
#define HID_SUBSTATE_CONFIGURATION_WAIT_ATTR_RSP            (HID_STATE_CONFIGURATION | 0X0009)
#define HID_SUBSTATE_CONFIGURATION_PUSH_ATTR                (HID_STATE_CONFIGURATION | 0X000a)
#define HID_SUBSTATE_CONFIGURATION_WAIT_GENERIC_RESPONSE    (HID_STATE_CONFIGURATION | 0X000b)
#define HID_SUBSTATE_CONFIGURATION_UNPAIR_HOST              (HID_STATE_CONFIGURATION | 0X000c)
#define HID_SUBSTATE_CONFIGURATION_SEND_CFG_COMP            (HID_STATE_CONFIGURATION | 0X000d)
#define HID_SUBSTATE_CONFIGURATION_DONE                     (HID_STATE_CONFIGURATION | 0X000e)

#ifdef ZID_FULL
/* HID PROXY states */
#define HID_STATE_PROXY 0x00050000
#define HID_SUBSTATE_PROXY_INIT                           (HID_STATE_PROXY | 0x0001)
#define HID_SUBSTATE_PROXY_GET_DEV_ATTR                   (HID_STATE_PROXY | 0x0002)
#define HID_SUBSTATE_PROXY_CLASS_DRIVER_NOTIFICATION      (HID_STATE_PROXY | 0x0003)
#define HID_SUBSTATE_PROXY_CLASS_DRIVER_WAIT_NOTIFICATION (HID_STATE_PROXY | 0x0004)
#define HID_SUBSTATE_PROXY_DONE                           (HID_STATE_PROXY | 0x0005)
#endif

/* HID UNPAIRED states */
#define HID_STATE_UNPAIRED 0x00060000

/* ZRC Configuration states */
#define ZRC_CONFIGURATION_INIT          0
#define ZRC_CONFIGURATION_WAIT_RSP      1
#define ZRC_CONFIGURATION_BLACKOUT_TIME 2

/* Start Context Type */
typedef struct startContextS {
  /* Flag to indicate the NLME_Start_confirm received */
  bool startComplete;
  /* Start Status response */
  uint32_t status;
  /* start time for timeout */
  uint32_t startTime;
  /* Profile Supported */
  uint8_t profileSupported;
} startContextType;

/* Pairing Context Type */
typedef struct pairingContextS {
  /* Pairing parameters */
  ZID_Pairing_Type param;
  /* Start time for a timer */
  uint32_t startTime;
  /* Pairing status */
  uint32_t pairingStatus;
  /* New Pairing reference */
  uint8_t newPairRef;
  /* Discovery Confirm Information */
  NLME_DISCOVERY_CONFIRM_Type dscConfirm;
  /* Pair Indication Information */
  NLME_PAIR_INDICATION_Type pairIndication;
  /* Flag to signal the discovery confirm */
  bool dscConf;
  /* Flag to signal the autodiscovery confirm */
  bool autoDscConf;
  /* Flag to signal the pair confirm */
  bool pairConf;
  /* Flag to signal the pair indication */
  bool pairInd;
  /* Flag to signal the comm status indication */
  bool commInd;
  /* Profile Supported */
  uint8_t profileSupported;  
  /* Remote number profile supported */
  uint8_t numProfile;
  /* Remote profile ID list */
  uint8_t profileList[7];
} pairingContextType;

typedef struct hidInfoS {
  uint8_t id;
  uint8_t received;
} hidInfoType;

typedef struct configurationContextS {
  /* Start time for the config
   * mode max duration
   */
  uint32_t startTime;
  /* Pairing Reference */
  uint8_t pairRef;
  /* Command request message */
  uint8_t msg[128];
  /* Message Length */
  uint8_t msgLen;
  /* Flag to signal a message received from a remote node */
  bool msgRcv;
  /* Non standard descriptor */
  uint8_t nonStdDesc[260];
  /* Number of non standard descriptor fragment */
  uint8_t fragment;
  /* Size of non stadard descriptor received */
  uint16_t descSizeRcv;
  /* List of attrbibutes received during configuration */
  hidInfoType proxyTableAttr[15];
  /* Flag to signal if the configuration is ended with success */
  bool configurationSuccess;
  /* Flag to signal the unpair request
   */
  bool unpairSignal;
  /* Which type of ZID attributes manage. Possible value are:
   * - 0x00 aplHID attr
   * - 0x01 non standard descriptors
   * - 0x02 NULL reports
   * - 0x03 Send Configuration Complete
   * - 0x04 Config Done
   */
  uint8_t zidAttrType;
  /* The non standard descriptor already sent
   */
  uint8_t nonStdDescSent;
  /* The number of non standard descriptor to collect
   * from the remote node
   */
  uint8_t numNonStdDesc;
  /* Number of Null Report */
  uint8_t numNullRpt;
  /* Number of Null Report sent */
  uint8_t numNullRptSent;
  /* List of report Id */
  uint8_t rptId[aplcMaxNonStdDescCompsPerHID];
  /** Flag to signal if the Generic Response at the End Configuration Message is sent */
  bool genRspSent;
  /* Profile Supported */
  uint8_t profileSupported;  
  /* ZRC configuration state */
  uint8_t zrcState;
  /* Remote number profile supported */
  uint8_t numProfile;
  /* Remote profile ID list */
  uint8_t profileList[7];
} configurationContextType;

typedef struct proxyContextS {
  /** Pairing reference for the device to proxy
   */
  uint8_t  pairRef;
  /** Flag to signal if start all device
   */
  uint32_t sourceState;
  /** Timeout to reset the host */
  uint32_t startTime;
} proxyContextType;

/** Dummy type just to compute the largest of all contexts */
typedef union hidGlobalContextU {
  startContextType a;
  pairingContextType b;
  configurationContextType d;
  proxyContextType e;
} hidGlobalContextType;

extern hidGlobalContextType *hidGlobalContext;

typedef struct HID_StateS {
  /** Macro State */
  uint32_t state;
  /** Substate */
  uint32_t subState;
} HID_StateType;

HID_StateType hidStateMachine(HID_StateType state);

/**@brief  This function sets the HID command response
  *        waited  in aplcMaxResponseWaitTime
  *@param  pairRef pairing reference
  *@param  cmd command code sent
  *@return None
  */
void ZID_WaitCmdRsp(uint8_t pairRef, uint8_t cmd);

/**@brief  Enable internal power save
  *@param  None
  *@return boolean value
  */
bool internalEnablePowerSave(void);

/**@brief  This function saves the last report received from an HID device.
  *        This report will be repeated at idle time to the USB host
  *@param  pairRef pairing reference
  *@param  rptId report identifier
  *@param  rptData report data
  *@param  rptLen report data length
  *@return None
  */
void ZID_SetReportToRepeat(uint8_t pairRef, uint8_t rptId, uint8_t *rptData, uint8_t rptLen);

/**@brief  This function increase a counter to signal the state machine the 
  *        number of message sent and still pending in the send queue
  *@return None
  */
void ZID_SetMessageSent(void);

#endif /* ZID_ZRC_SM_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
