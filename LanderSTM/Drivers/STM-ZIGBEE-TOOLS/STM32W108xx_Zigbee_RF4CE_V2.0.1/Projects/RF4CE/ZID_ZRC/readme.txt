/**
  @page Sample 
  
  @verbatim
  ******************** (C) COPYRIGHT 2012 STMicroelectronics *******************
  * @file    RF4CE/ZID_ZRC/readme.txt 
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012
  * @brief   Description of the Sample applications.
  ******************************************************************************
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
  * limitations under the Licens
  *   
  ******************************************************************************
   @endverbatim

@par Example Description 

The stRC_Mouse is a simple application to demonstrate the ZID&ZRC RF4CE application profiles merged.
You can set two boards, one like ZID RF4CE MEMS Mouse Device and ZRC and the second like 
a USB RF4CE ZID Host.

More details about this Demo implementation is given in the User manual 
"UM0909 STM32W108xx RF4CE  library", available for download from the ST
microcontrollers website: www.st.com/stm32w



@par Directory contents 

  - RF4CE/ZID/stm32w108xx_conf.h        Library Configuration file
  - RF4CE/ZID/stm32w108xx_it.h          Interrupt handlers header file
  - RF4CE/ZID/stm32w108xx_it.c          Interrupt handlers                
  - RF4CE/ZID/stRC_Mouse                stRC_Mouse program body
  - RF4CE/ZID/zid_zrc_usb.h             ZID ZRC USB header file
  - RF4CE/ZID/zid_zrc_usb.c             HID RF4CE usb interface with HID class driver
  - RF4CE/ZID/zid_zrc_transport.h       ZID ZRC transport header file
  - RF4CE/ZID/zid_zrc_transport.c       HID RF4CE usb transport firmware package
  - RF4CE/ZID/zid_zrc_sm.h              ZID ZRC RF4CE state machines  header file
  - RF4CE/ZID/zid_zrc_sm.c              zid_zrc state machine program body
  - RF4CE/ZID/zid_zrc_constants.h       ZID ZRC RF4CE constants header file
  - RF4CE/ZID/zid_zrc_attributes.h      ZID ZRC Device attributes header file
  - RF4CE/ZID/zid_zrc_attributes.c      ZID attributes management
  - RF4CE/ZID/zid_zrc.h                 ZID ZRC RF4CE header file
  - RF4CE/ZID/zid_zrc.c                 ZID ZRC RF4CE implementation
         
@par Hardware and Software environment

To set-up the boards you need to use the board buttons, 
following the procedure below:

  - MEMS Mouse Device + RC MB950

     1) Flash hid_rf4ce.s37 firmware on the board with the command:
       stm32w_flasher.exe -p COMnn -m -f -r hid_rf4ce.s37
     2) CONFIGURE ZID RF4CE MOUSE DEVICE AND DELETE THE PAIRING TABLE:
       reset the board with the button S1 pressed.
       When you release the button S1 you can see the 
       LED D1 (green led) flash two times to indicate that 
       the command was executed.
     3) PAIR MOUSE DEVICE WITH USB DONGLE
       Press S1.
  
  Note that the mouse and RC will go to deep sleep after five seconds of no movement or buttons events.
  To wakeup the mouse, please press any buttons or reset.
  
  - Other buttons commands (not normally needed):

     - UNCONFIGURE ZID RF4CE MOUSE DEVICE:
       reset the board with the button S1 pressed, keep the button S1 pressed at least 5 seconds.
       When you see flash the LED D1 for two times you can release the button S1. At this 
       time the board will be unconfigured.
       
     - RESET the board:
       if you reset the board without any button pressed,the board will not lose the configuration
  
  
  - USB DONGLE (MB950 or MB951)

       1) Flash hid_rf4ce.s37 firmware on the board with the command:
          stm32w_flasher.exe -p COMnn -m -f -r hid_rf4ce.s37
       2) CONFIGURE ZID RF4CE USB HOST
          After reset, press the button S1. You can see the LED D1 flash every half second 
          for 5 sec. If the configuration end with SUCCESS the LED D1 will remain on otherwise
          will remain off.
      
       3) PAIRING PROCEDURE:
          press the button S1 on the ZID RF4CE Mouse board and on ZID RF4CE USB Host board, 
          the LED D1 will start to blink during the pairing procedure. If the procedure will end with 
          SUCCESS, the ZID RF4CE Host will notify the mouse device connected to the PC and now you can 
          use the mouse board like a pointer with the button right and left.
  
  - Other buttons commands (not normally needed):

       DELETE PAIRING TABLE ON ZID RF4CE USB HOST:
       press the S1 button for a couple of seconds, when the LED D1 
       will be off the device will have the pairing table deleted
  
  
  - Board supported
  
                               |MB851 |MB954| MB950| MB951|
    ---------------------------+------+-----+------+------+
     ZID ZRC RF4CE MOUSE ANDRC:| x    |  x  |   v  |  x   |
    ---------------------------+------+-----+------+------+
     ZID RF4CE USB HOST DONGLE:| v    |  v  |   v  |  v   |
    ---------------------------+------+-----+------+------+
   v = supported
   x = not supported

  - This example has been tested with STMicroelectronics MB851 revD, MB954 revC, MB950 revB 
    and MB951 revB (STM32W108xx) boards and can be easily tailored to any other supported 
    device and development board.

 
@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain      
 - Rebuild all files and load your image into target memory
 - Run the application
 
 - Basic Mouse instructions:
  
   - Left click (S1 or S2)
   - Right Click (S3 or S4)
      Mouse movement will be detected by the on board MEMS, tilt the mouse in a direction and the speed of the mouse will be proportional to the tilt angle.
      In particular:
      - Tilting towards ground will move mouse down
      - Tilting towards ceiling will move mouse up
      - Tilting left will move mouse left
      - Tilting right will move mouse right
  
 - Basic RC instructions:

    - VOL+ click S1
    - VOL- click S2
    - CH+ click S4
    - CH- click S3

BUTTON S5 is used to switch from Mouse to RC and vice versa

@note
 - STM32W108xx devices are STM32W108xx microcontrollers where the Flash memory 
   density ranges between 64 and 256 Kbytes.
   
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */