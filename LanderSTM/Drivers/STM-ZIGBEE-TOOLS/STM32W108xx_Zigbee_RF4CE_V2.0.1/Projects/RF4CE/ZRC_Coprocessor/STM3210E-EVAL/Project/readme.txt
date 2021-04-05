/**
  @page Sample 
  
  @verbatim
  ******************** (C) COPYRIGHT 2012 STMicroelectronics *******************
  * @file    RF4CE/ZRC_Coprocessor/STM32F10E-EVAL/Project/readme.txt 
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

It is possible to use an STM32W in an RF4CE network as a slave device driven by an
external microcontroller, that acts as a host via a serial communication connection.
This Coprocessor mode is useful when the RF4CE target is a device which already has a
microcontroller. For example, a TV, STB or DVD player.
   
More details about this Demo implementation is given in the User manual 
"UM0909 STM32W108xx RF4CE  library", available for download from the ST
microcontrollers website: www.st.com/stm32w
   

@par Directory contents 

  - RF4CE/ZRC_Coprocessor/STM32F10E-EVAL/Project/src/stm32f10x_it.c       Main Interrupt Service Routines
  - RF4CE/ZRC_Coprocessor/STM32F10E-EVAL/Project/src/stm32_sc.c           API for nwk coprocessor serial implemenation
  - RF4CE/ZRC_Coprocessor/STM32F10E-EVAL/Project/src/serial_utils.c       Driver for serial_utilities              
  - RF4CE/ZRC_Coprocessor/STM32F10E-EVAL/Project/src/rf4ce_cmds.c         Host commands for nwk coprocessor implemenation
  - RF4CE/ZRC_Coprocessor/STM32F10E-EVAL/Project/src/cerc_rf4ce.c         CERC RF4CE profile implemenation
  - RF4CE/ZRC_Coprocessor/STM32F10E-EVAL/Project/src/stRC.c               stRC program body
  - RF4CE/ZRC_Coprocessor/STM32F10E-EVAL/Project/inc/stm32f10x_it.h       Interrupt handlers header file
  - RF4CE/ZRC_Coprocessor/STM32F10E-EVAL/Project/inc/stm32f10x_conf.h     Library Configuration file 
  - RF4CE/ZRC_Coprocessor/STM32F10E-EVAL/Project/inc/stm32_sc.h           RF4CE API definition for nwk coprocessor serial implemenation
  - RF4CE/ZRC_Coprocessor/STM32F10E-EVAL/Project/inc/serial_utils.h       Header file for serial utilities 
  - RF4CE/ZRC_Coprocessor/STM32F10E-EVAL/Project/inc/rf4ce_type.h         rf4ce_type Header file 
  - RF4CE/ZRC_Coprocessor/STM32F10E-EVAL/Project/inc/rf4ce_nwkcop.h       rf4ce_nwkcop header file
  - RF4CE/ZRC_Coprocessor/STM32F10E-EVAL/Project/inc/rf4ce.h              rf4ce header file
  - RF4CE/ZRC_Coprocessor/STM32F10E-EVAL/Project/inc/nib.h                nib header file
  - RF4CE/ZRC_Coprocessor/STM32F10E-EVAL/Project/inc/cerc_rf4ce.h         cerc_rf4ce header file
  - RF4CE/ZRC_Coprocessor/STM32F10E-EVAL/Project/inc/cerc_constants.h     cerc_cpnstants header file
  - RF4CE/ZRC_Coprocessor/STM32F10E-EVAL/Project/inc/stRC.h               Functions prototypes for the stRC firmware library
         
@par Hardware and Software environment

    The software demo highlights the same functions as those described for th ZRC demo as well as
    how to work using an STM32W as the RF4CE network coprocessor.
  
    The firmware for the MB672 board can be built from the IAR Embedded Workbench
    sources, using the related IAR project file. Please note that the firmware on the host MCU
    will be built for the UART interface by default.
    The firmware running on the coprocessor board is only provided prebuilt and can be found
    in the prebuilt directory.

  
  - Board supported
  
    The following boards are used to execute the demo:
      - MB672 with STM32F103VET6 processor (host board)
      - MB851 with the STM32W108 processor (coprocessor board)  

  - This example has been tested with STMicroelectronics MB851 revD

  - This demo can be running in two modes: SPI and UART. Two images are provided in the project:
     - nwkCop10EEval_SPI
     - nwkCop10EEval_UART  
  
@par How to use it ? 

In order to make the program work, you must do the following :

  For the STM32F firmware: 
   - Open your preferred toolchain      
   - Rebuild all files and load your image into target memory
   - Run the application
  
  For the stm32w coprocessor:
   - To program the STM32W prebuilt coprocessor firmware, please use the stm32w_flasher.exe 
 


@note
 - STM32W108xx devices are STM32W108xx microcontrollers where the Flash memory 
   density ranges between 64 and 256 Kbytes.
   
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */