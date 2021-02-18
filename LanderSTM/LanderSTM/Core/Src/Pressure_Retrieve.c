/*
 * Pressure_Retrieve.c
 *
 *  Created on: Feb 12, 2021
 *      Author: alexh
 */

/* Includes ------------------------------------------------------------------*/
#include "lps22hh.h"

LPS22HH_Object_t *pPressObj;  // Create instance of the LPS22HH_Object_t object


/* Functions ------------------------------------------------------------------*/


/**
 * @brief  Get the LPS22HH temperature sensor output data rate
 * @param  Mode FIFO operating mode
 * @param  Odr pointer where the output data rate is written
 * @retval Nothing, yet...
 */
void Initialise_Press(uint8_t Mode, float Odr)	{
	/*
	 * Enable drdy
	 * Fifo Set mode 1
	 * Get_press
	 * 		LPS22HH_PRESS_GetPressure(LPS22HH_Object_t *pObj, float *Value)
	 * LPS22H_init
	 * LPS22H_temp_disable
	 * LPS22H_press_enable
	 * LPS22H_press_set_output_dataRate
	 */
	//uint8_t Mode = 1;  // Mode for the FIFO
	//float Odr = 9600;  // The output data rate

//	LPS22HH_Initialize(LPS22HH_Object_t *pObj); // is this needed?
	LPS22HH_Init(*pPressObj);  // Initialise the sensor
	LPS22HH_PRESS_Enable(*pPressObj);  // Enable the pressure sensor
	LPS22HH_TEMP_Disable(*pPressObj);  // Disable the temperature sensor
	LPS22HH_Enable_DRDY_Interrupt(*pPressObj);  // Enable the DRDY Interrupt
	LPS22HH_FIFO_Set_Mode(*pPressObj, Mode);  // Set the FIFO mode
	LPS22HH_PRESS_SetOutputDataRate(*pPressObj, Odr);  // Set the output data rate

}


