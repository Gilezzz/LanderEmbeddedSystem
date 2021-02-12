/*
 * Pressure_Retrieve.c
 *
 *  Created on: Feb 12, 2021
 *      Author: alexh
 */

/* Includes ------------------------------------------------------------------*/
#include "lps22hh.h"

LPS22HH_Object_t *pPressObj;

/**
 * @brief  Get the LPS22HH temperature sensor output data rate
 * @param  pObj the device pObj
 * @param  Odr pointer where the output data rate is written
 * @retval 0 in case of success, an error code otherwise
 */
void Initialise_Press()	{
	/*
	 * Enable drdy
	Fifo Set mode 1
	Get_press
	LPS22H_init
	LPS22H_temp_disable
	LPS22H_press_set_output_dataRate
	 */
	LPS22HH_Init(LPS22HH_Object_t *pObj);

}
