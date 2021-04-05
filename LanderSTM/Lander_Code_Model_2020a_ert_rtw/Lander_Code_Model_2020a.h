/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: Lander_Code_Model_2020a.h
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

#ifndef RTW_HEADER_Lander_Code_Model_2020a_h_
#define RTW_HEADER_Lander_Code_Model_2020a_h_
#include <string.h>
#include <math.h>
#include <stddef.h>
#ifndef Lander_Code_Model_2020a_COMMON_INCLUDES_
# define Lander_Code_Model_2020a_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "rtw_continuous.h"
#include "rtw_solver.h"
#endif                            /* Lander_Code_Model_2020a_COMMON_INCLUDES_ */

#include "Lander_Code_Model_2020a_types.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetContStateDisabled
# define rtmGetContStateDisabled(rtm)  ((rtm)->contStateDisabled)
#endif

#ifndef rtmSetContStateDisabled
# define rtmSetContStateDisabled(rtm, val) ((rtm)->contStateDisabled = (val))
#endif

#ifndef rtmGetContStates
# define rtmGetContStates(rtm)         ((rtm)->contStates)
#endif

#ifndef rtmSetContStates
# define rtmSetContStates(rtm, val)    ((rtm)->contStates = (val))
#endif

#ifndef rtmGetContTimeOutputInconsistentWithStateAtMajorStepFlag
# define rtmGetContTimeOutputInconsistentWithStateAtMajorStepFlag(rtm) ((rtm)->CTOutputIncnstWithState)
#endif

#ifndef rtmSetContTimeOutputInconsistentWithStateAtMajorStepFlag
# define rtmSetContTimeOutputInconsistentWithStateAtMajorStepFlag(rtm, val) ((rtm)->CTOutputIncnstWithState = (val))
#endif

#ifndef rtmGetDerivCacheNeedsReset
# define rtmGetDerivCacheNeedsReset(rtm) ((rtm)->derivCacheNeedsReset)
#endif

#ifndef rtmSetDerivCacheNeedsReset
# define rtmSetDerivCacheNeedsReset(rtm, val) ((rtm)->derivCacheNeedsReset = (val))
#endif

#ifndef rtmGetIntgData
# define rtmGetIntgData(rtm)           ((rtm)->intgData)
#endif

#ifndef rtmSetIntgData
# define rtmSetIntgData(rtm, val)      ((rtm)->intgData = (val))
#endif

#ifndef rtmGetOdeF
# define rtmGetOdeF(rtm)               ((rtm)->odeF)
#endif

#ifndef rtmSetOdeF
# define rtmSetOdeF(rtm, val)          ((rtm)->odeF = (val))
#endif

#ifndef rtmGetOdeY
# define rtmGetOdeY(rtm)               ((rtm)->odeY)
#endif

#ifndef rtmSetOdeY
# define rtmSetOdeY(rtm, val)          ((rtm)->odeY = (val))
#endif

#ifndef rtmGetPeriodicContStateIndices
# define rtmGetPeriodicContStateIndices(rtm) ((rtm)->periodicContStateIndices)
#endif

#ifndef rtmSetPeriodicContStateIndices
# define rtmSetPeriodicContStateIndices(rtm, val) ((rtm)->periodicContStateIndices = (val))
#endif

#ifndef rtmGetPeriodicContStateRanges
# define rtmGetPeriodicContStateRanges(rtm) ((rtm)->periodicContStateRanges)
#endif

#ifndef rtmSetPeriodicContStateRanges
# define rtmSetPeriodicContStateRanges(rtm, val) ((rtm)->periodicContStateRanges = (val))
#endif

#ifndef rtmGetZCCacheNeedsReset
# define rtmGetZCCacheNeedsReset(rtm)  ((rtm)->zCCacheNeedsReset)
#endif

#ifndef rtmSetZCCacheNeedsReset
# define rtmSetZCCacheNeedsReset(rtm, val) ((rtm)->zCCacheNeedsReset = (val))
#endif

#ifndef rtmGetdX
# define rtmGetdX(rtm)                 ((rtm)->derivs)
#endif

