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
#include "custom_bus.h"
#include "stm32f405xx.h"



/*-------------------------------------------------------------------------------*/
/* Declarations -----------------------------------------------------------------*/
/* ------------------------------------------------------------------------------*/
LSM6DSR_Object_t *pAttitudeObj;	// Create instance of the LSM6DSR_Object_t object TODO
LSM6DSR_Axes_t *pAcceleration;
LSM6DSR_Axes_t *pAngularRate;
LSM6DSR_IO_t *pAttitudeIO;

const uint8_t FIFO_Mode = 6;	// LSM6DSR_STREAM_MODE
const uint8_t Watermark = 2;	// Number of data pieces in FIFO before int triggered
const float Odr = 200;

extern float currentACC_x;			// The acceleration as estimated using double exponential smoothing (externally visible)
extern float currentACC_y;			// The acceleration as estimated using double exponential smoothing (externally visible)
extern float currentACC_z;			// The acceleration as estimated using double exponential smoothing (externally visible)



/* ---------------------------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/
/* ---------------------------------------------------------------------------*/
/* ----------------- General -------------------------------------------------*/


/**
 * @brief  Utilises the SPI bus to read from registers on connected devices
 * @param  reg the output data rate value to be set
 * @param  data pointer where the data will be stored
 * @param  len the number of consecutive registers to read
 * @retval Just 0
 */
static int32_t attitude_device_read(void *handle, uint8_t reg, uint8_t* pData, uint16_t len)	{
	/* Args example (ctx->handle, reg, data, len)	*/

	reg = reg|0x40;			// setting read bit 8
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	BSP_SPI1_Send(&reg, 1);
	BSP_SPI1_Recv(&pData, len);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);

	return 0;
}
// TODO ideally swap to interrupt mode, would need to replace custom_bus.c as modifications will be overwritten if reconfigd



/**
 * @brief  Utilises the SPI bus to write to registers on connected devices
 * @param  reg the output data rate value to be set
 * @param  data pointer where the data will be stored
 * @param  len the number of consecutive registers to read
 * @retval Just 0
 */
static int32_t attitude_device_write(void *handle, uint8_t reg, uint8_t* pData, uint16_t len)	{
	/* Args example (ctx->handle, reg, data, len)	*/
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	BSP_SPI1_Send(&reg, 1);
	BSP_SPI1_Send(pData, len);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);

	return 0;
}



/**
 * @brief  Initialise the LSM6DSR accelerometer by calling the init, enable and Odr functions.
 * @param  Odr float the output data rate to be set
 * @retval Nothing
 */
void Initialise_Attitude(void)	{
	// TODO, check that we are looking at the right axis for the acceleration orientation

	pAttitudeIO->Init = &BSP_SPI1_Init;
	pAttitudeIO->DeInit = &BSP_SPI1_DeInit;
	pAttitudeIO->BusType = LSM6DSR_SPI_4WIRES_BUS;  // Spi 4 wire
	pAttitudeIO->Address = NULL;  // Null because this is an I2C address. We are using SPI
	pAttitudeIO->WriteReg = &attitude_device_write;
	pAttitudeIO->ReadReg = &attitude_device_read;
	pAttitudeIO->GetTick = &BSP_GetTick;
	LSM6DSR_RegisterBusIO(pAttitudeObj, pAttitudeIO);

	/* Init Sensor*/
	LSM6DSR_Init(pAttitudeObj);  // Initialise the IMU
	LSM6DSR_FIFO_Set_Mode(pAttitudeObj, FIFO_Mode); // Set FIFO mode
	LSM6DSR_FIFO_Set_Watermark_Level(pAttitudeObj, Watermark);

	/* Accelerometer */
	LSM6DSR_ACC_Enable(pAttitudeObj);  // Enable the accelerometer
	LSM6DSR_ACC_SetOutputDataRate(pAttitudeObj, Odr);  // Set Odr

	/* Gyro */
	LSM6DSR_GYRO_Enable(pAttitudeObj);  // Enable the GYRO so we can get acceleration rates
	LSM6DSR_GYRO_SetOutputDataRate(pAttitudeObj, Odr);  // Set Odr

	*pAcceleration->x = 0;
	*pAcceleration->y = 0;
	*pAcceleration->z = 0;

	/* Possibly usefull functions*/
	// LSM6DSR_FIFO_ACC_Set_BDR(LSM6DSR_Object_t *pObj, float Bdr) TODO
	// LSM6DSR_ACC_SetFullScale(LSM6DSR_Object_t *pObj, int32_t FullScale); // Set full scale
	// LSM6DSR_ACC_Enable_Tilt_Detection(LSM6DSR_Object_t *pObj, LSM6DSR_SensorIntPin_t IntPin); // Enable tilt detection
	// LSM6DSR_ACC_Enable_6D_Orientation(LSM6DSR_Object_t *pObj, LSM6DSR_SensorIntPin_t IntPin); // Enable 6D orientation
	// LSM6DSR_ACC_Get_6D_Orientation_XL(pAttitudeObj, uint8_t *XLow); // Get 6D orientation
	// LSM6DSR_ACC_GetSensitivity(pAttitudeObj, float *Sensitivity); // Get sensitivity
	// 
}


