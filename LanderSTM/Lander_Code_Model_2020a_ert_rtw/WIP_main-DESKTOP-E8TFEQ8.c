// Work In Progress main.c file to try and figure out how to use the Simulink Model

// Pete & Alex
// 17/3/21


#include "Lander_Code_Model_2020a.h"



double Gimbal_X_angle = 0.0;  // degrees
double Gimbal_Y_angle = 0.0;
double Reaction_Wheel_torque = 0.0;


Lander_Code_Model_2020a_U.ReferenceEulerAngles = 0.0;



void run_model(void){


    IMU_update();

    Lander_Code_Model_2020a_X.pqr_CSTATE[0] = IMU_angles[0];  //p
    Lander_Code_Model_2020a_X.pqr_CSTATE[1] = IMU_angles[1];  //q
    Lander_Code_Model_2020a_X.pqr_CSTATE[2] = IMU_angles[2];  //r


    Lander_Code_Model_2020a_step(); // run the model step
    Lander_Code_Model_2020a_derivatives();  // and derivatives


    Gimbal_X_angle = Lander_Code_Model_2020a_Y.Gimble_Angles[0];
    Gimbal_Y_angle = Lander_Code_Model_2020a_Y.Gimble_Angles[1];
    Reaction_Wheel_torque = Lander_Code_Model_2020a_Y.ReactionWheelTorque;


    Gimbal_X_Servo.write(Gimbal_X_angle);
    Gimbal_Y_Servo.write(Gimbal_Y_angle);
    Reactionwheel_esc.write(Reaction_Wheel_torque);


}



