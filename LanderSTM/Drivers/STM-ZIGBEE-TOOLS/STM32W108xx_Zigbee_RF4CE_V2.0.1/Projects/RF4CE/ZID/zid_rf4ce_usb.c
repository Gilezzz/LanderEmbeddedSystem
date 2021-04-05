/**
  ******************************************************************************
  * @file    zid_rf4ce_usb.c
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   This file provides the HID RF4CE usb interface with HID
  *          class driver. This file manage the serial communication
  *          and descriptor management 
  * @verbatim
  *
 ===============================================================================
                             ##### Note #####
 ===============================================================================
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
 * AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
 * CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
 * INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.

    @endverbatim
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
#include PLATFORM_HEADER
#include "hal.h"
#include "error.h"
#include <stdio.h>
#include <string.h>
#include "zid_rf4ce_usb.h"

/* Private define ------------------------------------------------------------*/
#define MAX_NUM_DEVICES      5 /* Max number of devices managed by the host */ 
#define MAX_NUM_NON_STD_DESC 4  /* Max number of non standard report descriptor */
#define HID_PROFILE_ATTR_NUM 11 /* Number of HID profile attributes */
#define MAX_NUM_STD_PROFILE  12 /* Max number of standard profile report descriptor */
#define MAX_STD_DESC_LEN     79 

#define MAX_BUFFER_LENGTH 255 /* Flush Buffer length ??? */

#define DEVICE_DESC {0x12,  USB_DEVICE_DESCRIPTOR,          \
                     0x00,  0x02, 0x00,  0x00, 0x00,  0x40, \
		     0x83,  0x04, 0x41,  0x57, 0x00,  0x01, \
                     0x00,  0x00, 0x00,  0x01}

#define CONFIGURATION_DESC {0x09, USB_CONFIGURATION_DESCRIPTOR, \
			    0x00, 0x00, 0x01, 0x01, 0x00, 0xc0, \
			    0x00}

#define INTERFACE_DESC {0x09, USB_INTERFACE_DESCRIPTOR,     \
			0x00, 0x00, 0x01, 0x03, 0x00, 0x00, \
			0x00}

#define ENDPOINT_DESC {0x07, USB_ENDPOINT_DESCRIPTOR,  \
		       0x81, 0x03, 0x40, 0x00, 0x00}

#define HID_DESC {0x09,USB_HID_DESCRIPTOR, 0x11, 0x01, 0x00, 0x01, \
		  USB_REPORT_DESCRIPTOR, 0x00, 0x00}

/* Mouse report descriptor */
#define  MOUSE_RPT_DESC {0x05, 0x01, 0x09, 0x02, 0xA1, 0x01, 0x85, 0x01, \
                         0x09, 0x01, 0xA1, 0x00, 0x05, 0x09, 0x19, 0x01, \
                         0x29, 0x03, 0x15, 0x00, 0x25, 0x01, 0x75, 0x01, \
                         0x95, 0x03, 0x81, 0x02, 0x75, 0x05, 0x95, 0x01, \
                         0x81, 0x01, 0x05, 0x01, 0x09, 0x30, 0x09, 0x31, \
                         0x15, 0x81, 0x25, 0x7F, 0x75, 0x08, 0x95, 0x02, \
                         0x81, 0x06, 0xC0, 0xC0}

/* Keyboard report descriptor */
#define KEYBOARD_RPT_DESC {0x05, 0x01, 0x09, 0x06, 0xA1, 0x01, 0x85, 0x02, \
                           0xA1, 0x00, 0x05, 0x07, 0x19, 0xE0, 0x29, 0xE7, \
                           0x15, 0x00, 0x25, 0x01, 0x75, 0x01, 0x95, 0x08, \
                           0x81, 0x02, 0x75, 0x08, 0x95, 0x01, 0x81, 0x01, \
                           0x75, 0x01, 0x95, 0x05, 0x05, 0x08, 0x19, 0x01, \
                           0x29, 0x05, 0x91, 0x02, 0x75, 0x03, 0x95, 0x01, \
                           0x91, 0x01, 0x75, 0x08, 0x95, 0x06, 0x15, 0x00, \
                           0x25, 0x65, 0x05, 0x07, 0x19, 0x00, 0x29, 0x65, \
                           0x81, 0x00, 0xC0, 0xC0}

