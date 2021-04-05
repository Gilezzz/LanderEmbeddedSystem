/**
  ******************************************************************************
  * @file    zid_rf4ce_usb.h
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   ZID USB header file
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

#ifndef ZID_RF4CE_USB_H
#define ZID_RF4CE_USB_H

#include "zid_rf4ce_transport.h"

/**
  *@addtogroup ZID USB API
  *See zid_rf4ce_usb.h for source code.
  *@{
  */

/**
  *@name HID USB descriptor constants
  *@{
  */

/**
  * @brief Device descriptor.
  */
#define USB_DEVICE_DESCRIPTOR       0x01

/**
  * @breif Configuration descriptor.
  */
#define USB_CONFIGURATION_DESCRIPTOR       0x02  

/**
  * @breif Interface descriptor.
  */
#define USB_INTERFACE_DESCRIPTOR  	 0x04  

/**
  * @breif Endpoint descriptor.
  */
#define USB_ENDPOINT_DESCRIPTOR  	 0x05  

/**
  * @brief HID descriptor.
  */
#define USB_HID_DESCRIPTOR  	 0x21  

/**
  * @brief REPORT descriptor.
  */
#define USB_REPORT_DESCRIPTOR  0x22

/**
  * @brief ID for the mouse standard report descriptor
  */
#define  HID_MOUSE_RPTDESC 0x01

/**
  * @brief ID for the keyboard standard report descriptor
  */
#define  HID_KEYBOARD_RPTDESC 0x02

/**
  * @brief ID for the contact data standard report descriptor
  */
#define  HID_CONTACT_DATA_RPTDESC 0x03

/**
  * @brief ID for the tap gesture standard report descriptor
  */
#define  HID_TAP_GESTURE_RPTDESC 0x04

/**
  * @brief ID for the scroll gesture standard report descriptor
  */
#define  HID_SCROLL_GESTURE_RPTDESC 0x05

/**
  * @brief ID for the pinch gesture standard report descriptor
  */
#define  HID_PINCH_GESTURE_RPTDESC 0x06

/**
  * @brief ID for the rotate gesture standard report descriptor
  */
#define  HID_ROTATE_GESTURE_RPTDESC 0x07

/**
  * @brief ID for the sync standard report descriptor
  */
#define  HID_SYNC_RPTDESC 0x08

/**
  * @brief ID for the touch sensor standard report descriptor
  */
#define  HID_TOUCH_SENSOR_RPTDESC 0x09

/**
  * @brief ID for the tap support standard report descriptor
  */
#define  HID_TAP_SUPPORT_RPTDESC 0x0A

/* @} \\END HID USB descriptor constants */

/**
  * @name HID USB Class Driver return values
  *@{
  */

#define CLASS_DRIVER_SUCCESS 0x0
#define CLASS_DRIVER_INVALID_PARAMETER 0x01
#define CLASS_DRIVER_HOST_FULL 0x02

/* @} \\END HID USB Class Driver return values */

/**
  * @name HID USB Class Driver typedefs
  *@{
  */
/**
  * @brief Structure used for USB device attributes. See the explanation 
  * of the associated constants
  */
typedef struct deviceAttrbiuteS {
  uint16_t aplHIDVendorId;
  uint16_t aplHIDProductId;
  uint16_t aplHIDDeviceReleaseNumber;
  uint8_t  aplHIDNumEndpoints;
  uint8_t  aplHIDDeviceSubclass;
  uint8_t  aplHIDProtocolCode;
  uint8_t  aplHIDPollInterval;
  uint16_t aplHIDParserVersion;
  uint8_t  aplHIDCountryCode;
  uint8_t  aplHIDNumStdDescComps;
  uint8_t  aplHIDNumNonStdDescComps;
} deviceAttributeType;

/* @} \\END HID USB Class Driver typedefs */

/**
  * @name HID USB Class Driver function user interface
  *@{
  */

/**@brief  This function send a RESET command to the USB Class Driver.
  *        This command configures reset the USB host configuring it like virtual com.
  *@param  usbEnumeration after host reset if TRUE enumerates the USB device, if 
  *        FALSE doesn't enumerate the USB device
  *@retval None 
  */
