/*
 * Attitude_Retrieve.c
 *
 *  Created on: 18 Feb 2021
 *      Author: alexh
 */

/* ---------------------------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
/* ---------------------------------------------------------------------------*/

#include "lsm6dsr.h"
LSM6DSR_Object_t *pAttitudeObj;  // Create instance of the LPS22HH_Object_t object
const uint8_t FIFO_Mode = 6;  // LSM6DSR_STREAM_MODE


/* ---------------------------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/
/* ---------------------------------------------------------------------------*/

/**
 * @brief  Get the LPS22HH temperature sensor output data rate
 * @param  pAttitudeObj pointer to the device object
 * @param  Odr float where the output data rate is written
 * @retval
 */
void Initialise_Attitude(uint8_t Mode, float Odr)	{
	/*
	 * Initialise Sensor
	 * Enable Accelerometer
	 * Set output data rate
	 */

	LSM6DSR_Init(pAttitudeObj);  // Initialise the IMU
	LSM6DSR_ACC_Enable(pAttitudeObj);  // Enable the accelerometer
	LSM6DSR_ACC_SetOutputDataRate(pAttitudeObj, Odr);  // Set Odr
//	LSM6DSR_ACC_SetFullScale(LSM6DSR_Object_t *pObj, int32_t FullScale); // Set full scale
//	LSM6DSR_ACC_Enable_Tilt_Detection(LSM6DSR_Object_t *pObj, LSM6DSR_SensorIntPin_t IntPin); // Enable tilt detection
//	LSM6DSR_ACC_Enable_6D_Orientation(LSM6DSR_Object_t *pObj, LSM6DSR_SensorIntPin_t IntPin); // Enable 6D orientation

	LSM6DSR_FIFO_Set_Mode(pAttitudeObj, FIFO_Mode); // Set FIFO mode

//	LSM6DSR_ACC_Get_6D_Orientation_XL(pAttitudeObj, uint8_t *XLow); // Get 6D orientation
//	LSM6DSR_ACC_GetSensitivity(pAttitudeObj, float *Sensitivity); // Get sensitivity

}

float Get_Attitude()	{
	// TODO
}
