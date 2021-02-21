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
LSM6DSR_Object_t *pAttitudeObj;  // Create instance of the LSM6DSR_Object_t object TODO
const uint8_t FIFO_Mode = 6;  // LSM6DSR_STREAM_MODE



/* ---------------------------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/
/* ---------------------------------------------------------------------------*/

/**
 * @brief  Initialise the LSM6DSR accelerometer by calling the init, enable and Odr functions.
 * @param  Odr float the output data rate to be set
 * @retval Nothing
 */
void Initialise_Attitude(float Odr)	{
	// TODO, check that we are looking at the right axis for the acceleration orientation
	/* Init Sensor*/
	LSM6DSR_Init(pAttitudeObj);  // Initialise the IMU
	LSM6DSR_FIFO_Set_Mode(pAttitudeObj, FIFO_Mode); // Set FIFO mode

	/* Accelerometer */
	LSM6DSR_ACC_Enable(pAttitudeObj);  // Enable the accelerometer
	LSM6DSR_ACC_SetOutputDataRate(pAttitudeObj, Odr);  // Set Odr

	/* Gyro */
	LSM6DSR_GYRO_Enable(pAttitudeObj);  // Enable the GYRO so we can get acceleration rates
	LSM6DSR_GYRO_SetOutputDataRate(pAttitudeObj, Odr);  // Set Odr

	/* Possibly usefull functions*/
	// LSM6DSR_FIFO_ACC_Set_BDR(LSM6DSR_Object_t *pObj, float Bdr) TODO
	//	LSM6DSR_ACC_SetFullScale(LSM6DSR_Object_t *pObj, int32_t FullScale); // Set full scale
	//	LSM6DSR_ACC_Enable_Tilt_Detection(LSM6DSR_Object_t *pObj, LSM6DSR_SensorIntPin_t IntPin); // Enable tilt detection
	//	LSM6DSR_ACC_Enable_6D_Orientation(LSM6DSR_Object_t *pObj, LSM6DSR_SensorIntPin_t IntPin); // Enable 6D orientation
	//	LSM6DSR_ACC_Get_6D_Orientation_XL(pAttitudeObj, uint8_t *XLow); // Get 6D orientation
	//	LSM6DSR_ACC_GetSensitivity(pAttitudeObj, float *Sensitivity); // Get sensitivity
}

/**
 * @brief  Measure the current acceleration and angular rate values and write to location
 * @param  pAcceleration pointer to a LSM6DSR_Axes_t struct containing the acceleration in the three axes
 * @param  pAngularRate pointer to a LSM6DSR_Axes_t struct containing the angular rates
 * @retval Nothing
 */
void Update_ACC_GYRO(LSM6DSR_Axes_t *pAcceleration, LSM6DSR_Axes_t *pAngularRate)	{
	/* Accelerometer */
	LSM6DSR_ACC_GetAxes(pAttitudeObj, pAcceleration);
	// LSM6DSR_ACC_GetAxesRaw(LSM6DSR_Object_t *pObj, LSM6DSR_AxesRaw_t *Value)	
	
	/* Gyro */
	LSM6DSR_GYRO_GetAxes(pAttitudeObj, pAngularRate);
	// LSM6DSR_GYRO_GetAxesRaw(LSM6DSR_Object_t *pObj, LSM6DSR_AxesRaw_t *Value)
}


/**
 * @brief  Read the x acceleration stored at the pAcceleration struct
 * @param  pAcceleration pointer to a LSM6DSR_Axes_t struct containing the acceleration in the three axes
 * @retval x the acceleration in the x direction
 */
int32_t Read_ACC_x(LSM6DSR_Axes_t *pAcceleration)	{
	return pAcceleration->x;
}


/**
 * @brief  Read the y acceleration stored at the pAcceleration struct
 * @param  pAcceleration pointer to a LSM6DSR_Axes_t struct containing the acceleration in the three axes
 * @retval x the acceleration in the x direction
 */
int32_t Read_ACC_y(LSM6DSR_Axes_t *pAcceleration)	{
	return pAcceleration->y;
}


/**
 * @brief  Read the z acceleration stored at the pAcceleration struct
 * @param  pAcceleration pointer to a LSM6DSR_Axes_t struct containing the acceleration in the three axes
 * @retval x the acceleration in the x direction
 */
int32_t Read_ACC_z(LSM6DSR_Axes_t *pAcceleration)	{
	return pAcceleration->z;
}

