/**
  ******************************************************************************
  * @file    zid_rf4ce.h
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   ZID RF4CE Device attributes header file
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
 * @addtogroup zid_rf4ce_attributes ZID RF4CE ATTRIBUTES API
 * See zid_rf4ce_attributes.h for source code.
 * @{
 */

#ifndef ZID_RF4CE_ATTRIBUTES_H
#define ZID_RF4CE_ATTRIBUTES_H

#include  "rf4ce.h"
#include  "nvm.h"

/**
  *@name GDP RF4CE attributes
  *@{
  */

/**
  * @brief Specifies the value of the KeyExTransferCount parameter passed 
  * to the pair request primitive during the push button pairing procedure.
  * This value shall be specified in the range (aplcMinKeyExchangeTransferCount - 0xff)
  */
#define aplKeyExchangeTransferCount_ID      0x82
#define aplKeyExchangeTransferCount_Default 0x24

/**
  * @brief It is 8 bits in length and specifies the high level status of the power supply 
  * of the node. This attribute shall be formatted as follow:
  * - Bits 0-3 Power meter 
  * - Bits 4-5 Reserved
  * - Bit  6   Charging
  * - Bit  7   Impending doom 
  * The power meter sub-field is 4 bits in length and specifies the status of the power source in the node in 16 values, 
  * ranging from lowest power (0x0) to highest power (0xf). The charging sub-field is 1 bit in length and specifies whether 
  * the power source in the node is being charged. This value shall be set to 1 to indicate that the node is being charged, 
  * 0 otherwise. The impending doom sub-field is 1 bit in length and specifies whether the node is about to cease all activity.
  * This value shall be set to 1 to indicate that it is about to cease all activity, 0 otherwise.
  */
#define aplPowerStatus_ID      0x90
#define aplPowerStatus_Default 0x00

//@} \\END GDP RF4CE attributes

/**
  *@name ZID RF4CE attributes
  *@{
  */

/**
  * @brief Shall specify the version of the ZID profile specification to which 
  * the device was designed. The format of this value shall be 0xJJMN representing 
  * a version JJ.M.N, where JJ is the major version number, M is the minor version 
  * number and N is the sub-minor version number. 
  * For this version of the ZID profile specification, the aplZIDProfileVersion 
  * attribute shall be set to the value 0x0100, representing v1.0.0.
  */
#define aplZIDProfileVersion_ID      0xa0
#define aplZIDProfileVersion_Default 0x0100

/**
  * @brief This attribute is 16 bits in length and specifies the maximum time, 
  * during which transmissions use the unacknowledged, single channel service, 
  * permitted before an acknowledged, multiple channel transmission is required. 
  * This value shall be specified in the range (aplcMinIntPipeUnsafeTxWindowTime - 0xffff).
  * The values are specified in msec.
  */
#define aplIntPipeUnsafeTxWindowTime_ID      0xa1
#define aplIntPipeUnsafeTxWindowTime_Default aplcMinIntPipeUnsafeTxWindowTime

/**
  * @brief This attribute is 8 bits in length and specifies the interval in 
  * milliseconds at which a repeated report data command frame is transmitted
  * to the HID adaptor. If the value of this attribute is zero, the HID class
  * device shall not auto-repeat reports on behalf of the application. 
  * This value shall be specified in the range (0-aplcMaxReportRepeatInterval).
  */
#define aplReportRepeatInterval_ID       0xa2
#define aplReportRepeatInterval_Default  (aplcMaxReportRepeatInterval/2)

/**
  * @brief Specify the version of the core HID specification to which 
  * the device was designed. The format of this value shall be 0xJJMM 
  * representing a version JJ.M.N, where JJ is the major version number, 
  * M is the minor version number and N is the sub-minor version number.
  */
#define aplHIDParserVersion_ID      0xe0
#define aplHIDParserVersion_Default 0x0111

/**
  * @brief Shall specify the HID subclass code of the HID device.
  * This value shall be set to one of the non reserved values listed:
  * - 0x00      No subclass
  * - 0x01      Boot interface subclass
  * - 0x02:0xff Reserved
  */
#define aplHIDDeviceSubclass_ID      0xe1
#define aplHIDDeviceSubclass_Default 0x00

