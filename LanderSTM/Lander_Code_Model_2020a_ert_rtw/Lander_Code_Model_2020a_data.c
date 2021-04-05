/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: Lander_Code_Model_2020a_data.c
 *
 * Code generated for Simulink model 'Lander_Code_Model_2020a'.
 *
 * Model version                  : 1.0
 * Simulink Coder version         : 9.3 (R2020a) 18-Nov-2019
 * C/C++ source code generated on : Tue Mar 16 21:18:12 2021
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "Lander_Code_Model_2020a.h"
#include "Lander_Code_Model_2020a_private.h"

/* Block parameters (default storage) */
P_Lander_Code_Model_2020a_T Lander_Code_Model_2020a_P = {
  /* Variable: Awet
   * Referenced by: '<S3>/Wetted Area (m^2)1'
   */
  0.017,

  /* Variable: Cd
   * Referenced by: '<S3>/Drag Coefficient1'
   */
  0.9,

  /* Variable: I
   * Referenced by: '<S8>/Constant1'
   */
  { 0.01467119791, -3.025858E-5, -1.498435E-5, -3.025858E-5, 0.00934316828,
    1.765228E-5, -1.498435E-5, 1.765228E-5, 0.01468029903 },

  /* Variable: l_r2cg
   * Referenced by:
   *   '<S2>/l_r2cg1'
   *   '<S5>/Constant'
   */
  0.05,

  /* Variable: rho
   * Referenced by: '<S3>/Density (kg//m^3)1'
   */
  1.225,

  /* Mask Parameter: PIDController3_D
   * Referenced by: '<S152>/Derivative Gain'
   */
  -0.1,

  /* Mask Parameter: PIDController1_D
   * Referenced by: '<S54>/Derivative Gain'
   */
  -0.125,

  /* Mask Parameter: PIDController2_D
   * Referenced by: '<S104>/Derivative Gain'
   */
  0.1,

  /* Mask Parameter: PIDController3_DifferentiatorIC
   * Referenced by: '<S153>/UD'
   */
  0.0,

  /* Mask Parameter: PIDController1_DifferentiatorIC
   * Referenced by: '<S55>/UD'
   */
  0.0,

  /* Mask Parameter: PIDController1_I
   * Referenced by: '<S59>/Integral Gain'
   */
  0.0,

  /* Mask Parameter: PIDController2_I
   * Referenced by: '<S107>/Integral Gain'
   */
  0.0,

  /* Mask Parameter: PIDController3_I
   * Referenced by: '<S157>/Integral Gain'
   */
  0.0,

  /* Mask Parameter: PIDController2_InitialCondition
   * Referenced by: '<S105>/Filter'
   */
  0.0,

  /* Mask Parameter: PIDController3_InitialCondition
   * Referenced by: '<S160>/Integrator'
   */
  0.0,

  /* Mask Parameter: PIDController1_InitialCondition
   * Referenced by: '<S62>/Integrator'
   */
  0.0,

  /* Mask Parameter: PIDController2_InitialConditi_o
   * Referenced by: '<S110>/Integrator'
   */
  0.0,

  /* Mask Parameter: PIDController2_LowerSaturationL
   * Referenced by: '<S117>/Saturation'
   */
  -0.01,

  /* Mask Parameter: PIDController2_N
   * Referenced by: '<S113>/Filter Coefficient'
   */
  15.0,

  /* Mask Parameter: PIDController3_P
   * Referenced by: '<S165>/Proportional Gain'
   */
  -0.3,

  /* Mask Parameter: PIDController1_P
   * Referenced by: '<S67>/Proportional Gain'
   */
  -0.45,

  /* Mask Parameter: PIDController2_P
   * Referenced by: '<S115>/Proportional Gain'
   */
  0.4,

  /* Mask Parameter: PIDController2_UpperSaturationL
   * Referenced by: '<S117>/Saturation'
   */
  0.01,

  /* Mask Parameter: uDOFEulerAngles_Vm_0
   * Referenced by: '<S1>/ub,vb,wb'
   */
  { 0.0, 0.0, 0.0 },

  /* Mask Parameter: uDOFEulerAngles_eul_0
   * Referenced by: '<S6>/phi theta psi'
   */
  { 0.0, 0.0, 0.0 },

  /* Mask Parameter: uDOFEulerAngles_mass_0
   * Referenced by: '<S8>/Constant'
   */
  1.4,

  /* Mask Parameter: uDOFEulerAngles_pm_0
   * Referenced by: '<S1>/p,q,r '
   */
  { 0.0, 0.0, 0.0 },

  /* Mask Parameter: uDOFEulerAngles_xme_0
   * Referenced by: '<S1>/xe,ye,ze'
   */
  { 0.0, 0.0, 80.0 },

  /* Expression: zeros(3)
   * Referenced by: '<S8>/Constant2'
   */
  { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },

  /* Expression: pi
   * Referenced by: '<S6>/phi theta psi'
   */
  3.1415926535897931,

  /* Expression: -pi
   * Referenced by: '<S6>/phi theta psi'
   */
  -3.1415926535897931,

  /* Expression: -9.81
   * Referenced by: '<S3>/g'
   */
  -9.81,

  /* Expression: 1.4
   * Referenced by: '<S3>/Mass (kg)'
   */
  1.4,

  /* Expression: 0
   * Referenced by: '<S3>/Constant'
   */
  0.0,

  /* Expression: 0.5
   * Referenced by: '<S3>/Gain3'
   */
  0.5,

  /* Computed Parameter: Integrator_gainval
   * Referenced by: '<S160>/Integrator'
   */
  0.1,

  /* Computed Parameter: Tsamp_WtEt
   * Referenced by: '<S155>/Tsamp'
   */
  10.0,

  /* Computed Parameter: Integrator_gainval_h
   * Referenced by: '<S62>/Integrator'
   */
  0.1,

  /* Computed Parameter: Tsamp_WtEt_n
   * Referenced by: '<S57>/Tsamp'
   */
  10.0,

  /* Computed Parameter: Integrator_gainval_o
   * Referenced by: '<S110>/Integrator'
   */
  0.1,

  /* Computed Parameter: Filter_gainval
   * Referenced by: '<S105>/Filter'
   */
  0.1,

  /* Expression: 180/pi
   * Referenced by: '<S24>/Gain'
   */
  57.295779513082323,

  /* Expression: 12
   * Referenced by: '<S2>/Saturation1'
   */
  12.0,

  /* Expression: -12
   * Referenced by: '<S2>/Saturation1'
   */
  -12.0,

  /* Expression: -1
   * Referenced by: '<S2>/Gain'
   */
  -1.0,

  /* Expression: pi/180
   * Referenced by: '<S177>/Gain1'
   */
  0.017453292519943295
};

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