/**
 * @brief  Updates the global pressure variable (declared in header) using sensor data using double exponential smoothing
 */
void Update_Att_DES(void) {
	const float alpha = 0.03;
  	const float gamma = 0.01;
	static float intercept = 0;	
  	float newAlt = -1;

  	Update_GYRO();
	currentACC_x = pAcceleration->x;
	currentACC_y = pAcceleration->y;
	currentACC_z = pAcceleration->z;

	if (newAlt != -1)	{
		float prevEst = currentACC_x;
		currentACC_x = (alpha*newAlt)+((1-alpha)*(prevEst+intercept));	// Predicted next pressure in hPa (100Pa)
		intercept = gamma*(currentACC_x-prevEst)+((1-gamma)*intercept);
	}

	if (newAlt != -1)	{
		float prevEst = currentACC_y;
		currentACC_y = (alpha*newAlt)+((1-alpha)*(prevEst+intercept));	// Predicted next pressure in hPa (100Pa)
		intercept = gamma*(currentACC_y-prevEst)+((1-gamma)*intercept);
	}

	if (newAlt != -1)	{
		float prevEst = currentACC_z;
		currentACC_z = (alpha*newAlt)+((1-alpha)*(prevEst+intercept));	// Predicted next pressure in hPa (100Pa)
		intercept = gamma*(currentACC_z-prevEst)+((1-gamma)*intercept);
	}
}



/**
 * @brief  Measure the current acceleration and angular rate values and write to location
 * @param  pAcceleration pointer to a LSM6DSR_Axes_t struct containing the acceleration in the three axes
 * @param  pAngularRate pointer to a LSM6DSR_Axes_t struct containing the angular rates
 * @retval Nothing
 */
void Update_GYRO(void)	{
	/* Accelerometer */
	LSM6DSR_ACC_GetAxes(pAttitudeObj, pAcceleration);
	// LSM6DSR_ACC_GetAxesRaw(LSM6DSR_Object_t *pObj, LSM6DSR_AxesRaw_t *Value)	
	
	/* Gyro */
	LSM6DSR_GYRO_GetAxes(pAttitudeObj, pAngularRate);
	// LSM6DSR_GYRO_GetAxesRaw(LSM6DSR_Object_t *pObj, LSM6DSR_AxesRaw_t *Value)
}



/* ----------------- Accelerometer -------------------------------------------------------------------*/
/**
 * @brief  Read the x acceleration stored at the pAcceleration struct
 * @param  pAcceleration pointer to a LSM6DSR_Axes_t struct containing the acceleration in the three axes
 * @retval x, the acceleration in the x direction
 */
int32_t Read_ACC_x(LSM6DSR_Axes_t *pAcceleration)	{
	return pAcceleration->x;
}


/**
 * @brief  Read the y acceleration stored at the pAcceleration struct
 * @param  pAcceleration pointer to a LSM6DSR_Axes_t struct containing the acceleration in the three axes
 * @retval y, the acceleration in the y direction
 */
int32_t Read_ACC_y(LSM6DSR_Axes_t *pAcceleration)	{
	return pAcceleration->y;
}


/**
 * @brief  Read the z acceleration stored at the pAcceleration struct
 * @param  pAcceleration pointer to a LSM6DSR_Axes_t struct containing the acceleration in the three axes
 * @retval z, the acceleration in the z direction
 */
int32_t Read_ACC_z(LSM6DSR_Axes_t *pAcceleration)	{
	return pAcceleration->z;
}



/* ----------------- GYRO ----------------------------------------------------------------------------*/
/**
 * @brief  Read the rotation about the x axis from the pAngularRate struct
 * @param  pAngularRate pointer to a LSM6DSR_Axes_t struct containing the rotation rate of the three axes
 * @retval x, the rotation about the x axis
 */
int32_t Read_GYRO_x(LSM6DSR_Axes_t *pAngularRate)	{
	return pAngularRate->x;
}

/**
 * @brief  Read the rotation about the y axis from the pAngularRate struct
 * @param  pAngularRate pointer to a LSM6DSR_Axes_t struct containing the rotation rate of the three axes
 * @retval y, the rotation about the y axis
 */
int32_t Read_GYRO_y(LSM6DSR_Axes_t *pAngularRate)	{
	return pAngularRate->y;
}

/**
 * @brief  Read the rotation about the z axis from the pAngularRate struct
 * @param  pAngularRate pointer to a LSM6DSR_Axes_t struct containing the rotation rate of the three axes
 * @retval z, the rotation about the z axis
 */
int32_t Read_GYRO_z(LSM6DSR_Axes_t *pAngularRate)	{
	return pAngularRate->z;
}