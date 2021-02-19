/*
 * Attitude_Retrieve.c
 *
 *  Created on: 18 Feb 2021
 *      Author: alexh
 */


/* Includes ------------------------------------------------------------------*/
#include "lsm6dsr.h"


/* Functions -----------------------------------------------------------------*/


/**
 * @brief  Get the LPS22HH temperature sensor output data rate
 * @param  pObj the device pObj
 * @param  Odr pointer where the output data rate is written
 * @retval
 */
void Initialise_Attitude(uint8_t Mode, float Odr)	{
	/*
	 * Initialise Sensor
	 *
	 */

	LSM6DSR_Init(LSM6DSR_Object_t *pObj);  // Initialise the IMU
	LSM6DSR_ACC_Enable(LSM6DSR_Object_t *pObj);  // Enable the accelerometer
	LSM6DSR_ACC_SetOutputDataRate(LSM6DSR_Object_t *pObj, float Odr);  // Set Odr
	LSM6DSR_ACC_SetFullScale(LSM6DSR_Object_t *pObj, int32_t FullScale); // Set full scale
	LSM6DSR_ACC_Enable_Tilt_Detection(LSM6DSR_Object_t *pObj, LSM6DSR_SensorIntPin_t IntPin); // Enable tilt detection
	LSM6DSR_ACC_Enable_6D_Orientation(LSM6DSR_Object_t *pObj, LSM6DSR_SensorIntPin_t IntPin); // Enable 6D orientation

	LSM6DSR_FIFO_Set_Mode(LSM6DSR_Object_t *pObj, uint8_t Mode); // Set FIFO mode

	LSM6DSR_ACC_Get_6D_Orientation_XL(LSM6DSR_Object_t *pObj, uint8_t *XLow); // Get 6D orientation
	LSM6DSR_ACC_GetSensitivity(LSM6DSR_Object_t *pObj, float *Sensitivity); // Get sensitivity

}