#ifndef rtmSetdX
# define rtmSetdX(rtm, val)            ((rtm)->derivs = (val))
#endif

#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

#ifndef rtmGetStopRequested
# define rtmGetStopRequested(rtm)      ((rtm)->Timing.stopRequestedFlag)
#endif

#ifndef rtmSetStopRequested
# define rtmSetStopRequested(rtm, val) ((rtm)->Timing.stopRequestedFlag = (val))
#endif

#ifndef rtmGetStopRequestedPtr
# define rtmGetStopRequestedPtr(rtm)   (&((rtm)->Timing.stopRequestedFlag))
#endif

#ifndef rtmGetT
# define rtmGetT(rtm)                  (rtmGetTPtr((rtm))[0])
#endif

#ifndef rtmGetTPtr
# define rtmGetTPtr(rtm)               ((rtm)->Timing.t)
#endif

/* Block signals (default storage) */
typedef struct {
  real_T VectorConcatenate[18];        /* '<S8>/Vector Concatenate' */
  real_T Selector[9];                  /* '<S7>/Selector' */
  real_T Selector1[9];                 /* '<S7>/Selector1' */
  real_T Selector2[9];                 /* '<S7>/Selector2' */
  real_T Transpose[9];                 /* '<S1>/Transpose' */
  real_T rtb_Transpose_m[9];
  real_T d[9];
  real_T A[9];
  real_T Product[3];                   /* '<S13>/Product' */
  real_T MatrixConcatenate[3];         /* '<S4>/Matrix Concatenate' */
  real_T Product2[3];                  /* '<S7>/Product2' */
  real_T TmpSignalConversionAtphithetaps[3];/* '<S6>/phidot thetadot psidot' */
  real_T sincos_o2[3];                 /* '<S15>/sincos' */
  real_T sincos_o1[3];                 /* '<S15>/sincos' */
  real_T GravitationalForce;           /* '<S3>/Gravitational Force' */
  real_T Product3[2];                  /* '<S2>/Product3' */
  real_T Sum[3];                       /* '<S1>/Sum' */
  real_T Cospsi;
  real_T Costheta;
  real_T Gain;                         /* '<S2>/Gain' */
  real_T Fcn;                          /* '<S2>/Fcn' */
  real_T F_r_tmp;
  real_T F_r_tmp_c;
  real_T d_k;
  real_T d1;
  real_T d2;
  real_T rtb_Sum_p_idx_0;
  real_T rtb_Gain1_idx_0;
  real_T Sum2_idx_1;
  real_T Sum2_idx_0;
  real_T Sum2_idx_2;
  real_T rtb_Gain1_idx_1;
  real_T unnamed_idx_2;
  real_T Selector_tmp;
  real_T rtb_Sum_p_tmp;
  real_T rtb_sincos_o2_tmp;
  real_T rtb_Sum_p_idx_1_tmp;
  real_T Tsamp;                        /* '<S155>/Tsamp' */
  real_T Tsamp_j;                      /* '<S57>/Tsamp' */
  real_T FilterCoefficient;            /* '<S113>/Filter Coefficient' */
  real_T IntegralGain;                 /* '<S59>/Integral Gain' */
} B_Lander_Code_Model_2020a_T;

/* Block states (default storage) for system '<Root>' */
typedef struct {
  real_T Integrator_DSTATE;            /* '<S160>/Integrator' */
  real_T UD_DSTATE;                    /* '<S153>/UD' */
  real_T Integrator_DSTATE_m;          /* '<S62>/Integrator' */
  real_T UD_DSTATE_k;                  /* '<S55>/UD' */
  real_T Integrator_DSTATE_g;          /* '<S110>/Integrator' */
  real_T Filter_DSTATE;                /* '<S105>/Filter' */
  real_T Product2_DWORK4[9];           /* '<S7>/Product2' */
} DW_Lander_Code_Model_2020a_T;

