/**
  @page Sample 
  
  @verbatim
  ******************** (C) COPYRIGHT 2012 STMicroelectronics *******************
  * @file    RF4CE/ZRC/readme.txt 
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

The ZigBee RF4CE demo applications target the ZigBee RF4CE ZRC (Consumer electronic remote control) 
profile for remote control.

ZigBee RF4CE ZRC demo applications scenarios are mainly:
  -RS232 Target/RS232 Controller: This demo is exuted on Hyperterminal
  -ST virtual RC/ST virtual TV: Using the two PC_GUI for ZRC demo
  -ST virtual RC/Sony infrared TV
  
More details about this Demo implementation is given in the User manual 
"UM0909 STM32W108xx RF4CE  library", available for download from the ST
microcontrollers website: www.st.com/stm32w
  

@par Directory contents 

  - RF4CE/ZRC/stm32w108xx_conf.h     Library Configuration file
  - RF4CE/ZRC/stm32w108xx_it.h       Interrupt handlers header file
  - RF4CE/ZRC/stm32w108xx_it.c       Interrupt handlers               
  - RF4CE/ZRC/stRC.h                 This file contains functions prototypes 
  - RF4CE/ZRC/stRC.c                 Main program
  - RF4CE/ZRC/ir.h                   ir profile header file 
  - RF4CE/ZRC/ir.c                   ir program body 
  - RF4CE/ZRC/cerc_rf4ce.h           cerc_rf4ce profile header file 
  - RF4CE/ZRC/cerc_rf4ce.c           This file provides the CERC RF4CE profile implemenation
  - RF4CE/ZRC/cerc_constants.h       cerc_constants profile header file
  - RF4CE/ZRC/bootloader.h           Bootloader header file
  - RF4CE/ZRC/bootloader.c           Bootloader main program
         
@par Hardware and Software environment

  - This example runs on STM32W108xx Devices.
  
  - This example has been tested with STMicroelectronics MB851 revD, MB954 revC, MB950 revB 
    and MB951 revB (STM32W108xx) boards and can be easily tailored to any other supported 
    device and development board.

  - Boards Set-up
    - Connect the boards to a PC with a USB cable through USB connector J2 to power the board 
      and to communicate with the Hypertherminal
  
  - PC_GUI execution
    There is two PC_GUI: one for RC and one for TV.
    You can run this two PC_GUI under PCApplet/RF4CE.
  
  - Hyperterminal configuration:
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - BaudRate = 115200 baud
    - flow control: Disable
 
@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain      
 - Rebuild all files and load your image into target memory
 - Run the application

@note
 - STM32W108xx devices are STM32W108xx microcontrollers where the Flash memory 
   density ranges between 64 and 256 Kbytes.
   
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */