/**
  ******************************************************************************
  * @file    ir.h
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012
  * @brief   ir profile header file 
  * @verbatim
  *
 ===============================================================================
                              ##### Note #####
 =============================================================================== 
  * 
  * This file contains prototypes of functions needed to generate waveforms to be 
  * fed to an infrared driver input stage for both PHILPS RC5 and SONY SIRC infrared 
  * standards.
  *   
  * @endverbatim  
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


/* Conding rule : last bit to distinguish SIRC from RC5, high nibble embeds */
/* deviceID size used to send command */

/* Includes ------------------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

#define SIRC    0x50
#define SIRC12  0x50
#define SIRC15  0x80
#define SIRC20  0xD0
#define RC5     0x01

/* Private typedef -----------------------------------------------------------*/

typedef union 
{
  struct sircstr {
  uint8_t  command;
  uint16_t deviceID;
  } sirc;
  
  struct rc5str {
  uint8_t  command;
  uint8_t  deviceID;
  uint8_t  buttonStatus;
  } rc5;
  
} frameCommand;

/* Private macro -------------------------------------------------------------*/
/* Public variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Initializes and enables IR carrier */
/* param: SIRC or RC5 */

void IR_Init(uint8_t irStandard);

/* Sends out an IR frame according to standard and parameters definitions above */
void IR_Sendcommand (uint8_t  irStandard, frameCommand *frameParams);


/* CHECK BELOW A LIST OF COMMANDS FOR SONY TV (DEVICE ID 1) */
/*
Sony:1

Command Code
 Command(s)
 
0
 1
 
1
 2
 
2
 3
 
3
 4
 
4
 5
 
5
 6
 
6
 7
 
7
 8
 
8
 9
 
9
 0
 
10
 11
 
11
 Enter, 12
 
12
 1-, 13
 
13
 2-, 14
 
14
 Guide, Channel Guide, P/C, 15
 
15
 Clear
 
16
 Channel Up
 
17
 Channel Down
 
18
 Volume Up
 
19
 Volume Down
 
20
 Mute
 
21
 Power
 
22
 Reset
 
23
 MTS/SAP, Audio Monitor, Nicam
 
24
 Picture/Contrast Up
 
25
 Picture/Contrast Down
 
26
 Color Up
 
27
 Color Down
 
28
 Channel Lock
 
29
 -/--
 
30
 Bright Up
 
31
 Bright Down
 
32
 Hue Down
 
33
 Hue Up
 
34
 Sharpness Up
 
35
 Sharpness Down
 
36
 TV tuner (discrete)
 
37
 TV/Video
 
38
 Balance Left
 
39
 Balance Right
 
40
 Bass, Tone, Loudness On/Off, OSE
 
41
 SRS, Surround, 3D Sound On/Off
 
42
 Antenna/Aux
 
43
 Clock, Time
 
44
 External Antenna
 
45
 RGB
 
46
 Power On (discrete)
 
47
 Power Off (discrete), System Off
 
48
 Timer, Time, Blk
 
49
 Up
 
50
 Down
 
51
 Right, Clock AM/PM
 
52
 Left, Clock Clear
 
54
 Sleep, Off Timer
 
56
 Analog Tuner (discrete), Teletext Off/Out, Power On (discrete), Select Tuner
 
58
 Display, Info, Teletext Index
 
59
 Jump, Last
 
60
 Timer-Off, Repeat
 
62
 Pic Off
 
63
 Teletext On/Mode, Display
 
64
 Video 1 (discrete), Line A, Line, Video
 
65
 Video 2 (discrete), Line B, Option
 
66
 Video 3 (discrete), VTR
 
67
 RGB Input 1 (discrete), Computer (RGB) input (discrete)
 
68
 RGB Input 2 (discrete)
 
71
 Video 4 (discrete)
 
72
 Video 5 (discrete), YUV
 
73
 Video 6 (discrete), FM
 
74
 Teletext Hold, Noise Reduction On/Off
 
75
 Fasttext Red (obsolete?)
 
76
 Fasttext Green (obsolete?)
 
77
 S-Video, Y/C, Fasttext Yellow (obsolete?)
 
78
 Cable toggle
 
79
 Notch Filter On/Off, Fasttext Blue (obsolete?)
 
80
 Internal Speakers On/Off
 
81
 Sat (?)
 
88
 PIP Channel Up
 
89
 PIP Channel Down
 
90
 PIP TV/Video
 
91
 PIP On/Toggle
 
92
 PIP Freeze, Digital Memo
 
94
 PIP Position
 
95
 PIP Swap
 
96
 Menu, Guide, Wega Gate
 
97
 Video
 
98
 Audio
 
99
 Exit
 
100
 Picture Mode, ASC, Profile
 
101
 Return, Select, Enter (menu select key)
 
102
 Output Select
 
103
 PIP source
 
104
 TV System
 
106
 Color System
 
107
 Auto Program
 
108
 Preset
 
110
 Search +
 
111
 Search –
 
112
 Treble Up
 
113
 Treble Down
 
114
 Bass Up
 
115
 Bass Down
 
116
 Up, R, Level Up
 
117
 Down, L, Level Down
 
120
 Add
 
121
 Erase
 
124
 Select
 
125
 Trinitron
 
127
 test code
*/



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
