/**
  ******************************************************************************
  * @file    ir.c
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012
  * @brief   ir program body 
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

/* Includes ------------------------------------------------------------------*/
#include PLATFORM_HEADER
#include "ir.h"
#include "stm32w108xx_tim.h"

/** @addtogroup ZRC_demos
  * @{
  */

/** @addtogroup IR
  * @{
  */

/* Private define ------------------------------------------------------------*/
#define DIFF(a,b) ((a >= b)? (a - b) : ((1<<20) - (b - a)))
#define DELAY(uSeconds) {uint32_t start = ST_RadioGetMacTimer(); while (DIFF(ST_RadioGetMacTimer(), start) < uSeconds); }

/* Private macro -------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
extern uint32_t ST_RadioGetMacTimer(void);
uint32_t GpioStatus;

/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/

/*-------------------------------COMMON IR FUNCTIONS--------------------------*/
/** 
  *@brief  Enables carrier output
  *@param  None
  *@retval inline
  */
static inline void IRCarrierEnable(void)
{
  /* Restore GPIOA status */
  GPIOB->CRH = (GpioStatus | 0x00000009) & 0xFFFFFFF9;
}

/** 
  *@brief  Disables carrier output
  *@param  None
  *@retval inline
  */
static inline void IRCarrierDisable (void)
{
   /* Save GPIO status in order to restore it on enable */
   GpioStatus = GPIOB->CRH;
   GPIOB->ODR  &= 0xFFFFFFEF;
   GPIOB->CRH &= 0xFFFFFFF1;      
}

/*-------------------------------PHILIPS-RC5 FUNCTIONS------------------------*/
/** 
  *@brief  Initializes and enables SIRC carrier
  *@param  None
  *@retval None
  */
void RC5CarrierInit(void)
{

  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  
  TIM_TimeBaseStructure.TIM_Period = 334; /* 333.6 */
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  
  /* PWM1 Mode configuration: Channell */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 83;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC4Init(TIM2, &TIM_OCInitStructure);
  
  TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);

  TIM_ARRPreloadConfig(TIM2, ENABLE);
  
  /* Save default alternate function GPIO status for GPIOA to be restored
   * after carrier disable */
  GpioStatus = GPIOB->CRH;  
  
  /* this will be allways on */
  TIM_Cmd(TIM2, ENABLE);
  
  /* remap waveform on PB4/pin20 */  
  TIM2->CCR4 |= 0x80;
}

/** 
  *@brief  Generates an RC5 zero symbol
  *@param  None
  *@retval None
  */
void RC5zero(void)
{
   IRCarrierEnable();
   
   DELAY(889);
   
   IRCarrierDisable();
   
   DELAY(889);
}

/** 
  *@brief  Generates an RC5 one symbol
  *@param  None
  *@retval None
  */
void RC5one(void)
{
   DELAY(889);
   
   IRCarrierEnable();
      
   DELAY(889);
   
   IRCarrierDisable();
}

/** 
  *@brief  Generates an RC5 frame preamble
  *@param  None
  *@retval None
  */
static inline void RC5Preamble(uint8_t T)
{
  RC5one();
  RC5one();
  T? RC5one():RC5zero();
}

/** 
  *@brief  Sends out an RC5 frame containing a given command device ID and button status
  *@param  None
  *@retval None
  */
void SendRC5_IRcommand (uint8_t command, uint8_t deviceID,uint8_t buttonStatus)
{
   uint8_t j = 0;

   RC5Preamble(buttonStatus);

   deviceID<<=3;
   /* Device ID loop */
   for(j=0; j<5; j++)
   {  
     if (deviceID & 0x80)
       RC5one();
     else  
       RC5zero();
   
     deviceID<<=1;
   } 
   
   command<<=2;
   /* Command loop */
   for(j=0; j<6; j++)
   {  
     if (command & 0x80)  
       RC5one();
     else
       RC5zero();
   
     command<<=1;
   } 
   
   /* now wait for interpacket time constraints */   
   DELAY(89100);
}

/* -------------------------------SONY-SIRC FUNCTIONS-------------------------*/
/** 
  *@brief  Initializes and enables SIRC carrier
  *@param  None
  *@retval None
  */