/* Continuous states (default storage) */
typedef struct {
  real_T phithetapsi_CSTATE[3];        /* '<S6>/phi theta psi' */
  real_T ubvbwb_CSTATE[3];             /* '<S1>/ub,vb,wb' */
  real_T pqr_CSTATE[3];                /* '<S1>/p,q,r ' */
  real_T xeyeze_CSTATE[3];             /* '<S1>/xe,ye,ze' */
} X_Lander_Code_Model_2020a_T;

/* Periodic continuous state vector (global) */
typedef int_T PeriodicIndX_Lander_Code_Mode_T[3];
typedef real_T PeriodicRngX_Lander_Code_Mode_T[6];

/* State derivatives (default storage) */
typedef struct {
  real_T phithetapsi_CSTATE[3];        /* '<S6>/phi theta psi' */
  real_T ubvbwb_CSTATE[3];             /* '<S1>/ub,vb,wb' */
  real_T pqr_CSTATE[3];                /* '<S1>/p,q,r ' */
  real_T xeyeze_CSTATE[3];             /* '<S1>/xe,ye,ze' */
} XDot_Lander_Code_Model_2020a_T;

/* State disabled  */
typedef struct {
  boolean_T phithetapsi_CSTATE[3];     /* '<S6>/phi theta psi' */
  boolean_T ubvbwb_CSTATE[3];          /* '<S1>/ub,vb,wb' */
  boolean_T pqr_CSTATE[3];             /* '<S1>/p,q,r ' */
  boolean_T xeyeze_CSTATE[3];          /* '<S1>/xe,ye,ze' */
} XDis_Lander_Code_Model_2020a_T;

#ifndef ODE3_INTG
#define ODE3_INTG

/* ODE3 Integration Data */
typedef struct {
  real_T *y;                           /* output */
  real_T *f[3];                        /* derivatives */
} ODE3_IntgData;

#endif

/* External inputs (root inport signals with default storage) */
typedef struct {
  real_T ReferenceEulerAngles;         /* '<Root>/Reference Euler Angles' */
  real_T Thrust;                       /* '<Root>/Thrust' */
} ExtU_Lander_Code_Model_2020a_T;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  real_T Gimble_Angles[2];             /* '<Root>/Gimble_Angles' */
  real_T ReactionWheelTorque;          /* '<Root>/Reaction Wheel Torque' */
} ExtY_Lander_Code_Model_2020a_T;