/* Contact data report descriptor */
#define CONTACT_DATA_RPT_DESC {0x05, 0x0D, 0x09, 0x04, 0xA1, 0x01, 0x85, 0x03, \
                               0xA1, 0x00, 0x09, 0x00, 0x15, 0x00, 0x25, 0x0F, \
                               0x75, 0x04, 0x95, 0x02, 0x81, 0x02, 0x09, 0x00, \
                               0x25, 0x03, 0x75, 0x02, 0x95, 0x01, 0x81, 0x02, \
                               0x75, 0x06, 0x81, 0x01, 0x09, 0x00, 0x26, 0xFF, \
                               0x00, 0x75, 0x08, 0x81, 0x02, 0x09, 0x00, 0x25, \
                               0x7F, 0x81, 0x02, 0x05, 0x01, 0x09, 0x30, 0x09, \
                               0x31, 0x26, 0xFF, 0x0F, 0x75, 0x0C, 0x95, 0x02, \
                               0x81, 0x02, 0x09, 0x00, 0x26, 0xFF, 0x1F, 0x75, \
                               0x10, 0x95, 0x02, 0x81, 0x02, 0xC0, 0xC0}

/* Tap gesture report descriptor */
#define TAP_GESTURE_RPT_DESC {0x05, 0x0D, 0x09, 0x04, 0xA1, 0x01, 0x85, 0x04, \
                              0xA1, 0x00, 0x09, 0x00, 0x15, 0x00, 0x25, 0x07, \
                              0x75, 0x03, 0x95, 0x01, 0x81, 0x02, 0x09, 0x00, \
                              0x25, 0x1F, 0x75, 0x05, 0x81, 0x02, 0x75, 0x08, \
                              0x81, 0x01, 0x05, 0x01, 0x09, 0x30, 0x09, 0x31, \
                              0x26, 0xFF, 0x0F, 0x75, 0x0C, 0x95, 0x02, 0x81, \
                              0x02, 0xC0, 0xC0}

/* Scroll gesture report descriptor */
#define SCROLL_GESTURE_RPT_DESC {0x05, 0x0D, 0x09, 0x04, 0xA1, 0x01, 0x85, 0x05, \
                                 0xA1, 0x00, 0x09, 0x00, 0x15, 0x00, 0x25, 0x07, \
                                 0x75, 0x03, 0x95, 0x01, 0x81, 0x02, 0x09, 0x00, \
                                 0x25, 0x1F, 0x75, 0x05, 0x81, 0x02, 0x75, 0x08, \
                                 0x81, 0x01, 0x09, 0x00, 0x25, 0x07, 0x75, 0x03, \
                                 0x81, 0x02, 0x75, 0x01, 0x81, 0x01, 0x09, 0x00, \
                                 0x26, 0xFF, 0x0F, 0x75, 0x0C, 0x81, 0x02, 0xC0, \
                                 0xC0}

/* Pinch gesture report descriptor */
#define PINCH_GESTURE_RPT_DESC {0x05, 0x0D, 0x09, 0x04, 0xA1, 0x01, 0x85, 0x06, \
                                0xA1, 0x00, 0x09, 0x00, 0x09, 0x22, 0x15, 0x00, \
                                0x25, 0x01, 0x75, 0x01, 0x95, 0x02, 0x81, 0x02, \
                                0x75, 0x06, 0x95, 0x01, 0x81, 0x01, 0x09, 0x00, \
                                0x26, 0xFF, 0x0F, 0x75, 0x0C, 0x81, 0x02, 0x75, \
                                0x04, 0x81, 0x01, 0x05, 0x01, 0x09, 0x30, 0x09, \
                                0x31, 0x75, 0x0C, 0x95, 0x02, 0x81, 0x02, 0xC0, \
                                0xC0}

/* Rotation gesture report descriptor */
#define ROTATION_GESTURE_RPT_DESC {0x05, 0x0D, 0x09, 0x04, 0xA1, 0x01, 0x85, 0x07, \
                                   0xA1, 0x00, 0x09, 0x00, 0x09, 0x22, 0x15, 0x00, \
                                   0x25, 0x01, 0x75, 0x01, 0x95, 0x02, 0x81, 0x02, \
                                   0x75, 0x06, 0x95, 0x01, 0x81, 0x01, 0x09, 0x00, \
                                   0x26, 0xFF, 0x00, 0x75, 0x08, 0x81, 0x02, 0xC0, \
                                   0xC0}