void SIRCarrierInit(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  
  TIM_TimeBaseStructure.TIM_Period = 300; /* 300 -> 25 us period (works better)//325 -> 27 us period */
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  
  /* PWM1 Mode configuration: Channell */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 84;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  
  TIM_OC4Init(TIM2, &TIM_OCInitStructure);  
  TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);  

  TIM_ARRPreloadConfig(TIM2, ENABLE);
  
/* Save default alternate function GPIO status for GPIOA to be restored 
   after carrier disable */
  GpioStatus = GPIOB->CRH;  
  
  /* this will be allways on */
  TIM_Cmd(TIM2, ENABLE);
  
  /* remap waveform on PB4/pin20 */  
  TIM2->CCR4 |= 0x80;
}

/** 
  *@brief  Generates a SIRC start burst
  *@param  None
  *@retval None
  */
void SIRCstartBurst(void)
{
  IRCarrierEnable();
  
  DELAY(2400);
  
  IRCarrierDisable();
}

/** 
  *@brief  Generates an SIRC zero symbol
  *@param  None
  *@retval None
  */
void SIRCzero(void)
{ 
  DELAY(600);
  
  IRCarrierEnable();
  
  DELAY(600);
  
  IRCarrierDisable();
}

/** 
  *@brief  Generates an SIRC one symbol
  *@param  None
  *@retval None
  */
void SIRCone(void)
{
  DELAY(600);
  
  IRCarrierEnable();
  
  DELAY(1200);
  
  IRCarrierDisable();
}

/** 
  *@brief  Adds timegap upto 45ms from begin of frame start burst
  *@param  time from start
  *@retval None
  */
void SIRCwait4PacketInterspace(uint32_t timeFromStart)
{
  DELAY(45000-timeFromStart)
}

/** 
  *@brief  Generates a SIRC frame with given (by parameter) command,
  *        device ID and sirc frame
  *@param  command
  *@param  deviceID
  *@param  sircDevIDSize can be
  *         - 5   (for 12 bit SIRC)
  *         - 8   (for 15 bit SIRC)
  *         - 13  (for 20 bit SIRC)        
  *@retval None
  */
void SendSIRC_IRcommand (__IO uint8_t command, __IO uint16_t deviceID, __IO uint8_t sircDevIDSize)
{
  /* Parameters declared as __IO to avoid a weird optimization effect */
   uint8_t j = 0;
   uint32_t timefromStart = 2400;

#ifdef ST_INTERFRAME_SPACING_CHECK
/* toggles GPIO at each new frame transmit */   
   static uint8_t toggle = 0;
   toggle = 1 - toggle;
   
   GPIOA->ODR  = 0xFFFFFFF0 | toggle;
   GPIOA->CRL &= 0xFFFFFFF1;
#endif
  
   SIRCstartBurst();
  
   /* Command loop */
   for(j=0; j<7; j++)
   {  
     if (command & 0x01)
     {  
       SIRCone();
       timefromStart += 1800;
     }  
     else
     {  
       SIRCzero();
       timefromStart += 1200;
     }
     
     command>>=1;

   } 

   /* Device ID loop */
   for(j=0; j<sircDevIDSize; j++)      
   {
     
     if (deviceID & 0x0001)       
     {  
       SIRCone();
       timefromStart += 1800;
     }  
     else
     {  
       SIRCzero();
       timefromStart += 1200;
     }  
   
     deviceID>>=1;

   } 
   
  SIRCwait4PacketInterspace(timefromStart);
}

/*----------------STANDARD INDEPENDENT FUNCTIONS/MAIN INTERFACE---------------*/
/** 
  *@brief  Initializes and enables IR carrier
  *@param  SIRC or RC5 (definition at ir.h)       
  *@retval None
  */
void IR_Init(uint8_t irStandard)
{
  if ((irStandard & 0x01) == RC5)
    RC5CarrierInit();
  else
    SIRCarrierInit();
}

/** 
  *@brief  Sends out an IR frame according to standard 
  *        and parameters definitions above
  *@param  ir standard
  *@param  frame command       
  *@retval None
  */
void IR_Sendcommand (uint8_t  irStandard, frameCommand *frameParams)
{
  if ((irStandard & 0x01) == RC5)
    SendRC5_IRcommand (frameParams->rc5.command, frameParams->rc5.deviceID, frameParams->rc5.buttonStatus);
  else
    SendSIRC_IRcommand (frameParams->sirc.command,frameParams->sirc.deviceID, (irStandard >> 4));
}

/**
  * @}
  */

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