/* Parameters (default storage) */
struct P_Lander_Code_Model_2020a_T_ {
  real_T Awet;                         /* Variable: Awet
                                        * Referenced by: '<S3>/Wetted Area (m^2)1'
                                        */
  real_T Cd;                           /* Variable: Cd
                                        * Referenced by: '<S3>/Drag Coefficient1'
                                        */
  real_T I[9];                         /* Variable: I
                                        * Referenced by: '<S8>/Constant1'
                                        */
  real_T l_r2cg;                       /* Variable: l_r2cg
                                        * Referenced by:
                                        *   '<S2>/l_r2cg1'
                                        *   '<S5>/Constant'
                                        */
  real_T rho;                          /* Variable: rho
                                        * Referenced by: '<S3>/Density (kg//m^3)1'
                                        */
  real_T PIDController3_D;             /* Mask Parameter: PIDController3_D
                                        * Referenced by: '<S152>/Derivative Gain'
                                        */
  real_T PIDController1_D;             /* Mask Parameter: PIDController1_D
                                        * Referenced by: '<S54>/Derivative Gain'
                                        */
  real_T PIDController2_D;             /* Mask Parameter: PIDController2_D
                                        * Referenced by: '<S104>/Derivative Gain'
                                        */
  real_T PIDController3_DifferentiatorIC;
                              /* Mask Parameter: PIDController3_DifferentiatorIC
                               * Referenced by: '<S153>/UD'
                               */
  real_T PIDController1_DifferentiatorIC;
                              /* Mask Parameter: PIDController1_DifferentiatorIC
                               * Referenced by: '<S55>/UD'
                               */
  real_T PIDController1_I;             /* Mask Parameter: PIDController1_I
                                        * Referenced by: '<S59>/Integral Gain'
                                        */
  real_T PIDController2_I;             /* Mask Parameter: PIDController2_I
                                        * Referenced by: '<S107>/Integral Gain'
                                        */
  real_T PIDController3_I;             /* Mask Parameter: PIDController3_I
                                        * Referenced by: '<S157>/Integral Gain'
                                        */
  real_T PIDController2_InitialCondition;
                              /* Mask Parameter: PIDController2_InitialCondition
                               * Referenced by: '<S105>/Filter'
                               */
  real_T PIDController3_InitialCondition;
                              /* Mask Parameter: PIDController3_InitialCondition
                               * Referenced by: '<S160>/Integrator'
                               */
  real_T PIDController1_InitialCondition;
                              /* Mask Parameter: PIDController1_InitialCondition
                               * Referenced by: '<S62>/Integrator'
                               */
  real_T PIDController2_InitialConditi_o;
                              /* Mask Parameter: PIDController2_InitialConditi_o
                               * Referenced by: '<S110>/Integrator'
                               */
  real_T PIDController2_LowerSaturationL;
                              /* Mask Parameter: PIDController2_LowerSaturationL
                               * Referenced by: '<S117>/Saturation'
                               */
  real_T PIDController2_N;             /* Mask Parameter: PIDController2_N
                                        * Referenced by: '<S113>/Filter Coefficient'
                                        */
  real_T PIDController3_P;             /* Mask Parameter: PIDController3_P
                                        * Referenced by: '<S165>/Proportional Gain'
                                        */
  real_T PIDController1_P;             /* Mask Parameter: PIDController1_P
                                        * Referenced by: '<S67>/Proportional Gain'
                                        */
  real_T PIDController2_P;             /* Mask Parameter: PIDController2_P
                                        * Referenced by: '<S115>/Proportional Gain'
                                        */
  real_T PIDController2_UpperSaturationL;
                              /* Mask Parameter: PIDController2_UpperSaturationL
                               * Referenced by: '<S117>/Saturation'
                               */
  real_T uDOFEulerAngles_Vm_0[3];      /* Mask Parameter: uDOFEulerAngles_Vm_0
                                        * Referenced by: '<S1>/ub,vb,wb'
                                        */
  real_T uDOFEulerAngles_eul_0[3];     /* Mask Parameter: uDOFEulerAngles_eul_0
                                        * Referenced by: '<S6>/phi theta psi'
                                        */
  real_T uDOFEulerAngles_mass_0;       /* Mask Parameter: uDOFEulerAngles_mass_0
                                        * Referenced by: '<S8>/Constant'
                                        */
  real_T uDOFEulerAngles_pm_0[3];      /* Mask Parameter: uDOFEulerAngles_pm_0
                                        * Referenced by: '<S1>/p,q,r '
                                        */
  real_T uDOFEulerAngles_xme_0[3];     /* Mask Parameter: uDOFEulerAngles_xme_0
                                        * Referenced by: '<S1>/xe,ye,ze'
                                        */
  real_T Constant2_Value[9];           /* Expression: zeros(3)
                                        * Referenced by: '<S8>/Constant2'
                                        */
  real_T phithetapsi_WrappedStateUpperVa;/* Expression: pi
                                          * Referenced by: '<S6>/phi theta psi'
                                          */
  real_T phithetapsi_WrappedStateLowerVa;/* Expression: -pi
                                          * Referenced by: '<S6>/phi theta psi'
                                          */
  real_T g_Value;                      /* Expression: -9.81
                                        * Referenced by: '<S3>/g'
                                        */
  real_T Masskg_Value;                 /* Expression: 1.4
                                        * Referenced by: '<S3>/Mass (kg)'
                                        */
  real_T Constant_Value;               /* Expression: 0
                                        * Referenced by: '<S3>/Constant'
                                        */
  real_T Gain3_Gain;                   /* Expression: 0.5
                                        * Referenced by: '<S3>/Gain3'
                                        */
  real_T Integrator_gainval;           /* Computed Parameter: Integrator_gainval
                                        * Referenced by: '<S160>/Integrator'
                                        */
  real_T Tsamp_WtEt;                   /* Computed Parameter: Tsamp_WtEt
                                        * Referenced by: '<S155>/Tsamp'
                                        */
  real_T Integrator_gainval_h;       /* Computed Parameter: Integrator_gainval_h
                                      * Referenced by: '<S62>/Integrator'
                                      */
  real_T Tsamp_WtEt_n;                 /* Computed Parameter: Tsamp_WtEt_n
                                        * Referenced by: '<S57>/Tsamp'
                                        */
  real_T Integrator_gainval_o;       /* Computed Parameter: Integrator_gainval_o
                                      * Referenced by: '<S110>/Integrator'
                                      */
  real_T Filter_gainval;               /* Computed Parameter: Filter_gainval
                                        * Referenced by: '<S105>/Filter'
                                        */
  real_T Gain_Gain;                    /* Expression: 180/pi
                                        * Referenced by: '<S24>/Gain'
                                        */
  real_T Saturation1_UpperSat;         /* Expression: 12
                                        * Referenced by: '<S2>/Saturation1'
                                        */
  real_T Saturation1_LowerSat;         /* Expression: -12
                                        * Referenced by: '<S2>/Saturation1'
                                        */
  real_T Gain_Gain_j;                  /* Expression: -1
                                        * Referenced by: '<S2>/Gain'
                                        */
  real_T Gain1_Gain;                   /* Expression: pi/180
                                        * Referenced by: '<S177>/Gain1'
                                        */
};