/* Sync report descriptor */
#define SYNC_RPT_DESC {0x05, 0x0D, 0x09, 0x04, 0xA1, 0x01, 0x85, 0x08, \
                       0xA1, 0x01, 0x09, 0x00, 0x15, 0x00, 0x25, 0x0F, \
                       0x75, 0x04, 0x95, 0x01, 0x81, 0x02, 0x09, 0x00, \
                       0x25, 0x01, 0x75, 0x01, 0x81, 0x02, 0x75, 0x03, \
                       0x81, 0x01, 0xC0, 0xC0}

/* Touch Sensor report descriptor */
#define TOUCH_SENSOR_RPT_DESC {0x05, 0x0D, 0x09, 0x04, 0xA1, 0x01, 0x85, 0x09, \
                               0xA1, 0x02, 0x09, 0x00, 0x15, 0x00, 0x25, 0x0F, \
                               0x75, 0x04, 0x95, 0x01, 0x81, 0x02, 0x09, 0x00, \
                               0x25, 0x03, 0x75, 0x02, 0x81, 0x02, 0x09, 0x00, \
                               0x25, 0x01, 0x75, 0x01, 0x95, 0x02, 0x81, 0x02, \
                               0x09, 0x00, 0x25, 0x7F, 0x75, 0x08, 0x81, 0x02, \
                               0x09, 0x00, 0x15, 0x01, 0x26, 0xFF, 0x0F, 0x75, \
                               0x12, 0x81, 0x02, 0x09, 0x00, 0x15, 0x00, 0x25, \
                               0x07, 0x75, 0x04, 0x95, 0x01, 0x81, 0x02, 0x81, \
                               0x01, 0xC0, 0xC0}

/* Tap support properties report descriptor */
#define TAP_SUPPORT_RPT_DESC {0x05, 0x0D, 0x09, 0x04, 0xA1, 0x01, 0x85, 0x0A, \
                              0xA1, 0x00, 0x09, 0x00, 0x15, 0x00, 0x25, 0x01, \
                              0x75, 0x01, 0x95, 0x04, 0x81, 0x02, 0x75, 0x04, \
                              0x95, 0x01, 0x81, 0x01, 0x75, 0x08, 0x95, 0x03, \
                              0x81, 0x01, 0xC0, 0xC0}

/* Private typedef -----------------------------------------------------------*/
typedef struct classDriverContextS {
  uint32_t status;
  int8_t numDevices;
  deviceAttributeType devAttr[MAX_NUM_DEVICES];
  uint8_t devStdDesc[MAX_NUM_DEVICES][MAX_NUM_STD_PROFILE];
  uint16_t devNonStdDescLen[MAX_NUM_DEVICES][MAX_NUM_NON_STD_DESC];
  uint32_t devNonStdDesc[MAX_NUM_DEVICES][MAX_NUM_NON_STD_DESC];
} classDriverContextType;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t deviceDescriptor[] = DEVICE_DESC;
static uint8_t configurationDescriptor[] = CONFIGURATION_DESC;
static uint8_t interfaceDescriptor[] = INTERFACE_DESC;
static uint8_t endpointDescriptor[] = ENDPOINT_DESC;
static uint8_t hidDescriptor[] = HID_DESC; 
static classDriverContextType classDriverContext;
static boolean dataToFlush;
static uint8_t bufferLength;
static uint8_t bufferToFlush[MAX_BUFFER_LENGTH];

/* Private function prototypes -----------------------------------------------*/
static void buildDeviceDesc(uint8_t *deviceDescriptor, uint8_t deviceiId);
static void buildConfigurationDesc(uint8_t *configurationDescriptor, uint8_t deviceId);
static void buildInterfaceDesc(uint8_t *interfaceDescriptor, uint8_t deviceId);
static void buildHIDDesc(uint8_t *hidDescriptor, uint8_t deviceId);
static void buildEndpointDesc(uint8_t *endpointDescriptor, uint8_t deviceId);
static void reportDescNotification(uint8_t deviceId);
static void descriptorInterpreter(uint8_t *packet, uint8_t length);

