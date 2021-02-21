/*
 * Pressure_Retrieve.c
 *
 *  Created on: Feb 12, 2021
 *      Author: alexh
 */

/* ---------------------------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
/* ---------------------------------------------------------------------------*/

#include "lps22hh.h"
LPS22HH_Object_t *pPressObj;  // Create instance of the LPS22HH_Object_t object TODO
const uint8_t FIFO_Mode = 2;  // LPS22HH_STREAM_MODE
float *pPressure_Ground_loc;  // Decalre ground pressure value



/* ---------------------------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/
/* ---------------------------------------------------------------------------*/

/**
 * @brief  Initialise the LPS22HH device by setting DRDY, FIFO mode, obtaining ground pressure, pressure enable, and Odr
 * @param  Odr the output data rate value to be set
 * @param  pPressure_Ground_loc pointer where the ground pressure is stored
 * @retval Nothing, yet...
 */
void Initialise_Press(float Odr, float *pPressure_Ground_loc)	{
	/*
	 * LPS22H_init
	 * Enable drdy
	 * Fifo Set mode 1
	 * Get_press
	 * 		LPS22HH_PRESS_GetPressure(LPS22HH_Object_t *pObj, float *Value)
	 * LPS22H_temp_disable
	 * LPS22H_press_enable
	 * LPS22H_press_set_output_dataRate
	 */

	LPS22HH_Init(pPressObj);  // Initialise the sensor
	LPS22HH_PRESS_Enable(pPressObj);  // Enable the pressure sensor
	LPS22HH_TEMP_Disable(pPressObj);  // Disable the temperature sensor
	LPS22HH_Enable_DRDY_Interrupt(pPressObj);  // Enable the DRDY Interrupt
	LPS22HH_FIFO_Set_Mode(pPressObj, FIFO_Mode);  // Set the FIFO mode
	LPS22HH_PRESS_SetOutputDataRate(pPressObj, Odr);  // Set the output data rate
	LPS22HH_PRESS_GetPressure(pPressObj, pPressure_Ground_loc); // RECORD ground value of pressure.
}


/**
 * @brief  Get the pressure at the current location, do some maths using the ground pressure to find the current altitude
 * @param  pPressure_loc pointer where the current pressure value will be written
 * @param  pPressure_Ground_loc pointer where the ground pressure is stored. This was done during the Initialise_Press
 * @retval Nothing, yet...
 */
void Get_Altitude(float *pPressure_loc, float *pPressure_Ground_loc)	{

	/* Write the pressure at the current time to pPressure_loc */
	LPS22HH_PRESS_GetPressure(pPressObj, pPressure_loc);

	/* By using the value of pressure recorded during the initialise function we can calulate the current altutude */
	return (*pPressure_Ground_loc - *pPressure_loc)/1.225/9.81;  // TODO: use temp to calculate density
}


