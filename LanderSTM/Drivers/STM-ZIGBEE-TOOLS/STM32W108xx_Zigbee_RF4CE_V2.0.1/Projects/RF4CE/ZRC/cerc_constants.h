/**
  ******************************************************************************
  * @file    cerc_constants.h
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   cerc_constants profile header file 
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
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#ifndef CERC_CONSTANTS_H
#define CERC_CONSTANTS_H

/* Device Type List */
/* 0x00 Reserved */
#define RF4CE_DEVICE_REMOTE_CONTROL                     0x01
#define RF4CE_DEVICE_TELEVISION                         0x02
#define RF4CE_DEVICE_PROJECTOR                          0x03
#define RF4CE_DEVICE_PLAYER                             0x04
#define RF4CE_DEVICE_RECORDER                           0x05
#define RF4CE_DEVICE_VIDEO_PLAYER_RECORDER              0x06
#define RF4CE_DEVICE_AUDIO_PLAYER_RECORDER              0x07
#define RF4CE_DEVICE_AUDIO_VIDEO_RECORDER               0x08
#define RF4CE_DEVICE_SET_TOP_BOX                        0x09
#define RF4CE_DEVICE_HOME_THEATER_SYSTEM                0x0A
#define RF4CE_DEVICE_MEDIA_CENTER_PC                    0x0B
#define RF4CE_DEVICE_GAME_CONSOLE                       0x0C
#define RF4CE_DEVICE_SATELLITE_RADIO_RECEIVER           0x0D
#define RF4CE_DEVICE_IR_EXTENDER                        0x0E
#define RF4CE_DEVICE_MONITOR                            0x0F
/* 0x10..0xFD: Reserved */
#define RF4CE_DEVICE_GENERIC                            0xFE
#define RF4CE_DEVICE_RESERVED_FOR_WILDCARDS             0xFF

/* Vendor Identifiers */
/* 0x0000 Reserved */
#define RF4CE_VENDOR_PANASONIC                          0x0001
#define RF4CE_VENDOR_SONY                               0x0002
#define RF4CE_VENDOR_SAMSUNG                            0x0003
#define RF4CE_VENDOR_PHILIPS                            0x0004
#define RF4CE_VENDOR_FREESCALE                          0x0005
#define RF4CE_VENDOR_OKI                                0x0006
#define RF4CE_VENDOR_TEXAS_INSTRUMENTS                  0x0007
/* 0x0008 - 0xfff0 reserved */
#define RF4CE_VENDOR_TEST_VENDOR_1                      0xfff1
#define RF4CE_VENDOR_TEST_VENDOR_2                      0xfff2
#define RF4CE_VENDOR_TEST_VENDOR_3                      0xfff3
/* 0xfff4 - 0xffff Reserved */

/* Profile Identifiers */
/* 0x00 Reserved */
#define RF4CE_PROFILE_CERC                              0x01
/* 0x02 - 0xbf Reserved for future standard ZigBee RF4CE profiles */
/* 0xc0 - 0xfe Manufacturer specific profiles */
#define RF4CE_PROFILE_WILDCARD                          0xff

