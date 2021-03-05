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
#include "lps22hh_reg.h"
#include "custom_bus.h"
#include "stm32f405xx.h"
#include "stdint.h"
#include "string.h"



/*-------------------------------------------------------------------------------*/
/* Declarations ------------------------------------------------------------------*/
/* ------------------------------------------------------------------------------*/

LPS22HH_Object_t *pPressObj;  // Create instance of the LPS22HH_Object_t object TODO
LPS22HH_IO_t *pPressIO;		  //IO object used in populating pPressOBj


const uint8_t Watermark = 2;					// Number of data pieces in FIFO before int triggered
const float Odr = 200;							// Output data rate
const uint8_t FIFO_Mode = LPS22HH_STREAM_MODE;  // LPS22HH_STREAM_MODE, continuous
float Pressure_Ground_loc;  					// Decalre ground pressure value
float currentAlt;								// The altitude as estimated using double exponential smoothing (externally visible)



/* ---------------------------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/
/* ---------------------------------------------------------------------------*/


/**
 * @brief  Utilises the SPI bus to read from registers on connected devices
 * @param  reg the output data rate value to be set
 * @param  data pointer where the data will be stored
 * @param  len the number of consecutive registers to read
 * @retval Just 0
 */
static int32_t press_device_read(void *handle, uint8_t reg, uint8_t* pData, uint16_t len)	{
	/* Args example (ctx->handle, reg, data, len)	*/

	reg = reg|0x40;			// setting read bit 8
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
	BSP_SPI1_Send(&reg, 1);
	BSP_SPI1_Recv(&pData, len);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);

	return 0;
}
//TODO ideally swap to interrupt mode, would need to replace custom_bus.c as modifications will be overwritten if reconfigd



/**
 * @brief  Utilises the SPI bus to write to registers on connected devices
 * @param  reg the output data rate value to be set
 * @param  data pointer where the data will be stored
 * @param  len the number of consecutive registers to read
 * @retval Just 0
 */
static int32_t press_device_write(void *handle, uint8_t reg, uint8_t* pData, uint16_t len)	{
	/* Args example (ctx->handle, reg, data, len)	*/
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
	BSP_SPI1_Send(&reg, 1);
	BSP_SPI1_Send(pData, len);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);

	return 0;
}


/**
 * @brief  Initialise the LPS22HH device by setting DRDY, FIFO mode, obtaining ground pressure, pressure enable, and Odr
 * @retval Nothing, yet...
 */
void Initialise_Press(void)	{
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
	float *pPressure_Ground_loc = 0;

	pPressIO->Init = &BSP_SPI1_Init;
	pPressIO->DeInit = &BSP_SPI1_DeInit;
	pPressIO->BusType = LPS22HH_SPI_4WIRES_BUS;  // Spi 4 wire
	pPressIO->Address = NULL;
	pPressIO->WriteReg = &press_device_write;
	pPressIO->ReadReg = &press_device_read;
	pPressIO->GetTick = &BSP_GetTick;
	LPS22HH_RegisterBusIO(pPressObj, pPressIO);

	LPS22HH_Init(pPressObj);  // Initialise the sensor
	LPS22HH_TEMP_Disable(pPressObj);  // Disable the temperature sensor
	LPS22HH_Enable_DRDY_Interrupt(pPressObj);  // Enable the DRDY Interrupt
	LPS22HH_FIFO_Set_Mode(pPressObj, FIFO_Mode);  // Set the FIFO mode
	LPS22HH_PRESS_SetOutputDataRate(pPressObj, Odr);  // Set the output data rate
	LPS22HH_FIFO_Set_Watermark_Level(pPressObj, Watermark);
	LPS22HH_PRESS_Enable(pPressObj);  // Enable the pressure sensor
	LPS22HH_PRESS_GetPressure(pPressObj, pPressure_Ground_loc); // RECORD ground value of pressure.
	Pressure_Ground_loc = *pPressure_Ground_loc;

	currentAlt = 0;		// Initialises altitude value
}


/**
 * @brief  Get the pressure at the current location, do some maths using the ground pressure to find the current altitude
 * @retval Nothing, yet...
 */
float Calc_Altitude(void)	{
	//TODO will need to ignore pressure value in smoother if this func fails
	//TODO check for multiples of pressure data
	float *ptemp, *pPressure_loc;

	/* Write the pressure at the current time to pPressure_loc, ptemp is discarded but needs to be collected to empty fifo step */
	LPS22HH_FIFO_Get_Data(pPressObj,  pPressure_loc, ptemp);

	/* By using the value of pressure recorded during the initiali(se function we can calulate the current altutude */
	return (Pressure_Ground_loc - *pPressure_loc)/(1.225*9.81);  // TODO: use temp to calculate density
}



/**
 * @brief  Updates the global pressure variable (declared in header) using sensor data using double exponential smoothing
 */
void Update_Alt_DES(void) {
	const float alpha = 0.03;
  	const float gamma = 0.01;
	static float intercept = 0;	
  	float newAlt = -1;

  	newAlt = Calc_Altitude();

	if (newAlt != -1)	{
		float prevEst = currentAlt;
		currentAlt = (alpha*newAlt)+((1-alpha)*(prevEst+intercept));	// Predicted next pressure in hPa (100Pa)
		intercept = gamma*(currentAlt-prevEst)+((1-gamma)*intercept);
	}
}