/* Real-time Model Data Structure */
struct tag_RTM_Lander_Code_Model_202_T {
  const char_T *errorStatus;
  RTWSolverInfo solverInfo;
  X_Lander_Code_Model_2020a_T *contStates;
  int_T *periodicContStateIndices;
  real_T *periodicContStateRanges;
  real_T *derivs;
  boolean_T *contStateDisabled;
  boolean_T zCCacheNeedsReset;
  boolean_T derivCacheNeedsReset;
  boolean_T CTOutputIncnstWithState;
  real_T odeY[12];
  real_T odeF[3][12];
  ODE3_IntgData intgData;

  /*
   * Sizes:
   * The following substructure contains sizes information
   * for many of the model attributes such as inputs, outputs,
   * dwork, sample times, etc.
   */
  struct {
    int_T numContStates;
    int_T numPeriodicContStates;
    int_T numSampTimes;
  } Sizes;

  /*
   * Timing:
   * The following substructure contains information regarding
   * the timing information for the model.
   */
  struct {
    uint32_T clockTick0;
    time_T stepSize0;
    uint32_T clockTick1;
    SimTimeStep simTimeStep;
    boolean_T stopRequestedFlag;
    time_T *t;
    time_T tArray[2];
  } Timing;
};

/* Block parameters (default storage) */
extern P_Lander_Code_Model_2020a_T Lander_Code_Model_2020a_P;

/* Block signals (default storage) */
extern B_Lander_Code_Model_2020a_T Lander_Code_Model_2020a_B;

/* Continuous states (default storage) */
extern X_Lander_Code_Model_2020a_T Lander_Code_Model_2020a_X;

/* Block states (default storage) */
extern DW_Lander_Code_Model_2020a_T Lander_Code_Model_2020a_DW;

/* External inputs (root inport signals with default storage) */
extern ExtU_Lander_Code_Model_2020a_T Lander_Code_Model_2020a_U;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY_Lander_Code_Model_2020a_T Lander_Code_Model_2020a_Y;

/* Model entry point functions */
extern void Lander_Code_Model_2020a_initialize(void);
extern void Lander_Code_Model_2020a_step(void);
extern void Lander_Code_Model_2020a_terminate(void);

/* Real-time Model object */
extern RT_MODEL_Lander_Code_Model_20_T *const Lander_Code_Model_2020a_M;

