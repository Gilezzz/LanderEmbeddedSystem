/*
 * Pressure_Retrieve.h
 *
 *  Created on: Feb 12, 2021
 *      Author: alexh
 */

#ifndef INC_PRESSURE_RETRIEVE_H_
#define INC_PRESSURE_RETRIEVE_H_

extern float currentAlt;                                        //The altitude as estimated using double exponential smoothing (externally visible)

void Initialise_Press(float Odr, float *pPressure_Ground_loc);  //Initialisation function
void Update_DES(void)                                            //Updates current altitude estimation using sensor data

#endif /* INC_PRESSURE_RETRIEVE_H_ */