/* Public variables ----------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**@brief  This function builds the device descriptor
  *@param  device descriptor
  *@param  device identity
  *@retval None
  */
static void buildDeviceDesc(uint8_t *deviceDescriptor, uint8_t deviceId)
{
  uint16_t value;

  value = classDriverContext.devAttr[deviceId].aplHIDVendorId;
  deviceDescriptor[8] = value & 0x00ff;
  deviceDescriptor[9] = (value>>8) & 0x00ff;

  value = classDriverContext.devAttr[deviceId].aplHIDProductId;
  deviceDescriptor[10] = value & 0x00ff;
  deviceDescriptor[11] = (value>>8) & 0x00ff;

  value = classDriverContext.devAttr[deviceId].aplHIDDeviceReleaseNumber;
  deviceDescriptor[12] = value & 0x00ff;
  deviceDescriptor[13] = (value>>8) & 0x00ff;
}

/**@brief  This function builds the configuration for the descriptor
  *@param  configuration descriptor
  *@param  device identity
  *@retval None
  */
static void buildConfigurationDesc(uint8_t *configurationDescriptor, uint8_t deviceId)
{
  uint16_t length = 34; /* Configuration + Interface + 1 Endpoint + HID with 1 report desc */

  configurationDescriptor[2] = length & 0x00ff;
  configurationDescriptor[3] = (length>>8) & 0x00ff;
}

/**@brief  This function builds the interface descriptor
  *@param  interface descriptor
  *@param  device identity
  *@retval None
  */
static void buildInterfaceDesc(uint8_t *interfaceDescriptor, uint8_t deviceId)
{
  interfaceDescriptor[4] = classDriverContext.devAttr[deviceId].aplHIDNumEndpoints;
  interfaceDescriptor[6] = classDriverContext.devAttr[deviceId].aplHIDDeviceSubclass;
  interfaceDescriptor[7] = classDriverContext.devAttr[deviceId].aplHIDProtocolCode;
}

/**@brief  This function builds the hid descriptor
  *@param  hid descriptor
  *@param  device identity
  *@retval None
  */
static void buildHIDDesc(uint8_t *hidDescriptor, uint8_t deviceId)
{
  uint16_t value;
  uint8_t numDesc, numStdDesc, i, descLen, stdDescId;
  uint16_t totDescLen, nonStdDescLen;
  uint8_t desc[MAX_STD_DESC_LEN];

  value = classDriverContext.devAttr[deviceId].aplHIDParserVersion;
  hidDescriptor[2] = value & 0x00ff;
  hidDescriptor[3] = (value>>8) & 0x00ff;

  value = classDriverContext.devAttr[deviceId].aplHIDCountryCode;
  hidDescriptor[4] = value & 0x00ff;
  
  numDesc = classDriverContext.devAttr[deviceId].aplHIDNumNonStdDescComps;
  numStdDesc = classDriverContext.devAttr[deviceId].aplHIDNumStdDescComps;

  hidDescriptor[5] = 0x01; /* Only one cumulative descriptor */
  hidDescriptor[0] = 0x09;

  totDescLen = 0;
  for (i=0; i<numStdDesc; i++) {
    stdDescId = classDriverContext.devStdDesc[deviceId][i];
    HID_ClassDriver_GetUSBSTDRptDesc(stdDescId, desc, &descLen);
    totDescLen += descLen;
  }

  for (i=0; i<numDesc; i++) {
    nonStdDescLen =  classDriverContext.devNonStdDescLen[deviceId][i];
    totDescLen += nonStdDescLen;
  }

  hidDescriptor[6] = 0x22;
  hidDescriptor[7] = totDescLen & 0x00ff;
  hidDescriptor[8] = (totDescLen>>8) & 0x00ff;;

}

/**@brief  This function builds the endpoint descriptor
  *@param  endpoint descriptor
  *@param  device identity
  *@retval None
  */
static void buildEndpointDesc(uint8_t *endpointDescriptor, uint8_t deviceId)
{
  endpointDescriptor[6] = classDriverContext.devAttr[deviceId].aplHIDPollInterval;
}

/**@brief  This function is called for the report descriptor notification
  *@param  device descriptor
  *@retval None
  */