/* RC command codes */
#define RF4CE_CERC_SELECT                               0x00
#define RF4CE_CERC_UP                                   0x01
#define RF4CE_CERC_DOWN                                 0x02
#define RF4CE_CERC_LEFT                                 0x03
#define RF4CE_CERC_RIGHT                                0x04
#define RF4CE_CERC_RIGHT_UP                             0x05
#define RF4CE_CERC_RIGHT_DOWN                           0x06
#define RF4CE_CERC_LEFT_UP                              0x07
#define RF4CE_CERC_LEFT_DOWN                            0x08
#define RF4CE_CERC_ROOT_MENU                            0x09
#define RF4CE_CERC_SETUP_MENU                           0x0A
#define RF4CE_CERC_CONTENTS_MENU                        0x0B
#define RF4CE_CERC_FAVORITE_MENU                        0x0C
#define RF4CE_CERC_EXIT                                 0x0D
/* 0x0e - 0x0f Reserved */
#define RF4CE_CERC_NUM_0                                0x20
#define RF4CE_CERC_NUM_1                                0x21
#define RF4CE_CERC_NUM_2                                0x22
#define RF4CE_CERC_NUM_3                                0x23
#define RF4CE_CERC_NUM_4                                0x24
#define RF4CE_CERC_NUM_5                                0x25
#define RF4CE_CERC_NUM_6                                0x26
#define RF4CE_CERC_NUM_7                                0x27
#define RF4CE_CERC_NUM_8                                0x28
#define RF4CE_CERC_NUM_9                                0x29
#define RF4CE_CERC_DOT                                  0x2A
#define RF4CE_CERC_ENTER                                0x2B
#define RF4CE_CERC_CLEAR                                0x2C
/* 0x2d - 0x2e Reserved */
#define RF4CE_CERC_NEXT_FAVORITE                        0x2F
#define RF4CE_CERC_CHANNEL_UP                           0x30
#define RF4CE_CERC_CHANNEL_DOWN                         0x31
#define RF4CE_CERC_PREVIOUS_CHANNEL                     0x32
#define RF4CE_CERC_SOUND_SELECT                         0x33
#define RF4CE_CERC_INPUT_SELECT                         0x34
#define RF4CE_CERC_DISPLAY_INFORMATION                  0x35
#define RF4CE_CERC_HELP                                 0x36
#define RF4CE_CERC_PAGE_UP                              0x37
#define RF4CE_CERC_PAGE_DOWN                            0x38
/* 0x39 - 0x3f Reserved */
#define RF4CE_CERC_POWER                                0x40
#define RF4CE_CERC_VOLUME_UP                            0x41
#define RF4CE_CERC_VOLUME_DOWN                          0x42
#define RF4CE_CERC_MUTE                                 0x43
#define RF4CE_CERC_PLAY                                 0x44
#define RF4CE_CERC_STOP                                 0x45
#define RF4CE_CERC_PAUSE                                0x46
#define RF4CE_CERC_RECORD                               0x47
#define RF4CE_CERC_REWIND                               0x48
#define RF4CE_CERC_FAST_FORWARD                         0x49
#define RF4CE_CERC_EJECT                                0x4A
#define RF4CE_CERC_FORWARD                              0x4B
#define RF4CE_CERC_BACKWARD                             0x4C
#define RF4CE_CERC_STOP_RECORD                          0x4D
#define RF4CE_CERC_PAUSE_RECORD                         0x4E
/* 0x4f Reserved */
#define RF4CE_CERC_ANGLE                                0x50
#define RF4CE_CERC_SUB_PICTURE                          0x51
#define RF4CE_CERC_VIDEO_ON_DEMAND                      0x52
#define RF4CE_CERC_ELECTRONIC_PROGRAM_GUIDE             0x53
#define RF4CE_CERC_TIMER_PROGRAMMING                    0x54
#define RF4CE_CERC_INITIAL_CONFIGURATION                0x55
/* 0x56 - 0x5f Reserved */
#define RF4CE_CERC_PLAY_FUNCTION                        0x60
#define RF4CE_CERC_PAUSE_PLAY_FUNCTION                  0x61
#define RF4CE_CERC_RECORD_FUNCTION                      0x62
#define RF4CE_CERC_PAUSE_RECORD_FUNCTION                0x63
#define RF4CE_CERC_STOP_FUNCTION                        0x64
#define RF4CE_CERC_MUTE_FUNCTION                        0x65
#define RF4CE_CERC_RESTORE_VOLUME_FUNCTION              0x66
#define RF4CE_CERC_TUNE_FUNCTION                        0x67
#define RF4CE_CERC_SELECT_MEDIA_FUNCTION                0x68
#define RF4CE_CERC_SELECT_AV_INPUT_FUNCTION             0x69
#define RF4CE_CERC_SELECT_AUDIO_INPUT_FUNCTION          0x6A
#define RF4CE_CERC_POWER_TOGGLE_FUNCTION                0x6B
#define RF4CE_CERC_POWER_OFF_FUNCTION                   0x6C
#define RF4CE_CERC_POWER_ON_FUNCTION                    0x6D
/* 0x6e - 0x70 Reserved */
#define RF4CE_CERC_F1_BLUE                              0x71
#define RF4CE_CERC_F2_RED                               0x72
#define RF4CE_CERC_F3_GREEN                             0x73
#define RF4CE_CERC_F4_YELLOW                            0x74
#define RF4CE_CERC_F5                                   0x75
#define RF4CE_CERC_DATA                                 0x76
/* 0x77 - 0xff Reserved */

/* Added payload for play function */
#define RF4CE_CERC_PLAY_MODE_PLAY_FORWARD               0x24
#define RF4CE_CERC_PLAY_MODE_PLAY_REVERSE               0x20
#define RF4CE_CERC_PLAY_MODE_PLAY_STILL                 0x25
#define RF4CE_CERC_PLAY_MODE_FAST_FORWARD_MINIMUM_SPEED 0x05
#define RF4CE_CERC_PLAY_MODE_FAST_FORWARD_MEDIUM_SPEED  0x06
#define RF4CE_CERC_PLAY_MODE_FAST_FORWARD_MAXIMUM_SPEED 0x07
#define RF4CE_CERC_PLAY_MODE_FAST_REVERSE_MINIMUM_SPEED 0x09
#define RF4CE_CERC_PLAY_MODE_FAST_REVERSE_MEDIUM_SPEED  0x0A
#define RF4CE_CERC_PLAY_MODE_FAST_REVERSE_MAXIMUM_SPEED 0x0B
#define RF4CE_CERC_PLAY_MODE_SLOW_FORWARD_MINIMUM_SPEED 0x15
#define RF4CE_CERC_PLAY_MODE_SLOW_FORWARD_MEDIUM_SPEED  0x16
#define RF4CE_CERC_PLAY_MODE_SLOW_FORWARD_MAXIMUM_SPEED 0x17
#define RF4CE_CERC_PLAY_MODE_SLOW_REVERSE_MINIMUM_SPEED 0x19
#define RF4CE_CERC_PLAY_MODE_SLOW_REVERSE_MEDIUM_SPEED  0x1A
#define RF4CE_CERC_PLAY_MODE_SLOW_REVERSE_MAXIMUM_SPEED 0x1B

/* Size in bytes for command payload */
#define RF4CE_CERC_PLAY_FUNCTION_PAYLOAD 2
#define RF4CE_CERC_COMMAND_PAYLOAD 1

#endif /* CERC_CONSTANTS_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