/**
  * @brief Shall specify the HID protocol code of the HID device supporting
  * the boot protocol. This value shall be set to one of the 
  * non reserved values listed:
  * - 0x00      None 
  * - 0x01      Keyboard
  * - 0x02      Mouse 
  * - 0x03:0xff Reserved
  */
#define aplHIDProtocolCode_ID      0xe2
#define aplHIDProtocolCode_Default 0x00

/**
  * @brief Shall specify the country for which the hardware is localized.
  */
#define aplHIDCountryCode_ID      0xe3
#define aplHIDCountryCode_Default 0x00

/**
  * @brief Shall specify the release number of the device itself. 
  * The format of this value shall be 0xJJMN representing a version JJ.M.N,
  * where JJ is the major version number, M is the minor version number 
  * and N is the sub-minor version number. 
  * This attribute is used in the device descriptor.
  * Default value 1.00
  */
#define aplHIDDeviceReleaseNumber_ID      0xe4
#define aplHIDDeviceReleaseNumber_Default 0x0100

/**
  * @brief Shall specify the USB-IF assigned identifier corresponding 
  * to the vendor of the device.
  * This attribute is used in the device descriptor.
  * Default value 0x0483 = STMicroelectronics
  */
#define aplHIDVendorId_ID      0xe5
#define aplHIDVendorId_Default 0x0483

/**
  * @brief Shall specify a manufacturer assigned product identifier 
  * for the device. 
  * This attribute is used in the device descriptor.
  * Default value 0x7541
  */
#define aplHIDProductId_ID      0xe6
#define aplHIDProductId_Default 0x7541

/**
  * @brief Shall specify the number of endpoints on the device in addition 
  * to the mandatory control endpoint. This value shall be set to 0x01 
  * if only the mandatory input endpoint is used by the device or 0x02 
  * if both the mandatory input endpoint and the optional output endpoint 
  * are used by the device. 
  * This attribute is used in the interface descriptor.
  */
#define aplHIDNumEndpoints_ID      0xe7
#define aplHIDNumEndpoints_Default 0x01

/**
  * @brief Shall specify the polling interval for all endpoint data transfers, 
  * in 125us units. This value shall be set in the range 1 to 16 and is used 
  * as an exponent to calculate the value:
  * -    Poll Interval = 2^(aplHIDPollInterval-1)
  * This attribute is used in the endpoint descriptor.
  */
#define aplHIDPollInterval_ID      0xe8
#define aplHIDPollInterval_Default 0x01
#define aplHIDPollInterval_MIN     0x01
#define aplHIDPollInterval_MAX     0x10

/**
  * @brief Is an 8-bit integer in the range 0x00 - aplcMaxStdDescCompsPerHID
  * Shall specify the number of RF4CE HID profile defined standard
  * descriptors supported by the device
  */
#define aplHIDNumStdDescComps_ID      0xe9
#define aplHIDNumStdDescComps_Default 0x00

/**
  * @brief This attribute is of length equal to the value of the 
  * aplHIDNumStdDescComps attribute and shall specify the list of standard 
  * RF4CE HID profile defined descriptors supported by the device
  */
#define aplHIDStdDescCompsList_ID   0xea

/**
  * @brief This attribute is an 8 bits in length and the value are 
  * in the range 0x00-aplHIDNumNonStdDescComps and shall specify the 
  * number of NULL reports (each corresponding to a non standard 
  * descriptor component) defined by the HID class device.
  */
#define aplHIDNumNullReports_ID      0xeb
#define aplHIDNumNullReports_Default 0x00

/**
  * @brief Shall specify the number of non standard HID descriptors stored 
  * on the device. These descriptors can be read via successive read attribute 
  * list command frames sent to the device; only one descriptor shall be 
  * requested with each read attribute list command frame.
  * Valid range is 0x00 - aplcMaxNonStdDescCompsPerHID
  * This attribute is used in the HID descriptor.
  */
#define aplHIDNumNonStdDescComps_ID      0xf0
#define aplHIDNumNonStdDescComps_Default 0x00