/*-
 * These blocks were eliminated from the model due to optimizations:
 *
 * Block '<S10>/Unit Conversion' : Unused code path elimination
 * Block '<S12>/Unit Conversion' : Unused code path elimination
 * Block '<S55>/DTDup' : Unused code path elimination
 * Block '<S153>/DTDup' : Unused code path elimination
 * Block '<S16>/Reshape (9) to [3x3] column-major' : Reshape block reduction
 * Block '<S18>/Reshape1' : Reshape block reduction
 * Block '<S18>/Reshape2' : Reshape block reduction
 * Block '<S19>/Reshape1' : Reshape block reduction
 * Block '<S19>/Reshape2' : Reshape block reduction
 * Block '<S7>/Reshape' : Reshape block reduction
 * Block '<S7>/Reshape1' : Reshape block reduction
 * Block '<S11>/Unit Conversion' : Eliminated nontunable gain of 1
 * Block '<S13>/Reshape1' : Reshape block reduction
 * Block '<S13>/Reshape2' : Reshape block reduction
 */

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'Lander_Code_Model_2020a'
 * '<S1>'   : 'Lander_Code_Model_2020a/6DOF (Euler Angles)'
 * '<S2>'   : 'Lander_Code_Model_2020a/Gimble allocation '
 * '<S3>'   : 'Lander_Code_Model_2020a/Gravity&Drag'
 * '<S4>'   : 'Lander_Code_Model_2020a/PID Controller1'
 * '<S5>'   : 'Lander_Code_Model_2020a/Subsystem'
 * '<S6>'   : 'Lander_Code_Model_2020a/6DOF (Euler Angles)/Calculate DCM & Euler Angles'
 * '<S7>'   : 'Lander_Code_Model_2020a/6DOF (Euler Angles)/Calculate omega_dot'
 * '<S8>'   : 'Lander_Code_Model_2020a/6DOF (Euler Angles)/Determine Force,  Mass & Inertia'
 * '<S9>'   : 'Lander_Code_Model_2020a/6DOF (Euler Angles)/Vbxw'
 * '<S10>'  : 'Lander_Code_Model_2020a/6DOF (Euler Angles)/Velocity Conversion'
 * '<S11>'  : 'Lander_Code_Model_2020a/6DOF (Euler Angles)/Velocity Conversion1'
 * '<S12>'  : 'Lander_Code_Model_2020a/6DOF (Euler Angles)/Velocity Conversion2'
 * '<S13>'  : 'Lander_Code_Model_2020a/6DOF (Euler Angles)/transform to Inertial axes '
 * '<S14>'  : 'Lander_Code_Model_2020a/6DOF (Euler Angles)/Calculate DCM & Euler Angles/Rotation Angles to Direction Cosine Matrix'
 * '<S15>'  : 'Lander_Code_Model_2020a/6DOF (Euler Angles)/Calculate DCM & Euler Angles/phidot thetadot psidot'
 * '<S16>'  : 'Lander_Code_Model_2020a/6DOF (Euler Angles)/Calculate DCM & Euler Angles/Rotation Angles to Direction Cosine Matrix/Create 3x3 Matrix'
 * '<S17>'  : 'Lander_Code_Model_2020a/6DOF (Euler Angles)/Calculate omega_dot/3x3 Cross Product'
 * '<S18>'  : 'Lander_Code_Model_2020a/6DOF (Euler Angles)/Calculate omega_dot/I x w'
 * '<S19>'  : 'Lander_Code_Model_2020a/6DOF (Euler Angles)/Calculate omega_dot/I x w1'
 * '<S20>'  : 'Lander_Code_Model_2020a/6DOF (Euler Angles)/Calculate omega_dot/3x3 Cross Product/Subsystem'
 * '<S21>'  : 'Lander_Code_Model_2020a/6DOF (Euler Angles)/Calculate omega_dot/3x3 Cross Product/Subsystem1'
 * '<S22>'  : 'Lander_Code_Model_2020a/6DOF (Euler Angles)/Vbxw/Subsystem'
 * '<S23>'  : 'Lander_Code_Model_2020a/6DOF (Euler Angles)/Vbxw/Subsystem1'
 * '<S24>'  : 'Lander_Code_Model_2020a/Gimble allocation /Radians to Degrees'
 * '<S25>'  : 'Lander_Code_Model_2020a/Gravity&Drag/MATLAB Function1'
 * '<S26>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1'
 * '<S27>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2'
 * '<S28>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3'
 * '<S29>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Anti-windup'
 * '<S30>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/D Gain'
 * '<S31>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Filter'
 * '<S32>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Filter ICs'
 * '<S33>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/I Gain'
 * '<S34>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Ideal P Gain'
 * '<S35>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Ideal P Gain Fdbk'
 * '<S36>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Integrator'
 * '<S37>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Integrator ICs'
 * '<S38>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/N Copy'
 * '<S39>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/N Gain'
 * '<S40>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/P Copy'
 * '<S41>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Parallel P Gain'
 * '<S42>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Reset Signal'
 * '<S43>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Saturation'
 * '<S44>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Saturation Fdbk'
 * '<S45>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Sum'
 * '<S46>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Sum Fdbk'
 * '<S47>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Tracking Mode'
 * '<S48>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Tracking Mode Sum'
 * '<S49>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Tsamp - Integral'
 * '<S50>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Tsamp - Ngain'
 * '<S51>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/postSat Signal'
 * '<S52>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/preSat Signal'
 * '<S53>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Anti-windup/Passthrough'
 * '<S54>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/D Gain/Internal Parameters'
 * '<S55>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Filter/Differentiator'
 * '<S56>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Filter/Differentiator/Tsamp'
 * '<S57>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Filter/Differentiator/Tsamp/Internal Ts'
 * '<S58>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Filter ICs/Internal IC - Differentiator'
 * '<S59>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/I Gain/Internal Parameters'
 * '<S60>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Ideal P Gain/Passthrough'
 * '<S61>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Ideal P Gain Fdbk/Disabled'
 * '<S62>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Integrator/Discrete'
 * '<S63>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Integrator ICs/Internal IC'
 * '<S64>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/N Copy/Disabled wSignal Specification'
 * '<S65>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/N Gain/Passthrough'
 * '<S66>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/P Copy/Disabled'
 * '<S67>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Parallel P Gain/Internal Parameters'
 * '<S68>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Reset Signal/Disabled'
 * '<S69>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Saturation/Passthrough'
 * '<S70>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Saturation Fdbk/Disabled'
 * '<S71>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Sum/Sum_PID'
 * '<S72>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Sum Fdbk/Disabled'
 * '<S73>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Tracking Mode/Disabled'
 * '<S74>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Tracking Mode Sum/Passthrough'
 * '<S75>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Tsamp - Integral/Passthrough'
 * '<S76>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/Tsamp - Ngain/Passthrough'
 * '<S77>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/postSat Signal/Forward_Path'
 * '<S78>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller1/preSat Signal/Forward_Path'
 * '<S79>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/Anti-windup'
 * '<S80>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/D Gain'
 * '<S81>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/Filter'
 * '<S82>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/Filter ICs'
 * '<S83>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/I Gain'
 * '<S84>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/Ideal P Gain'
 * '<S85>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/Ideal P Gain Fdbk'
 * '<S86>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/Integrator'
 * '<S87>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/Integrator ICs'
 * '<S88>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/N Copy'
 * '<S89>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/N Gain'
 * '<S90>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/P Copy'
 * '<S91>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/Parallel P Gain'
 * '<S92>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/Reset Signal'
 * '<S93>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/Saturation'
 * '<S94>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/Saturation Fdbk'
 * '<S95>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/Sum'
 * '<S96>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/Sum Fdbk'
 * '<S97>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/Tracking Mode'
 * '<S98>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/Tracking Mode Sum'
 * '<S99>'  : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/Tsamp - Integral'
 * '<S100>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/Tsamp - Ngain'
 * '<S101>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/postSat Signal'
 * '<S102>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/preSat Signal'
 * '<S103>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/Anti-windup/Passthrough'
 * '<S104>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/D Gain/Internal Parameters'
 * '<S105>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/Filter/Disc. Forward Euler Filter'
 * '<S106>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/Filter ICs/Internal IC - Filter'
 * '<S107>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/I Gain/Internal Parameters'
 * '<S108>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/Ideal P Gain/Passthrough'
 * '<S109>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/Ideal P Gain Fdbk/Disabled'
 * '<S110>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/Integrator/Discrete'
 * '<S111>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/Integrator ICs/Internal IC'
 * '<S112>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/N Copy/Disabled'
 * '<S113>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/N Gain/Internal Parameters'
 * '<S114>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/P Copy/Disabled'
 * '<S115>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/Parallel P Gain/Internal Parameters'
 * '<S116>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/Reset Signal/Disabled'
 * '<S117>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/Saturation/Enabled'
 * '<S118>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/Saturation Fdbk/Disabled'
 * '<S119>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/Sum/Sum_PID'
 * '<S120>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/Sum Fdbk/Disabled'
 * '<S121>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/Tracking Mode/Disabled'
 * '<S122>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/Tracking Mode Sum/Passthrough'
 * '<S123>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/Tsamp - Integral/Passthrough'
 * '<S124>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/Tsamp - Ngain/Passthrough'
 * '<S125>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/postSat Signal/Forward_Path'
 * '<S126>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller2/preSat Signal/Forward_Path'
 * '<S127>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Anti-windup'
 * '<S128>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/D Gain'
 * '<S129>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Filter'
 * '<S130>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Filter ICs'
 * '<S131>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/I Gain'
 * '<S132>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Ideal P Gain'
 * '<S133>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Ideal P Gain Fdbk'
 * '<S134>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Integrator'
 * '<S135>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Integrator ICs'
 * '<S136>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/N Copy'
 * '<S137>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/N Gain'
 * '<S138>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/P Copy'
 * '<S139>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Parallel P Gain'
 * '<S140>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Reset Signal'
 * '<S141>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Saturation'
 * '<S142>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Saturation Fdbk'
 * '<S143>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Sum'
 * '<S144>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Sum Fdbk'
 * '<S145>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Tracking Mode'
 * '<S146>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Tracking Mode Sum'
 * '<S147>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Tsamp - Integral'
 * '<S148>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Tsamp - Ngain'
 * '<S149>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/postSat Signal'
 * '<S150>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/preSat Signal'
 * '<S151>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Anti-windup/Passthrough'
 * '<S152>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/D Gain/Internal Parameters'
 * '<S153>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Filter/Differentiator'
 * '<S154>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Filter/Differentiator/Tsamp'
 * '<S155>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Filter/Differentiator/Tsamp/Internal Ts'
 * '<S156>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Filter ICs/Internal IC - Differentiator'
 * '<S157>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/I Gain/Internal Parameters'
 * '<S158>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Ideal P Gain/Passthrough'
 * '<S159>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Ideal P Gain Fdbk/Disabled'
 * '<S160>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Integrator/Discrete'
 * '<S161>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Integrator ICs/Internal IC'
 * '<S162>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/N Copy/Disabled wSignal Specification'
 * '<S163>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/N Gain/Passthrough'
 * '<S164>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/P Copy/Disabled'
 * '<S165>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Parallel P Gain/Internal Parameters'
 * '<S166>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Reset Signal/Disabled'
 * '<S167>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Saturation/Passthrough'
 * '<S168>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Saturation Fdbk/Disabled'
 * '<S169>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Sum/Sum_PID'
 * '<S170>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Sum Fdbk/Disabled'
 * '<S171>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Tracking Mode/Disabled'
 * '<S172>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Tracking Mode Sum/Passthrough'
 * '<S173>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Tsamp - Integral/Passthrough'
 * '<S174>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/Tsamp - Ngain/Passthrough'
 * '<S175>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/postSat Signal/Forward_Path'
 * '<S176>' : 'Lander_Code_Model_2020a/PID Controller1/PID Controller3/preSat Signal/Forward_Path'
 * '<S177>' : 'Lander_Code_Model_2020a/Subsystem/Degrees to Radians'
 * '<S178>' : 'Lander_Code_Model_2020a/Subsystem/MATLAB Function'
 */
#endif                               /* RTW_HEADER_Lander_Code_Model_2020a_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