static void reportDescNotification(uint8_t deviceId)
{
  uint8_t numDesc, numStdDesc, stdDescId, i, descLen;
  uint16_t nonStdDescLen;
  uint8_t desc[MAX_STD_DESC_LEN];
  uint8_t *nonStdDesc;
    
  numDesc = classDriverContext.devAttr[deviceId].aplHIDNumNonStdDescComps;
  numStdDesc = classDriverContext.devAttr[deviceId].aplHIDNumStdDescComps;

  /* Standard descriptor */
  for (i=0; i<numStdDesc; i++) {
    stdDescId = classDriverContext.devStdDesc[deviceId][i];
    HID_ClassDriver_GetUSBSTDRptDesc(stdDescId, desc, &descLen);
    {
      uint8_t ii;
      printf("Packet to send with length=%d :", descLen); 
      for (ii=0; ii<descLen; ii++) {
	printf(" 0x%02x", desc[ii]);
      }
      printf("\r\n");
    }

    sendTransportPacket(GET_DESCRIPTOR_RSP_TRANS_CODE, desc, descLen); 
  }
  
  /* Non standard descriptor */
  for (i=0; i<numDesc; i++) {
    nonStdDesc = (uint8_t*)classDriverContext.devNonStdDesc[deviceId][i];
    nonStdDescLen = classDriverContext.devNonStdDescLen[deviceId][i];
    {
      uint16_t ii;
      printf("Packet to send with length=%d :", nonStdDescLen); 
      for (ii=0; ii<nonStdDescLen; ii++) {
	printf(" 0x%02x", nonStdDesc[ii]);
      }
      printf("\r\n");
    }

    sendTransportPacket(GET_DESCRIPTOR_RSP_TRANS_CODE, nonStdDesc, nonStdDescLen); 
  }
}

/**@brief  This function is called for descriptor interpreter
  *@param  Packet
  *@param  Packet lenght
  *@retval None
  */
static void descriptorInterpreter(uint8_t *packet, uint8_t length)
{
  uint8_t deviceId, descLength = 0, descType;
  uint8_t *desc = 0;

  descType = packet[0];
  deviceId = packet[1];
  printf ("!!!!!!!!DESCRIPTOR REQ 0x%02x 0x%02x\r\n", descType, deviceId);
  switch(descType) {
  case USB_DEVICE_DESCRIPTOR:
    printf ("!!!!!!!!DEVICE_DESC\r\n");
    buildDeviceDesc(deviceDescriptor, deviceId);
    desc = deviceDescriptor;
    descLength = deviceDescriptor[0];
    break;
  case USB_CONFIGURATION_DESCRIPTOR:
    printf ("!!!!!!!!CONFIG_DESC\r\n");
    buildConfigurationDesc(configurationDescriptor, deviceId);
    desc = configurationDescriptor;
    descLength = configurationDescriptor[0];
    break;
  case USB_INTERFACE_DESCRIPTOR:
    printf ("!!!!!!!!INTERFACE_DESC\r\n");
    buildInterfaceDesc(interfaceDescriptor, deviceId);
    desc = interfaceDescriptor;
    descLength = interfaceDescriptor[0];
    break;
  case USB_ENDPOINT_DESCRIPTOR:
    printf ("!!!!!!!!ENDP_DESC\r\n");
    buildEndpointDesc(endpointDescriptor, deviceId);
    desc = endpointDescriptor;
    descLength = endpointDescriptor[0];
    break;
  case USB_HID_DESCRIPTOR:
    printf ("!!!!!!!!HID_DESC\r\n");
    buildHIDDesc(hidDescriptor, deviceId);
    desc = hidDescriptor;
    descLength = hidDescriptor[0];
    break;
  case USB_REPORT_DESCRIPTOR:
    printf ("!!!!!!!!REPORT_DESC\r\n");
    reportDescNotification(deviceId);
    break;
  }

  if (descType != USB_REPORT_DESCRIPTOR) {
    uint8_t i;
    printf("Packet to send with length=%d :", descLength); 
    for (i=0; i<descLength; i++) {
      printf(" 0x%02x", desc[i]);
    }
    printf("\r\n");

    sendTransportPacket(GET_DESCRIPTOR_RSP_TRANS_CODE, desc, descLength);
  }
}

/* Public functions ----------------------------------------------------------*/

