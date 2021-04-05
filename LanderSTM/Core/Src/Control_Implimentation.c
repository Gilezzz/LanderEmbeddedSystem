// Work In Progress main.c file to try and figure out how to use the Simulink Model

// Pete & Alex
// 17/3/21

#include "Lander_Code_Model_2020a.h"

double Gimbal_X_angle = 0.0; // -6 to 6
double Gimbal_Y_angle = 0.0;
double Reaction_Wheel_torque = 0.0;

double X_servo_zero_offset_pos = 90.0;
double Y_servo_zero_offset_pos = 90.0;

Lander_Code_Model_2020a_U.ReferenceEulerAngles = 0.0;

void run_model(void)
{

    /* Initialize model */
    Lander_Code_Model_2020a_initialize();

    while (enabled)
    {

        IMU_update();

        Lander_Code_Model_2020a_X.pqr_CSTATE[0] = IMU_angles[0]; //p
        Lander_Code_Model_2020a_X.pqr_CSTATE[1] = IMU_angles[1]; //q
        Lander_Code_Model_2020a_X.pqr_CSTATE[2] = IMU_angles[2]; //r

        Lander_Code_Model_2020a_step();        // run the model step
        Lander_Code_Model_2020a_derivatives(); // and derivatives

        Gimbal_X_angle = Lander_Code_Model_2020a_Y.Gimble_Angles[0] + X_servo_zero_offset_pos;
        Gimbal_Y_angle = Lander_Code_Model_2020a_Y.Gimble_Angles[1] + Y_servo_zero_offset_pos;
        Reaction_Wheel_torque = Lander_Code_Model_2020a_Y.ReactionWheelTorque;

        Gimbal_X_Servo.write(Gimbal_X_angle); // .write should exist as a driver and normalise based on the angles given
        Gimbal_Y_Servo.write(Gimbal_Y_angle);
        Reactionwheel_esc.write(Reaction_Wheel_torque);
    }
}




// Work In Progress main.c file to try and figure out how to use the Simulink Model

// Pete & Alex
// 17/3/21


#include "Lander_Code_Model_2020a.h"



double Gimbal_X_angle = 0.0;  // -6 to 6
double Gimbal_Y_angle = 0.0;  // 362E312E31-2E337E454E-477E57696E-5669737461-20362E322E-393230302E-7E31323334-35235F3135-39484C5447-3052545F32-4238303146-3238305F33-3543383739-4345454645-33415F3431-3037323639-323930345F-36616C6578-2E68696E65-406F75746C-6F6F6B2E63-6F6D5F3844-45534B544F-5045385446-455138

double Reaction_Wheel_torque = 0.0;

double X_servo_zero_offset_pos = 90.0;
double Y_servo_zero_offset_pos = 90.0;

Lander_Code_Model_2020a_U.ReferenceEulerAngles = 0.0;



void run_model(void){


    IMU_update();

    Lander_Code_Model_2020a_X.pqr_CSTATE[0] = IMU_angles[0];  //p
    Lander_Code_Model_2020a_X.pqr_CSTATE[1] = IMU_angles[1];  //q
    Lander_Code_Model_2020a_X.pqr_CSTATE[2] = IMU_angles[2];  //r


    Lander_Code_Model_2020a_step(); // run the model step
    Lander_Code_Model_2020a_derivatives();  // and derivatives


    Gimbal_X_angle = Lander_Code_Model_2020a_Y.Gimble_Angles[0] + X_servo_zero_offset_pos;
    Gimbal_Y_angle = Lander_Code_Model_2020a_Y.Gimble_Angles[1] + Y_servo_zero_offset_pos;
    Reaction_Wheel_torque = Lander_Code_Model_2020a_Y.ReactionWheelTorque;


    Gimbal_X_Servo.write(Gimbal_X_angle);  // .write should exist as a driver and normalise based on the angles given
    Gimbal_Y_Servo.write(Gimbal_Y_angle);
    Reactionwheel_esc.write(Reaction_Wheel_torque);


}