/**
  * @brief The aplHIDNonStdDescCompSpec-i attributes 
  * (where 1 <= i <= aplHIDNumNonStdDescComps) shall specify the actual 
  * HID descriptors supported on the device. Each descriptor 
  * specification shall be formatted as follow:
  * - Descriptor type. Valid values are: 0x22(report) or 0x23(physical). 1 byte
  * - The HID component size in bytes. 2 byte
  * - The HID component identifier. For report descriptor components, 
  * this field indicates the report identifier for this component 
  * (0x80-0xff, 0x00-0x07f is reserved for standard components). 
  * For physical descriptor set components, 
  * this field indicates the physical descriptor set identifier for this 
  * component (0x00 - 0xfe, 0xff is reserved). 1 byte
  * - The contents of the HID descriptor. With max size aplcMaxNonStdDescCompSize
  */
#define aplHIDNonStdDescCompSpec_MIN_ID      0xf1
#define aplHIDNonStdDescCompSpec_MAX_ID      (0xf0 + aplcMaxNonStdDescCompsPerHID)

/* @} \\END ZID RF4CE attributes */

/**
  *@name ZID RF4CE profile report collections
  *@{
  */

/**
  *@brief ID for the mouse standard report descriptor
  */
#define  HID_MOUSE_RPTDESC 0x01

/**
  *@brief ID for the keyboard standard report descriptor
  */
#define  HID_KEYBOARD_RPTDESC 0x02

/**
  *@brief ID for the contact data standard report descriptor
  */
#define  HID_CONTACT_DATA_RPTDESC 0x03

/**
  *@brief ID for the tap gesture standard report descriptor
  */
#define  HID_TAP_GESTURE_RPTDESC 0x04

/**
  *@brief ID for the scroll gesture standard report descriptor
  */
#define  HID_SCROLL_GESTURE_RPTDESC 0x05

/**
  *@brief ID for the pinch gesture standard report descriptor
  */
#define  HID_PINCH_GESTURE_RPTDESC 0x06

/**
  *@brief ID for the rotate gesture standard report descriptor
  */
#define  HID_ROTATE_GESTURE_RPTDESC 0x07

/**
  *@brief ID for the sync standard report descriptor
  */
#define  HID_SYNC_RPTDESC 0x08

/**
  *@brief ID for the touch sensor properties report descriptor
  */
#define HID_TOUCH_SENSOR_RPTDESC 0x09

/**
  *@brief ID for the tap support properties report descriptor
  */
#define HID_TAP_SUPPORT_RPTDESC 0x0A

/* @} \\END ZID RF4CE profile report collections */

/**
  * @brief Value of the attributes commands status
  */
typedef enum  {
  /** Attribute successfully write or read and included
    */
  HID_ATTR_SUCCESS,
  /** Unsupported attribute
    */
  HID_ATTR_UNSUPPORTED,
  /** Illegal request
    */
  HID_ATTR_ILLEGAL_REQ
} ATTR_Status;

/**
  * @brief Structure used to store the device attribute decriptor information
  */
typedef struct deviceAttributeInfoS {
  /** Flag to signal if the attribute is 
    *  a proxy attribute
    */
  bool proxyAttr;
  /** Pairing Reference if the attribute
    *  is a proxy attribute
    */
  uint8_t proxyRef;
  /** Attribute identifier
     */
  uint8_t id;
  /** Attribute length 
    */
  uint16_t length;
   /** Attribute Value (260 Max length for a no standard report descpritor)
    */
  uint8_t *value; 
} deviceAttributeInfo_Type;

/**@brief  Init the Device Attributes with the default value
  *@param  coldStart if the device has executed a cold start 
  *        the function init the default parameters. Otherwise recall the user 
  *        modified device parameter
  *@param  hidDeviceType device type:
  *        - 1 = HID Adapter
  *        - 0 = HID Device
  *@retval None
  */
void ZID_DeviceAttrInit(bool coldStart, uint8_t hidDeviceType);

/**@brief  Get the HID device attribute information
  *@param  value contains the attribute information
  *@retval operation result. Valid value are:
  *        - HID_ATTR_SUCCESS
  *        - HID_ATTR_UNSUPPRTED 
  *        - HID_ATTR_ILLEGAL_REQ
  */
ATTR_Status ZID_GetDeviceAttr(deviceAttributeInfo_Type *value);

/**@brief  Set the HID device attribute information
  *@param  value is an input variable with the attribute value to set
  *@retval operation result. Valid value are:
  *        - HID_ATTR_SUCCESS
  *        - HID_ATTR_UNSUPPRTED 
  *        - HID_ATTR_ILLEGAL_REQ
  */
ATTR_Status ZID_SetDeviceAttr(deviceAttributeInfo_Type *value);