/**@brief  This function send a RESET command to the USB Class Driver.
  *        This command configures reset the USB host configuring it like virtual com.
  *@param  usbEnumeration after host reset if TRUE enumerates the USB device, if 
  *        FALSE doesn't enumerate the USB device
  *@retval None 
  */
uint32_t HID_ClassDriver_HostReset(boolean usbEnumearion)
{
  sendTransportPacket(RESET_TRANS_CODE, &usbEnumearion, 1);
  return CLASS_DRIVER_SUCCESS;
}

/**@brief  This function sets the device attributes necessary to build all the 
  *        USB descriptors.
  *@param  devAttr contains all the device attributes
  *@retval The status of the command. The values are:
  *        - CLASS_DRIVER_SUCCESS
  *        - CLASS_DRIVER_INVALID_PARAMETER
  */
uint32_t HID_ClassDriver_SetUSBDescAttribute(deviceAttributeType *devAttr)
{
  uint8_t index;

  if (classDriverContext.numDevices == (MAX_NUM_DEVICES-1))
    return CLASS_DRIVER_HOST_FULL;

  if ((devAttr->aplHIDNumStdDescComps > MAX_NUM_STD_PROFILE) ||
      (devAttr->aplHIDNumNonStdDescComps > MAX_NUM_NON_STD_DESC))
    return CLASS_DRIVER_INVALID_PARAMETER;

  classDriverContext.numDevices++;
  index = classDriverContext.numDevices;
  memcpy(&classDriverContext.devAttr[index], devAttr, sizeof(deviceAttributeType));

  return CLASS_DRIVER_SUCCESS;
}

/**@brief  This function sets the USB report descriptor for the device
  *@param  stdRpt is a flag to signal if you want use a standard descriptor or not
  *@param  rptDesc this list contains the non standard report descriptor, or the IDs 
  *        of the standard report descriptors.
  *@param  rptLen list length
  *@retval The status of the command. The values are:
  *        - CLASS_DRIVER_SUCCESS
  *        - CLASS_DRIVER_INVALID_PARAMETER
  */
uint32_t HID_ClassDriver_SetUSBRptDesc(boolean stdRpt, uint8_t *rptDesc, uint16_t rptLen)
{
  uint8_t index;
  
  index = 0;
  
  if (stdRpt) {
    if (rptLen > MAX_NUM_STD_PROFILE)
      return CLASS_DRIVER_INVALID_PARAMETER;
    memcpy(classDriverContext.devStdDesc[classDriverContext.numDevices], rptDesc, rptLen);
  } else {
    while((classDriverContext.devNonStdDescLen[classDriverContext.numDevices][index] != 0) && 
	  (index < MAX_NUM_DEVICES)) 
      index++;

    if (index == MAX_NUM_NON_STD_DESC)
      return CLASS_DRIVER_INVALID_PARAMETER;

    classDriverContext.devNonStdDesc[classDriverContext.numDevices][index] = (uint32_t)rptDesc;
    classDriverContext.devNonStdDescLen[classDriverContext.numDevices][index] = rptLen;
  }
  
  return CLASS_DRIVER_SUCCESS;
}

/**@brief  This function init the USB Class Driver for the device
  *@param  None
  *@retval None
  */
void HID_ClassDriver_USBInit(void)
{
  memset(&classDriverContext, 0, sizeof(classDriverContextType));
  classDriverContext.numDevices = -1;
  dataToFlush = FALSE;
  bufferLength = 0;
}

/**@brief  This function signals the USB Class Driver that you want notify a new device
  *@param  None
  *@retval None
  */
void HID_ClassDriver_NotifyUSBDevice(void)
{
  uint8_t dummy;

  dummy = 0;
  sendTransportPacket(ADD_HID_DEVICE_TRANS_CODE, &dummy, 0);
}

/**@brief  This function is called from the transport layer when a HID USB packet is received
  *@param  Packet
  *@param  Packet lenght
  *@retval None
  */