uint32_t HID_ClassDriver_HostReset(boolean usbEnumearion);

/**@brief  This function init the USB Class Driver for the device
  *@param  None
  *@retval None
  */
void HID_ClassDriver_USBInit(void);

/**@brief  This function sets the device attributes necessary to build all the 
  *        USB descriptors.
  *@param  devAttr contains all the device attributes
  *@retval The status of the command. The values are:
  *        - CLASS_DRIVER_SUCCESS
  *        - CLASS_DRIVER_INVALID_PARAMETER
  */
uint32_t HID_ClassDriver_SetUSBDescAttribute(deviceAttributeType *devAttr);

/**@brief  This function sets the USB report descriptor for the device
  *@param  stdRpt is a flag to signal if you want use a standard descriptor or not
  *@param  rptDesc this list contains the non standard report descriptor, or the IDs 
  *        of the standard report descriptors.
  *@param  rptLen list length
  *@retval The status of the command. The values are:
  *        - CLASS_DRIVER_SUCCESS
  *        - CLASS_DRIVER_INVALID_PARAMETER
  */
uint32_t HID_ClassDriver_SetUSBRptDesc(boolean stdRpt, uint8_t *rptDesc, uint16_t rptLen);

/**@brief  This function signals the USB Class Driver that you want notify a new device
  *@param  None
  *@retval None
  */
void HID_ClassDriver_NotifyUSBDevice(void);

/**@brief  This function returns the standard USB report descriptor managed from this software
  *@param  rptId ID of the standard report device
  *@param  rptDesc will contain the value of the standard report descriptor
  *@param  length standard report descriptor length
  *@retval None
  */
void HID_ClassDriver_GetUSBSTDRptDesc(uint8_t rptId, uint8_t *rptDesc, uint8_t *length);

/**@brief  This function calls the transport layer to listen if a new HID USB packet is arrvied
  *@param  None
  *@retval None
  */
void HID_ClassDriver_USBTick(void);

/**@brief  This function notifies the report data
  *        to the USB Class Driver
  *@param  deviceId device identifier
  *@param  reportId report identifier
  *@param  report contains the report data information
  *@param  report data length
  *@retval None 
  */
void HID_ClassDriver_USBRptDataNotify(uint8_t deviceId, uint8_t reportId, uint8_t *report, uint8_t length);

/**@brief  This function returns the byte received from serial input that haven't
  *        HID packet format
  *@param  packet list of bytes received
  *@param  length list length
  *@retval TRUE if some data is present, FALSE otherwise
  */
boolean HID_ClassDriver_SerialData(uint8_t *packet, uint8_t *length);
 
/**@brief  This function notifies the USB class driver request to the device
  *@param  commandCode USB command code notification
  *@param  packet contains the information of the command (first byte si always the
  *        device number)
  *@retval None
  */
extern void HID_ClassDriver_USBCmdNotification(uint8_t commandCode, uint8_t *packet);


/**@brief  This function Send the command response for certain USB request
  *@param  cmdReq request from USB class driver
  *@param  numDevice USB device number
  *@param  packet contains the information of the command response
  *@param  packetLen contains the length of the packet
  *@retval None
  */
void HID_ClassDriver_USBCommandRsp(uint8_t cmdReq, uint8_t numDevice, uint8_t *packet, uint8_t packetLen);

/* @} \\END ZID USB Class Driver function user interface */

/** @} // END addtogroup
 */

/*********** Functions to exchange information with the transport layer *****************/

/**@brief  This function flushes the serial input (not HID messages) read from the Host device
  *@param  packet
  *@param  packet lenght
  *@retval None
  */
void hidSerialInputFlush(uint8_t *packet, uint8_t length);

/**@brief  This function is called from the transport layer when a HID USB packet is received
  *@param  Packet
  *@param  Packet lenght
  *@retval None
  */
void hidCommandInterpreter(uint8_t *packet, uint16_t length);

#endif /* ZID_RF4CE_USB_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