/**@brief  Executes a flush to store the HID device attributes inside 
  *        the non __IO memory. 
  *@NOTE:  This function will be executed only when the HID host/device
  *        is in IDLE state.
  *@param  None
  *@retval None
  */
void ZID_AttrFlush(void);

/**@brief  This function checks if the device is a HID adapter
  *@param  None
  *@retval TRUE if the device is a HID Adapter, FALSE otherwise
  */
bool isHIDAdapter(void);

/**@brief  This function erase the proxy table entry
  *@param  pairRef is the pairing refernece which associated the proxy entry
  *@retval None
  */
void ZID_EraseProxyEntry(uint8_t pairRef);

/**@brief  This function sets the NULL report for the non std descriptor
  *@param  pairRef If the device is an adapter, this parameter is the pairing reference.
  *        If it is a hid class device the paramter shall be 0
  *@param  nullDesc returns the null descriptor
  *@param  descLen returns the null descriptor length
  *@retval operation result. Valid value are:
  *        - HID_ATTR_SUCCESS
  *        - HID_ATTR_UNSUPPRTED 
  *        - HID_ATTR_ILLEGAL_REQ
  */
ATTR_Status ZID_SetNullReport(uint8_t pairRef, uint8_t *nullDesc, uint8_t descLen);

/**@brief  This function gets the NULL report for the non std descriptor
  *@param  pairRef If the device is an adapter, this parameter is the pairing reference.
  *        If it is a hid class device the paramter shall be 0
  *@param  reportId is the report identifier for the asscoiated null report
  *@param  nullDesc returns the null descriptor
  *@param  descLen returns the null descriptor length
  *@retval operation result. Valid value are:
  *        - HID_ATTR_SUCCESS
  *        - HID_ATTR_UNSUPPRTED 
  *        - HID_ATTR_ILLEGAL_REQ
  */
ATTR_Status ZID_GetNullReport(uint8_t pairRef, uint8_t reportId, uint8_t *nullDesc, uint8_t *descLen);

#ifdef ZID_FULL
/**@brief  This function return the flash address for the non standard report descriptor
  *@param  pairRef pairing reference index
  *@param  reportId is the report identifier for the asscoiated null report
  *@param  reportLen is the length of the non standard report descriptor
  *@retval The start flash address for the non standard report descriptor
  */
uint32_t ZID_GetNonStdReportAddr(uint8_t pairRef, uint8_t reportId, uint16_t *reportLen);

/**@brief  This function returns the device current protocol
  *@param  pairRef pairing reference index
  *@param  protocol contain the protocol value:
  *        - 0 Boot
  *        - 1 Report
  *@retval HID_ATTR_ILLEGAL_REQ or HID_ATTR_SUCCESS;
  */
uint32_t ZID_GetCurrentProtocol(uint8_t pairRef, uint8_t *protocol);

/**@brief  This function sets the device current protocol
  *@param  pairRef pairing reference index
  *@param  protocol contain the protocol value:
  *        - 0 Boot
  *        - 1 Report
  *@retval HID_ATTR_ILLEGAL_REQ or HID_ATTR_SUCCESS;
  */
uint32_t ZID_SetCurrentProtocol(uint8_t pairRef, uint8_t protocol);

/**@brief  This function returns the device idle rate
  *@param  pairRef pairing reference index
  *@param  idleRate contains the device idle rate
  *        - 0x00 = 0.004s
  *        - 0x01 = 0.008s
  *        - ....
  *        - 0xff = 1.020s
  *@retval HID_ATTR_ILLEGAL_REQ or HID_ATTR_SUCCESS;
  */
uint32_t ZID_GetIdleRate(uint8_t pairRef, uint8_t *idleRate);

/**@brief  This function sets the device idle rate
  *@param  pairRef pairing reference index
  *@param  idleRate contains the device idle rate
  *        - 0x00 = 0.004s
  *        - 0x01 = 0.008s
  *        - ....
  *        - 0xff = 1.020s
  *@retval HID_ATTR_ILLEGAL_REQ or HID_ATTR_SUCCESS;
  */
uint32_t ZID_SetIdleRate(uint8_t pairRef, uint8_t idleRate);
#endif

#endif // ZID_RF4CE_ATTRIBUTES_H

/** @} // END addtogroup
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/