void hidCommandInterpreter(uint8_t *packet, uint16_t length)
{
  uint8_t commandCode;
  uint8_t *payload;
  uint8_t payloadLen;

  commandCode = packet[0];
  payload = &packet[1];
  payloadLen = length - 1;

  switch (commandCode) {
  case GET_DESCRIPTOR_TRANS_CODE: 
    {
      descriptorInterpreter(payload, payloadLen);
    }
    break;
  case GET_REPORT_TRANS_CODE:
    {
      HID_ClassDriver_USBCmdNotification(GET_REPORT_TRANS_CODE, payload);
    }
    break;
  case SET_REPORT_TRANS_CODE:
    {
      HID_ClassDriver_USBCmdNotification(SET_REPORT_TRANS_CODE, payload);
    }
    break;
  case GET_IDLE_TRANS_CODE:
    {
      HID_ClassDriver_USBCmdNotification(GET_IDLE_TRANS_CODE, payload);
    }
    break;
  case SET_IDLE_TRANS_CODE:
    {
      HID_ClassDriver_USBCmdNotification(SET_IDLE_TRANS_CODE, payload);
    }
    break;
  case GET_PROTOCOL_TRANS_CODE:
    {
      HID_ClassDriver_USBCmdNotification(GET_PROTOCOL_TRANS_CODE, payload);
    }
    break;
  case SET_PROTOCOL_TRANS_CODE:
    {
      HID_ClassDriver_USBCmdNotification(SET_PROTOCOL_TRANS_CODE, payload);
    }
    break;
  case CONFIGURATION_COMPLETE_TRANS_CODE:
    {
      HID_ClassDriver_USBCmdNotification(CONFIGURATION_COMPLETE_TRANS_CODE, payload);
    }
    break;
  case HOST_DEVICE_FULL_TRANS_CODE:
    {
      HID_ClassDriver_USBCmdNotification(HOST_DEVICE_FULL_TRANS_CODE, payload);
    }
    break;
  case GET_NUMBER_INTERFACE_TRANS_CODE:
    {
      uint8_t packet;
      
      packet = classDriverContext.numDevices + 1;
      sendTransportPacket(GET_NUMBER_INTERFACE_RSP_TRANS_CODE, &packet, 1);
    }
    printf ("!!!!!!!!NUMBER_INTERFACE\r\n");
    break;
  case GET_NUMBER_REPORT_DESC_TRANS_CODE:
    {
      uint8_t totDesc, deviceId;

      deviceId = payload[0];
      totDesc = classDriverContext.devAttr[deviceId].aplHIDNumNonStdDescComps + classDriverContext.devAttr[deviceId].aplHIDNumStdDescComps;
      sendTransportPacket(GET_NUMBER_REPORT_DESC_RSP_TRANS_CODE, &totDesc, 1); 
    }
    printf("!!!!!!!!NUMBER_REPORT_DESC\r\n");
    break;
  }
}

/**@brief  This function returns the standard USB report descriptor managed from this software
  *@param  rptId ID of the standard report device
  *@param  rptDesc will contain the value of the standard report descriptor
  *@param  length standard report descriptor length
  *@retval None
  */
void HID_ClassDriver_USBTick(void)
{
  transportInterpreter();
}

/**@brief  This function notifies the report data
  *        to the USB Class Driver
  *@param  deviceId device identifier
  *@param  reportId report identifier
  *@param  report contains the report data information
  *@param  report data length
  *@retval None 
  */
void HID_ClassDriver_USBRptDataNotify(uint8_t deviceId, uint8_t reportId, uint8_t *report, uint8_t length)
{
  uint8_t packet[66];

  packet[0] = deviceId;
  packet[1] = reportId;
  memcpy(&packet[2], report, length);
  length += 2;
  sendTransportPacket(REPORT_DATA_TRANS_CODE, packet, length);
}

/**@brief  This function returns the standard USB report descriptor managed from this software
  *@param  rptId ID of the standard report device
  *@param  rptDesc will contain the value of the standard report descriptor
  *@param  length standard report descriptor length
  *@retval None
  */
void HID_ClassDriver_GetUSBSTDRptDesc(uint8_t rptId, uint8_t *rptDesc, uint8_t *length)
{
 
  switch(rptId) {
  case HID_MOUSE_RPTDESC:
    {
      uint8_t mouseRptDesc[] = MOUSE_RPT_DESC; 
      *length = sizeof(mouseRptDesc);
      memcpy(rptDesc, mouseRptDesc, *length);
    }
    break;
  case HID_KEYBOARD_RPTDESC:
    {
      uint8_t keyboardRptDesc[] = KEYBOARD_RPT_DESC;
      *length = sizeof(keyboardRptDesc);
      memcpy(rptDesc, keyboardRptDesc, *length);
    }
    break;
  case  HID_CONTACT_DATA_RPTDESC:
    {
      uint8_t contactDataRptDesc[] = CONTACT_DATA_RPT_DESC;
      *length = sizeof(contactDataRptDesc);
      memcpy(rptDesc, contactDataRptDesc, *length);
    }
    break;
  case  HID_TAP_GESTURE_RPTDESC:
    {
      uint8_t tapGestureRptDesc[] = TAP_GESTURE_RPT_DESC;
      *length = sizeof(tapGestureRptDesc);
      memcpy(rptDesc, tapGestureRptDesc, *length);
    }
    break;
  case  HID_SCROLL_GESTURE_RPTDESC:
    {
      uint8_t scrollGestureRptDesc[] = SCROLL_GESTURE_RPT_DESC;
      *length = sizeof(scrollGestureRptDesc);
      memcpy(rptDesc, scrollGestureRptDesc, *length);
    }
    break;
  case  HID_PINCH_GESTURE_RPTDESC:
    {
      uint8_t pinchGestureRptDesc[] = PINCH_GESTURE_RPT_DESC;
      *length = sizeof(pinchGestureRptDesc);
      memcpy(rptDesc, pinchGestureRptDesc, *length);
    }
    break;
  case  HID_ROTATE_GESTURE_RPTDESC:
    {
      uint8_t rotationGestureRptDesc[] = ROTATION_GESTURE_RPT_DESC;
      *length = sizeof(rotationGestureRptDesc);
      memcpy(rptDesc, rotationGestureRptDesc, *length);
    }
    break;
  case  HID_SYNC_RPTDESC:
    {
      uint8_t syncRptDesc[] = SYNC_RPT_DESC;
      *length = sizeof(syncRptDesc);
      memcpy(rptDesc, syncRptDesc, *length);
    }
    break;
  case HID_TOUCH_SENSOR_RPTDESC:
    {
      uint8_t touchRptDesc[] = TOUCH_SENSOR_RPT_DESC;
      *length = sizeof(touchRptDesc);
      memcpy(rptDesc, touchRptDesc, *length);
    }
    break;
  case HID_TAP_SUPPORT_RPTDESC:
    {
      uint8_t tapRptDesc[] = TAP_SUPPORT_RPT_DESC;
      *length = sizeof(tapRptDesc);
      memcpy(rptDesc, tapRptDesc, *length);
    }
    break;
  default:
    /* Error wrong id */
    break;
  }
}

/**@brief  This function flushes the serial input (not HID messages) read from the Host device
  *@param  packet
  *@param  packet lenght
  *@retval None
  */
void hidSerialInputFlush(uint8_t *packet, uint8_t length)
{
  dataToFlush = TRUE;
  if ((bufferLength+length) > MAX_BUFFER_LENGTH) {
    bufferLength = 0;
  }
  if (length > 1)
    memcpy(&bufferToFlush[bufferLength], packet, length);
  else
    bufferToFlush[0] = packet[0];
  bufferLength += length;
}

/**@brief  This function returns the byte received from serial input that haven't
  *        HID packet format
  *@param  packet list of bytes received
  *@param  length list length
  *@retval TRUE if some data is present, FALSE otherwise
  */
boolean HID_ClassDriver_SerialData(uint8_t *packet, uint8_t *length)
{
  if (dataToFlush) {
    if (bufferLength >1)
      memcpy(packet, bufferToFlush, bufferLength);
    else
      packet[0] = bufferToFlush[0];
    *length = bufferLength;
    dataToFlush = FALSE;
    bufferLength = 0;
    return TRUE;
  }
  return FALSE;
}

/**@brief  This function Send the command response for certain USB request
  *@param  cmdReq request from USB class driver
  *@param  numDevice USB device number
  *@param  packet contains the information of the command response
  *@param  packetLen contains the length of the packet
  *@retval None
  */
void HID_ClassDriver_USBCommandRsp(uint8_t cmdReq, uint8_t numDevice, uint8_t *packet, uint8_t packetLen)
{
  sendTransportPacket(cmdReq, packet, packetLen);
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
