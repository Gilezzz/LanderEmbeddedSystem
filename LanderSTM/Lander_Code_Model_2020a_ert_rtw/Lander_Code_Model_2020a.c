/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: Lander_Code_Model_2020a.c
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

/* Block signals (default storage) */
B_Lander_Code_Model_2020a_T Lander_Code_Model_2020a_B;

/* Continuous states */
X_Lander_Code_Model_2020a_T Lander_Code_Model_2020a_X;

/* Periodic continuous states */
PeriodicIndX_Lander_Code_Mode_T Lander_Code_Model__PeriodicIndX;
PeriodicRngX_Lander_Code_Mode_T Lander_Code_Model__PeriodicRngX;

/* Block states (default storage) */
DW_Lander_Code_Model_2020a_T Lander_Code_Model_2020a_DW;

/* External inputs (root inport signals with default storage) */
ExtU_Lander_Code_Model_2020a_T Lander_Code_Model_2020a_U;

/* External outputs (root outports fed by signals with default storage) */
ExtY_Lander_Code_Model_2020a_T Lander_Code_Model_2020a_Y;

/* Real-time model */
RT_MODEL_Lander_Code_Model_20_T Lander_Code_Model_2020a_M_;
RT_MODEL_Lander_Code_Model_20_T *const Lander_Code_Model_2020a_M =
  &Lander_Code_Model_2020a_M_;


/* Forward declaration for local functions */
static real_T Lander_Code_Model_2020a_norm(const real_T x[3]);
static void rt_mrdivide_U1d1x3_U2d_9vOrDY_j(const real_T u0[3], const real_T u1
  [9], real_T y[3]);

/* State reduction function */
void local_stateReduction(real_T* x, int_T* p, int_T n, real_T* r)
{
  int_T i, j;
  for (i = 0, j = 0; i < n; ++i, ++j) {
    int_T k = p[i];
    real_T lb = r[j++];
    real_T xk = x[k]-lb;
    real_T rk = r[j]-lb;
    int_T q = (int_T) floor(xk/rk);
    if (q) {
      x[k] = xk-q*rk+lb;
    }
  }
}

/*
 * This function updates continuous states using the ODE3 fixed-step
 * solver algorithm
 */
static void rt_ertODEUpdateContinuousStates(RTWSolverInfo *si )
{
  /* Solver Matrices */
  static const real_T rt_ODE3_A[3] = {
    1.0/2.0, 3.0/4.0, 1.0
  };

  static const real_T rt_ODE3_B[3][3] = {
    { 1.0/2.0, 0.0, 0.0 },

    { 0.0, 3.0/4.0, 0.0 },

    { 2.0/9.0, 1.0/3.0, 4.0/9.0 }
  };

  time_T t = rtsiGetT(si);
  time_T tnew = rtsiGetSolverStopTime(si);
  time_T h = rtsiGetStepSize(si);
  real_T *x = rtsiGetContStates(si);
  ODE3_IntgData *id = (ODE3_IntgData *)rtsiGetSolverData(si);
  real_T *y = id->y;
  real_T *f0 = id->f[0];
  real_T *f1 = id->f[1];
  real_T *f2 = id->f[2];
  real_T hB[3];
  int_T i;
  int_T nXc = 12;
  rtsiSetSimTimeStep(si,MINOR_TIME_STEP);

  /* Save the state values at time t in y, we'll use x as ynew. */
  (void) memcpy(y, x,
                (uint_T)nXc*sizeof(real_T));

  /* Assumes that rtsiSetT and ModelOutputs are up-to-date */
  /* f0 = f(t,y) */
  rtsiSetdX(si, f0);
  Lander_Code_Model_2020a_derivatives();

  /* f(:,2) = feval(odefile, t + hA(1), y + f*hB(:,1), args(:)(*)); */
  hB[0] = h * rt_ODE3_B[0][0];
  for (i = 0; i < nXc; i++) {
    x[i] = y[i] + (f0[i]*hB[0]);
  }

  rtsiSetT(si, t + h*rt_ODE3_A[0]);
  rtsiSetdX(si, f1);
  Lander_Code_Model_2020a_step();
  Lander_Code_Model_2020a_derivatives();

  /* f(:,3) = feval(odefile, t + hA(2), y + f*hB(:,2), args(:)(*)); */
  for (i = 0; i <= 1; i++) {
    hB[i] = h * rt_ODE3_B[1][i];
  }

  for (i = 0; i < nXc; i++) {
    x[i] = y[i] + (f0[i]*hB[0] + f1[i]*hB[1]);
  }

  rtsiSetT(si, t + h*rt_ODE3_A[1]);
  rtsiSetdX(si, f2);
  Lander_Code_Model_2020a_step();
  Lander_Code_Model_2020a_derivatives();

  /* tnew = t + hA(3);
     ynew = y + f*hB(:,3); */
  for (i = 0; i <= 2; i++) {
    hB[i] = h * rt_ODE3_B[2][i];
  }

  for (i = 0; i < nXc; i++) {
    x[i] = y[i] + (f0[i]*hB[0] + f1[i]*hB[1] + f2[i]*hB[2]);
  }

  rtsiSetT(si, tnew);
  local_stateReduction(x, rtsiGetPeriodicContStateIndices(si), 3,
                       rtsiGetPeriodicContStateRanges(si));
  rtsiSetSimTimeStep(si,MAJOR_TIME_STEP);


//  Lander_Code_Model_2020a_DW.  // Contains filter and update state TODO

}


/* Function for MATLAB Function: '<S5>/MATLAB Function' */
static real_T Lander_Code_Model_2020a_norm(const real_T x[3])
{
  real_T y;
  real_T scale;
  real_T absxk;
  real_T t;
  scale = 3.3121686421112381E-170;
  absxk = fabs(x[0]);
  if (absxk > 3.3121686421112381E-170) {
    y = 1.0;
    scale = absxk;
  } else {
    t = absxk / 3.3121686421112381E-170;
    y = t * t;
  }

  absxk = fabs(x[1]);
  if (absxk > scale) {
    t = scale / absxk;
    y = y * t * t + 1.0;
    scale = absxk;
  } else {
    t = absxk / scale;
    y += t * t;
  }

  absxk = fabs(x[2]);
  if (absxk > scale) {
    t = scale / absxk;
    y = y * t * t + 1.0;
    scale = absxk;
  } else {
    t = absxk / scale;
    y += t * t;
  }

  return scale * sqrt(y);
}

static void rt_mrdivide_U1d1x3_U2d_9vOrDY_j(const real_T u0[3], const real_T u1
  [9], real_T y[3])
{
  int32_T r1;
  int32_T r2;
  int32_T r3;
  real_T maxval;
  real_T a21;
  int32_T rtemp;
  memcpy(&Lander_Code_Model_2020a_B.A[0], &u1[0], 9U * sizeof(real_T));
  r1 = 0;
  r2 = 1;
  r3 = 2;
  maxval = fabs(u1[0]);
  a21 = fabs(u1[1]);
  if (a21 > maxval) {
    maxval = a21;
    r1 = 1;
    r2 = 0;
  }

  if (fabs(u1[2]) > maxval) {
    r1 = 2;
    r2 = 1;
    r3 = 0;
  }

  Lander_Code_Model_2020a_B.A[r2] = u1[r2] / u1[r1];
  Lander_Code_Model_2020a_B.A[r3] /= Lander_Code_Model_2020a_B.A[r1];
  Lander_Code_Model_2020a_B.A[r2 + 3] -= Lander_Code_Model_2020a_B.A[r1 + 3] *
    Lander_Code_Model_2020a_B.A[r2];
  Lander_Code_Model_2020a_B.A[r3 + 3] -= Lander_Code_Model_2020a_B.A[r1 + 3] *
    Lander_Code_Model_2020a_B.A[r3];
  Lander_Code_Model_2020a_B.A[r2 + 6] -= Lander_Code_Model_2020a_B.A[r1 + 6] *
    Lander_Code_Model_2020a_B.A[r2];
  Lander_Code_Model_2020a_B.A[r3 + 6] -= Lander_Code_Model_2020a_B.A[r1 + 6] *
    Lander_Code_Model_2020a_B.A[r3];
  if (fabs(Lander_Code_Model_2020a_B.A[r3 + 3]) > fabs
      (Lander_Code_Model_2020a_B.A[r2 + 3])) {
    rtemp = r2 + 1;
    r2 = r3;
    r3 = rtemp - 1;
  }

  Lander_Code_Model_2020a_B.A[r3 + 3] /= Lander_Code_Model_2020a_B.A[r2 + 3];
  Lander_Code_Model_2020a_B.A[r3 + 6] -= Lander_Code_Model_2020a_B.A[r3 + 3] *
    Lander_Code_Model_2020a_B.A[r2 + 6];
  y[r1] = u0[0] / Lander_Code_Model_2020a_B.A[r1];
  y[r2] = u0[1] - Lander_Code_Model_2020a_B.A[r1 + 3] * y[r1];
  y[r3] = u0[2] - Lander_Code_Model_2020a_B.A[r1 + 6] * y[r1];
  y[r2] /= Lander_Code_Model_2020a_B.A[r2 + 3];
  y[r3] -= Lander_Code_Model_2020a_B.A[r2 + 6] * y[r2];
  y[r3] /= Lander_Code_Model_2020a_B.A[r3 + 6];
  y[r2] -= Lander_Code_Model_2020a_B.A[r3 + 3] * y[r3];
  y[r1] -= y[r3] * Lander_Code_Model_2020a_B.A[r3];
  y[r1] -= y[r2] * Lander_Code_Model_2020a_B.A[r2];
}

/* Model step function */
void Lander_Code_Model_2020a_step(void)
{
  /* local block i/o variables */
  real_T rtb_IntegralGain_e;
  real_T rtb_IntegralGain_h;
  int32_T i;
  int32_T rtb_VectorConcatenate_tmp;
  int32_T rtb_VectorConcatenate_tmp_0;
  int32_T rtb_VectorConcatenate_tmp_1;
  static const int8_T a[3] = { 0, 0, 1 };

  static const int8_T b_a[3] = { 0, 1, 0 };

  static const int8_T c_a[3] = { 1, 0, 0 };

  static const real_T b_a_0[3] = { 1.0, 0.0, 0.0 };

  static const real_T a_0[3] = { 0.0, 1.0, 0.0 };

  static const real_T b[3] = { 0.0, 0.0, 1.0 };

  static const int8_T b_b[9] = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };

  if (rtmIsMajorTimeStep(Lander_Code_Model_2020a_M)) {
    /* set solver stop time */
    rtsiSetSolverStopTime(&Lander_Code_Model_2020a_M->solverInfo,
                          ((Lander_Code_Model_2020a_M->Timing.clockTick0+1)*
      Lander_Code_Model_2020a_M->Timing.stepSize0));
  }                                    /* end MajorTimeStep */

  /* Update absolute time of base rate at minor time step */
  if (rtmIsMinorTimeStep(Lander_Code_Model_2020a_M)) {
    Lander_Code_Model_2020a_M->Timing.t[0] = rtsiGetT
      (&Lander_Code_Model_2020a_M->solverInfo);
  }

  if (rtmIsMajorTimeStep(Lander_Code_Model_2020a_M)) {
    for (i = 0; i < 3; i++) {
      /* Concatenate: '<S8>/Vector Concatenate' incorporates:
       *  Constant: '<S8>/Constant1'
       *  Constant: '<S8>/Constant2'
       *  Selector: '<S7>/Selector1'
       */
      Lander_Code_Model_2020a_B.VectorConcatenate[6 * i] =
        Lander_Code_Model_2020a_P.I[3 * i];
      rtb_VectorConcatenate_tmp = 6 * i + 3;
      Lander_Code_Model_2020a_B.VectorConcatenate[rtb_VectorConcatenate_tmp] =
        Lander_Code_Model_2020a_P.Constant2_Value[3 * i];

      /* Selector: '<S7>/Selector' incorporates:
       *  Selector: '<S7>/Selector2'
       */
      Lander_Code_Model_2020a_B.Selector_tmp =
        Lander_Code_Model_2020a_B.VectorConcatenate[6 * i];
      Lander_Code_Model_2020a_B.Selector[3 * i] =
        Lander_Code_Model_2020a_B.Selector_tmp;

      /* Selector: '<S7>/Selector1' */
      Lander_Code_Model_2020a_B.Selector1[3 * i] =
        Lander_Code_Model_2020a_B.VectorConcatenate[rtb_VectorConcatenate_tmp];

      /* Selector: '<S7>/Selector2' */
      Lander_Code_Model_2020a_B.Selector2[3 * i] =
        Lander_Code_Model_2020a_B.Selector_tmp;

      /* Concatenate: '<S8>/Vector Concatenate' incorporates:
       *  Constant: '<S8>/Constant1'
       *  Constant: '<S8>/Constant2'
       *  Selector: '<S7>/Selector'
       *  Selector: '<S7>/Selector1'
       *  Selector: '<S7>/Selector2'
       */
      rtb_VectorConcatenate_tmp = 3 * i + 1;
      rtb_VectorConcatenate_tmp_0 = 6 * i + 1;
      Lander_Code_Model_2020a_B.VectorConcatenate[rtb_VectorConcatenate_tmp_0] =
        Lander_Code_Model_2020a_P.I[rtb_VectorConcatenate_tmp];
      rtb_VectorConcatenate_tmp_1 = 6 * i + 4;
      Lander_Code_Model_2020a_B.VectorConcatenate[rtb_VectorConcatenate_tmp_1] =
        Lander_Code_Model_2020a_P.Constant2_Value[rtb_VectorConcatenate_tmp];

      /* Selector: '<S7>/Selector' */
      Lander_Code_Model_2020a_B.Selector[rtb_VectorConcatenate_tmp] =
        Lander_Code_Model_2020a_B.VectorConcatenate[rtb_VectorConcatenate_tmp_0];

      /* Selector: '<S7>/Selector1' */
      Lander_Code_Model_2020a_B.Selector1[rtb_VectorConcatenate_tmp] =
        Lander_Code_Model_2020a_B.VectorConcatenate[rtb_VectorConcatenate_tmp_1];

      /* Selector: '<S7>/Selector2' */
      Lander_Code_Model_2020a_B.Selector2[rtb_VectorConcatenate_tmp] =
        Lander_Code_Model_2020a_B.VectorConcatenate[rtb_VectorConcatenate_tmp_0];

      /* Concatenate: '<S8>/Vector Concatenate' incorporates:
       *  Constant: '<S8>/Constant1'
       *  Constant: '<S8>/Constant2'
       *  Selector: '<S7>/Selector'
       *  Selector: '<S7>/Selector1'
       *  Selector: '<S7>/Selector2'
       */
      rtb_VectorConcatenate_tmp = 3 * i + 2;
      rtb_VectorConcatenate_tmp_0 = 6 * i + 2;
      Lander_Code_Model_2020a_B.VectorConcatenate[rtb_VectorConcatenate_tmp_0] =
        Lander_Code_Model_2020a_P.I[rtb_VectorConcatenate_tmp];
      rtb_VectorConcatenate_tmp_1 = 6 * i + 5;
      Lander_Code_Model_2020a_B.VectorConcatenate[rtb_VectorConcatenate_tmp_1] =
        Lander_Code_Model_2020a_P.Constant2_Value[rtb_VectorConcatenate_tmp];

      /* Selector: '<S7>/Selector' */
      Lander_Code_Model_2020a_B.Selector[rtb_VectorConcatenate_tmp] =
        Lander_Code_Model_2020a_B.VectorConcatenate[rtb_VectorConcatenate_tmp_0];

      /* Selector: '<S7>/Selector1' */
      Lander_Code_Model_2020a_B.Selector1[rtb_VectorConcatenate_tmp] =
        Lander_Code_Model_2020a_B.VectorConcatenate[rtb_VectorConcatenate_tmp_1];

      /* Selector: '<S7>/Selector2' */
      Lander_Code_Model_2020a_B.Selector2[rtb_VectorConcatenate_tmp] =
        Lander_Code_Model_2020a_B.VectorConcatenate[rtb_VectorConcatenate_tmp_0];
    }
//    Lander_Code_Model_2020a_P.uDOFEulerAngles_eul_0  // TODO

  }

  /* Trigonometry: '<S14>/sincos' incorporates:
   *  Integrator: '<S6>/phi theta psi'
   *  SignalConversion generated from: '<S14>/sincos'
   *  Trigonometry: '<S15>/sincos'
   */
  Lander_Code_Model_2020a_B.sincos_o2[0] = cos
    (Lander_Code_Model_2020a_X.phithetapsi_CSTATE[2]);
  Lander_Code_Model_2020a_B.rtb_Sum_p_idx_0 = sin
    (Lander_Code_Model_2020a_X.phithetapsi_CSTATE[2]);
  Lander_Code_Model_2020a_B.Selector_tmp = cos
    (Lander_Code_Model_2020a_X.phithetapsi_CSTATE[1]);
  Lander_Code_Model_2020a_B.rtb_Sum_p_idx_1_tmp = sin
    (Lander_Code_Model_2020a_X.phithetapsi_CSTATE[1]);
  Lander_Code_Model_2020a_B.rtb_sincos_o2_tmp = cos
    (Lander_Code_Model_2020a_X.phithetapsi_CSTATE[0]);
  Lander_Code_Model_2020a_B.rtb_Sum_p_tmp = sin
    (Lander_Code_Model_2020a_X.phithetapsi_CSTATE[0]);

  /* Fcn: '<S14>/Fcn11' incorporates:
   *  Trigonometry: '<S14>/sincos'
   */
  Lander_Code_Model_2020a_B.Transpose[0] =
    Lander_Code_Model_2020a_B.Selector_tmp *
    Lander_Code_Model_2020a_B.sincos_o2[0];

  /* Fcn: '<S14>/Fcn21' incorporates:
   *  Fcn: '<S14>/Fcn22'
   *  Trigonometry: '<S14>/sincos'
   */
  Lander_Code_Model_2020a_B.Sum2_idx_0 = Lander_Code_Model_2020a_B.rtb_Sum_p_tmp
    * Lander_Code_Model_2020a_B.rtb_Sum_p_idx_1_tmp;
  Lander_Code_Model_2020a_B.Transpose[1] = Lander_Code_Model_2020a_B.Sum2_idx_0 *
    Lander_Code_Model_2020a_B.sincos_o2[0] -
    Lander_Code_Model_2020a_B.rtb_sincos_o2_tmp *
    Lander_Code_Model_2020a_B.rtb_Sum_p_idx_0;

  /* Fcn: '<S14>/Fcn31' incorporates:
   *  Fcn: '<S14>/Fcn32'
   *  Trigonometry: '<S14>/sincos'
   */
  Lander_Code_Model_2020a_B.Sum2_idx_1 =
    Lander_Code_Model_2020a_B.rtb_sincos_o2_tmp *
    Lander_Code_Model_2020a_B.rtb_Sum_p_idx_1_tmp;
  Lander_Code_Model_2020a_B.Transpose[2] = Lander_Code_Model_2020a_B.Sum2_idx_1 *
    Lander_Code_Model_2020a_B.sincos_o2[0] +
    Lander_Code_Model_2020a_B.rtb_Sum_p_tmp *
    Lander_Code_Model_2020a_B.rtb_Sum_p_idx_0;

  /* Fcn: '<S14>/Fcn12' incorporates:
   *  Trigonometry: '<S14>/sincos'
   */
  Lander_Code_Model_2020a_B.Transpose[3] =
    Lander_Code_Model_2020a_B.Selector_tmp *
    Lander_Code_Model_2020a_B.rtb_Sum_p_idx_0;

  /* Fcn: '<S14>/Fcn22' incorporates:
   *  Trigonometry: '<S14>/sincos'
   */
  Lander_Code_Model_2020a_B.Transpose[4] = Lander_Code_Model_2020a_B.Sum2_idx_0 *
    Lander_Code_Model_2020a_B.rtb_Sum_p_idx_0 +
    Lander_Code_Model_2020a_B.rtb_sincos_o2_tmp *
    Lander_Code_Model_2020a_B.sincos_o2[0];

  /* Fcn: '<S14>/Fcn32' incorporates:
   *  Trigonometry: '<S14>/sincos'
   */
  Lander_Code_Model_2020a_B.Transpose[5] = Lander_Code_Model_2020a_B.Sum2_idx_1 *
    Lander_Code_Model_2020a_B.rtb_Sum_p_idx_0 -
    Lander_Code_Model_2020a_B.rtb_Sum_p_tmp *
    Lander_Code_Model_2020a_B.sincos_o2[0];

  /* Fcn: '<S14>/Fcn13' incorporates:
   *  Trigonometry: '<S14>/sincos'
   */
  Lander_Code_Model_2020a_B.Transpose[6] =
    -Lander_Code_Model_2020a_B.rtb_Sum_p_idx_1_tmp;

  /* Fcn: '<S14>/Fcn23' incorporates:
   *  Trigonometry: '<S14>/sincos'
   */
  Lander_Code_Model_2020a_B.Transpose[7] =
    Lander_Code_Model_2020a_B.rtb_Sum_p_tmp *
    Lander_Code_Model_2020a_B.Selector_tmp;

  /* Fcn: '<S14>/Fcn33' incorporates:
   *  Trigonometry: '<S14>/sincos'
   */
  Lander_Code_Model_2020a_B.Transpose[8] =
    Lander_Code_Model_2020a_B.rtb_sincos_o2_tmp *
    Lander_Code_Model_2020a_B.Selector_tmp;

  /* Math: '<S1>/Transpose' */
  for (i = 0; i < 3; i++) {
    Lander_Code_Model_2020a_B.rtb_Transpose_m[3 * i] =
      Lander_Code_Model_2020a_B.Transpose[i];
    Lander_Code_Model_2020a_B.rtb_Transpose_m[3 * i + 1] =
      Lander_Code_Model_2020a_B.Transpose[i + 3];
    Lander_Code_Model_2020a_B.rtb_Transpose_m[3 * i + 2] =
      Lander_Code_Model_2020a_B.Transpose[i + 6];
  }

  memcpy(&Lander_Code_Model_2020a_B.Transpose[0],
         &Lander_Code_Model_2020a_B.rtb_Transpose_m[0], 9U * sizeof(real_T));

  /* End of Math: '<S1>/Transpose' */

  /* MATLAB Function: '<S3>/MATLAB Function1' */
  Lander_Code_Model_2020a_B.Cospsi = 0.0;
  Lander_Code_Model_2020a_B.Costheta = 0.0;
  Lander_Code_Model_2020a_B.rtb_Sum_p_idx_0 = 0.0;
  for (rtb_VectorConcatenate_tmp_0 = 0; rtb_VectorConcatenate_tmp_0 < 3;
       rtb_VectorConcatenate_tmp_0++) {
    /* Product: '<S13>/Product' incorporates:
     *  Integrator: '<S1>/ub,vb,wb'
     */
    Lander_Code_Model_2020a_B.Product[rtb_VectorConcatenate_tmp_0] = 0.0;
    Lander_Code_Model_2020a_B.Product[rtb_VectorConcatenate_tmp_0] +=
      Lander_Code_Model_2020a_B.Transpose[rtb_VectorConcatenate_tmp_0] *
      Lander_Code_Model_2020a_X.ubvbwb_CSTATE[0];
    Lander_Code_Model_2020a_B.Product[rtb_VectorConcatenate_tmp_0] +=
      Lander_Code_Model_2020a_B.Transpose[rtb_VectorConcatenate_tmp_0 + 3] *
      Lander_Code_Model_2020a_X.ubvbwb_CSTATE[1];
    Lander_Code_Model_2020a_B.Product[rtb_VectorConcatenate_tmp_0] +=
      Lander_Code_Model_2020a_B.Transpose[rtb_VectorConcatenate_tmp_0 + 6] *
      Lander_Code_Model_2020a_X.ubvbwb_CSTATE[2];

    /* MATLAB Function: '<S3>/MATLAB Function1' */
    Lander_Code_Model_2020a_B.Cospsi += (real_T)(a[rtb_VectorConcatenate_tmp_0] *
      a[rtb_VectorConcatenate_tmp_0]);
    Lander_Code_Model_2020a_B.Costheta += (real_T)
      (b_a[rtb_VectorConcatenate_tmp_0] * a[rtb_VectorConcatenate_tmp_0]);
    Lander_Code_Model_2020a_B.rtb_Sum_p_idx_0 += (real_T)
      (c_a[rtb_VectorConcatenate_tmp_0] * a[rtb_VectorConcatenate_tmp_0]);
  }

  /* Fcn: '<S2>/Fcn' incorporates:
   *  Inport: '<Root>/Thrust'
   */
  Lander_Code_Model_2020a_B.Fcn = (real_T)(Lander_Code_Model_2020a_U.Thrust ==
    0.0) * 2.2204460492503131e-16 + Lander_Code_Model_2020a_U.Thrust;

  /* Sum: '<Root>/Sum2' incorporates:
   *  Inport: '<Root>/Reference Euler Angles'
   *  Integrator: '<S6>/phi theta psi'
   */
  Lander_Code_Model_2020a_B.Sum2_idx_0 =
    Lander_Code_Model_2020a_U.ReferenceEulerAngles -
    Lander_Code_Model_2020a_X.phithetapsi_CSTATE[0];
  Lander_Code_Model_2020a_B.Sum2_idx_1 =
    Lander_Code_Model_2020a_U.ReferenceEulerAngles -
    Lander_Code_Model_2020a_X.phithetapsi_CSTATE[1];
  Lander_Code_Model_2020a_B.Sum2_idx_2 =
    Lander_Code_Model_2020a_U.ReferenceEulerAngles -
    Lander_Code_Model_2020a_X.phithetapsi_CSTATE[2];
  if (rtmIsMajorTimeStep(Lander_Code_Model_2020a_M)) {
    /* Product: '<S3>/Gravitational Force' incorporates:
     *  Constant: '<S3>/Mass (kg)'
     *  Constant: '<S3>/g'
     */
    Lander_Code_Model_2020a_B.GravitationalForce =
      Lander_Code_Model_2020a_P.g_Value * Lander_Code_Model_2020a_P.Masskg_Value;

    /* SampleTimeMath: '<S155>/Tsamp' incorporates:
     *  Gain: '<S152>/Derivative Gain'
     *
     * About '<S155>/Tsamp':
     *  y = u * K where K = 1 / ( w * Ts )
     */
    Lander_Code_Model_2020a_B.Tsamp = Lander_Code_Model_2020a_P.PIDController3_D
      * Lander_Code_Model_2020a_B.Sum2_idx_1 *
      Lander_Code_Model_2020a_P.Tsamp_WtEt;

    /* Sum: '<S169>/Sum' incorporates:
     *  Delay: '<S153>/UD'
     *  DiscreteIntegrator: '<S160>/Integrator'
     *  Gain: '<S165>/Proportional Gain'
     *  Sum: '<S153>/Diff'
     */
    Lander_Code_Model_2020a_B.MatrixConcatenate[0] =
      (Lander_Code_Model_2020a_P.PIDController3_P *
       Lander_Code_Model_2020a_B.Sum2_idx_1 +
       Lander_Code_Model_2020a_DW.Integrator_DSTATE) +
      (Lander_Code_Model_2020a_B.Tsamp - Lander_Code_Model_2020a_DW.UD_DSTATE);

    /* SampleTimeMath: '<S57>/Tsamp' incorporates:
     *  Gain: '<S54>/Derivative Gain'
     *
     * About '<S57>/Tsamp':
     *  y = u * K where K = 1 / ( w * Ts )
     */
    Lander_Code_Model_2020a_B.Tsamp_j =
      Lander_Code_Model_2020a_P.PIDController1_D *
      Lander_Code_Model_2020a_B.Sum2_idx_0 *
      Lander_Code_Model_2020a_P.Tsamp_WtEt_n;

    /* Sum: '<S71>/Sum' incorporates:
     *  Delay: '<S55>/UD'
     *  DiscreteIntegrator: '<S62>/Integrator'
     *  Gain: '<S67>/Proportional Gain'
     *  Sum: '<S55>/Diff'
     */
    Lander_Code_Model_2020a_B.MatrixConcatenate[1] =
      (Lander_Code_Model_2020a_P.PIDController1_P *
       Lander_Code_Model_2020a_B.Sum2_idx_0 +
       Lander_Code_Model_2020a_DW.Integrator_DSTATE_m) +
      (Lander_Code_Model_2020a_B.Tsamp_j -
       Lander_Code_Model_2020a_DW.UD_DSTATE_k);

    /* Gain: '<S113>/Filter Coefficient' incorporates:
     *  DiscreteIntegrator: '<S105>/Filter'
     *  Gain: '<S104>/Derivative Gain'
     *  Sum: '<S105>/SumD'
     */
    Lander_Code_Model_2020a_B.FilterCoefficient =
      (Lander_Code_Model_2020a_P.PIDController2_D *
       Lander_Code_Model_2020a_B.Sum2_idx_2 -
       Lander_Code_Model_2020a_DW.Filter_DSTATE) *
      Lander_Code_Model_2020a_P.PIDController2_N;

    /* Sum: '<S119>/Sum' incorporates:
     *  DiscreteIntegrator: '<S110>/Integrator'
     *  Gain: '<S115>/Proportional Gain'
     */
    Lander_Code_Model_2020a_B.Gain = (Lander_Code_Model_2020a_P.PIDController2_P
      * Lander_Code_Model_2020a_B.Sum2_idx_2 +
      Lander_Code_Model_2020a_DW.Integrator_DSTATE_g) +
      Lander_Code_Model_2020a_B.FilterCoefficient;

    /* Saturate: '<S117>/Saturation' */
    if (Lander_Code_Model_2020a_B.Gain >
        Lander_Code_Model_2020a_P.PIDController2_UpperSaturationL) {
      Lander_Code_Model_2020a_B.MatrixConcatenate[2] =
        Lander_Code_Model_2020a_P.PIDController2_UpperSaturationL;
    } else if (Lander_Code_Model_2020a_B.Gain <
               Lander_Code_Model_2020a_P.PIDController2_LowerSaturationL) {
      Lander_Code_Model_2020a_B.MatrixConcatenate[2] =
        Lander_Code_Model_2020a_P.PIDController2_LowerSaturationL;
    } else {
      Lander_Code_Model_2020a_B.MatrixConcatenate[2] =
        Lander_Code_Model_2020a_B.Gain;
    }

    /* End of Saturate: '<S117>/Saturation' */

    /* Product: '<S2>/Product3' incorporates:
     *  Constant: '<S2>/l_r2cg1'
     */
    Lander_Code_Model_2020a_B.Product3[0] =
      Lander_Code_Model_2020a_B.MatrixConcatenate[0] /
      Lander_Code_Model_2020a_P.l_r2cg;
    Lander_Code_Model_2020a_B.Product3[1] =
      Lander_Code_Model_2020a_B.MatrixConcatenate[1] /
      Lander_Code_Model_2020a_P.l_r2cg;
  }

  /* Product: '<S2>/Product1' */
  Lander_Code_Model_2020a_B.Gain = Lander_Code_Model_2020a_B.Product3[0] /
    Lander_Code_Model_2020a_B.Fcn;

  /* Trigonometry: '<S2>/Trigonometric Function1' */
  if (Lander_Code_Model_2020a_B.Gain > 1.0) {
    Lander_Code_Model_2020a_B.Gain = 1.0;
  } else {
    if (Lander_Code_Model_2020a_B.Gain < -1.0) {
      Lander_Code_Model_2020a_B.Gain = -1.0;
    }
  }

  /* Gain: '<S24>/Gain' incorporates:
   *  Trigonometry: '<S2>/Trigonometric Function1'
   */
  Lander_Code_Model_2020a_B.Gain = Lander_Code_Model_2020a_P.Gain_Gain * asin
    (Lander_Code_Model_2020a_B.Gain);

  /* Product: '<S2>/Product1' */
  Lander_Code_Model_2020a_B.Fcn = Lander_Code_Model_2020a_B.Product3[1] /
    Lander_Code_Model_2020a_B.Fcn;

  /* Trigonometry: '<S2>/Trigonometric Function1' */
  if (Lander_Code_Model_2020a_B.Fcn > 1.0) {
    Lander_Code_Model_2020a_B.Fcn = 1.0;
  } else {
    if (Lander_Code_Model_2020a_B.Fcn < -1.0) {
      Lander_Code_Model_2020a_B.Fcn = -1.0;
    }
  }

  /* Gain: '<S24>/Gain' incorporates:
   *  Trigonometry: '<S2>/Trigonometric Function1'
   */
  Lander_Code_Model_2020a_B.Fcn = Lander_Code_Model_2020a_P.Gain_Gain * asin
    (Lander_Code_Model_2020a_B.Fcn);

  /* Saturate: '<S2>/Saturation1' */
  if (Lander_Code_Model_2020a_B.Fcn >
      Lander_Code_Model_2020a_P.Saturation1_UpperSat) {
    Lander_Code_Model_2020a_B.Fcn =
      Lander_Code_Model_2020a_P.Saturation1_UpperSat;
  } else {
    if (Lander_Code_Model_2020a_B.Fcn <
        Lander_Code_Model_2020a_P.Saturation1_LowerSat) {
      Lander_Code_Model_2020a_B.Fcn =
        Lander_Code_Model_2020a_P.Saturation1_LowerSat;
    }
  }

  if (Lander_Code_Model_2020a_B.Gain >
      Lander_Code_Model_2020a_P.Saturation1_UpperSat) {
    Lander_Code_Model_2020a_B.Gain =
      Lander_Code_Model_2020a_P.Saturation1_UpperSat;
  } else {
    if (Lander_Code_Model_2020a_B.Gain <
        Lander_Code_Model_2020a_P.Saturation1_LowerSat) {
      Lander_Code_Model_2020a_B.Gain =
        Lander_Code_Model_2020a_P.Saturation1_LowerSat;
    }
  }

  /* End of Saturate: '<S2>/Saturation1' */

  /* Gain: '<S2>/Gain' */
  Lander_Code_Model_2020a_B.Gain *= Lander_Code_Model_2020a_P.Gain_Gain_j;

  /* Gain: '<S177>/Gain1' */
  Lander_Code_Model_2020a_B.rtb_Gain1_idx_0 =
    Lander_Code_Model_2020a_P.Gain1_Gain * Lander_Code_Model_2020a_B.Gain;
  Lander_Code_Model_2020a_B.rtb_Gain1_idx_1 =
    Lander_Code_Model_2020a_P.Gain1_Gain * Lander_Code_Model_2020a_B.Fcn;

  /* MATLAB Function: '<S5>/MATLAB Function' */
  Lander_Code_Model_2020a_B.F_r_tmp = sin
    (Lander_Code_Model_2020a_B.rtb_Gain1_idx_1);
  Lander_Code_Model_2020a_B.rtb_Gain1_idx_1 = cos
    (Lander_Code_Model_2020a_B.rtb_Gain1_idx_1);
  Lander_Code_Model_2020a_B.F_r_tmp_c = sin
    (Lander_Code_Model_2020a_B.rtb_Gain1_idx_0);
  Lander_Code_Model_2020a_B.rtb_Gain1_idx_0 = cos
    (Lander_Code_Model_2020a_B.rtb_Gain1_idx_0);
  Lander_Code_Model_2020a_B.Transpose[1] = 0.0;
  Lander_Code_Model_2020a_B.Transpose[4] =
    Lander_Code_Model_2020a_B.rtb_Gain1_idx_1;
  Lander_Code_Model_2020a_B.Transpose[7] = Lander_Code_Model_2020a_B.F_r_tmp;
  Lander_Code_Model_2020a_B.Transpose[2] = 0.0;
  Lander_Code_Model_2020a_B.Transpose[5] = -Lander_Code_Model_2020a_B.F_r_tmp;
  Lander_Code_Model_2020a_B.Transpose[8] =
    Lander_Code_Model_2020a_B.rtb_Gain1_idx_1;
  Lander_Code_Model_2020a_B.rtb_Transpose_m[0] =
    Lander_Code_Model_2020a_B.rtb_Gain1_idx_0;
  Lander_Code_Model_2020a_B.rtb_Transpose_m[3] = 0.0;
  Lander_Code_Model_2020a_B.rtb_Transpose_m[6] =
    -Lander_Code_Model_2020a_B.F_r_tmp_c;
  Lander_Code_Model_2020a_B.Transpose[0] = 1.0;
  Lander_Code_Model_2020a_B.rtb_Transpose_m[1] = 0.0;
  Lander_Code_Model_2020a_B.Transpose[3] = 0.0;
  Lander_Code_Model_2020a_B.rtb_Transpose_m[4] = 1.0;
  Lander_Code_Model_2020a_B.Transpose[6] = 0.0;
  Lander_Code_Model_2020a_B.rtb_Transpose_m[7] = 0.0;
  Lander_Code_Model_2020a_B.rtb_Transpose_m[2] =
    Lander_Code_Model_2020a_B.F_r_tmp_c;
  Lander_Code_Model_2020a_B.rtb_Transpose_m[5] = 0.0;
  Lander_Code_Model_2020a_B.rtb_Transpose_m[8] =
    Lander_Code_Model_2020a_B.rtb_Gain1_idx_0;
  for (i = 0; i < 3; i++) {
    for (rtb_VectorConcatenate_tmp_0 = 0; rtb_VectorConcatenate_tmp_0 < 3;
         rtb_VectorConcatenate_tmp_0++) {
      rtb_VectorConcatenate_tmp = rtb_VectorConcatenate_tmp_0 + 3 * i;
      Lander_Code_Model_2020a_B.d[rtb_VectorConcatenate_tmp] = 0.0;
      Lander_Code_Model_2020a_B.d[rtb_VectorConcatenate_tmp] +=
        Lander_Code_Model_2020a_B.rtb_Transpose_m[3 * i] *
        Lander_Code_Model_2020a_B.Transpose[rtb_VectorConcatenate_tmp_0];
      Lander_Code_Model_2020a_B.d[rtb_VectorConcatenate_tmp] +=
        Lander_Code_Model_2020a_B.rtb_Transpose_m[3 * i + 1] *
        Lander_Code_Model_2020a_B.Transpose[rtb_VectorConcatenate_tmp_0 + 3];
      Lander_Code_Model_2020a_B.d[rtb_VectorConcatenate_tmp] +=
        Lander_Code_Model_2020a_B.rtb_Transpose_m[3 * i + 2] *
        Lander_Code_Model_2020a_B.Transpose[rtb_VectorConcatenate_tmp_0 + 6];
    }
  }

  Lander_Code_Model_2020a_B.F_r_tmp = 0.0;
  Lander_Code_Model_2020a_B.rtb_Gain1_idx_1 = 0.0;
  Lander_Code_Model_2020a_B.F_r_tmp_c = 0.0;
  for (rtb_VectorConcatenate_tmp_0 = 0; rtb_VectorConcatenate_tmp_0 < 3;
       rtb_VectorConcatenate_tmp_0++) {
    Lander_Code_Model_2020a_B.sincos_o2[rtb_VectorConcatenate_tmp_0] = 0.0;
    for (i = 0; i < 3; i++) {
      rtb_VectorConcatenate_tmp = rtb_VectorConcatenate_tmp_0 + 3 * i;
      Lander_Code_Model_2020a_B.Transpose[rtb_VectorConcatenate_tmp] = 0.0;
      Lander_Code_Model_2020a_B.Transpose[rtb_VectorConcatenate_tmp] += (real_T)
        b_b[3 * rtb_VectorConcatenate_tmp_0] * Lander_Code_Model_2020a_B.d[i];
      Lander_Code_Model_2020a_B.Transpose[rtb_VectorConcatenate_tmp] += (real_T)
        b_b[3 * rtb_VectorConcatenate_tmp_0 + 1] * Lander_Code_Model_2020a_B.d[i
        + 3];
      Lander_Code_Model_2020a_B.Transpose[rtb_VectorConcatenate_tmp] += (real_T)
        b_b[3 * rtb_VectorConcatenate_tmp_0 + 2] * Lander_Code_Model_2020a_B.d[i
        + 6];
      Lander_Code_Model_2020a_B.sincos_o2[rtb_VectorConcatenate_tmp_0] +=
        Lander_Code_Model_2020a_B.Transpose[rtb_VectorConcatenate_tmp] * b[i];
    }

    Lander_Code_Model_2020a_B.F_r_tmp += b[rtb_VectorConcatenate_tmp_0] *
      Lander_Code_Model_2020a_B.sincos_o2[rtb_VectorConcatenate_tmp_0];
    Lander_Code_Model_2020a_B.rtb_Gain1_idx_1 += a_0[rtb_VectorConcatenate_tmp_0]
      * Lander_Code_Model_2020a_B.sincos_o2[rtb_VectorConcatenate_tmp_0];
    Lander_Code_Model_2020a_B.F_r_tmp_c += b_a_0[rtb_VectorConcatenate_tmp_0] *
      Lander_Code_Model_2020a_B.sincos_o2[rtb_VectorConcatenate_tmp_0];
  }

  Lander_Code_Model_2020a_B.rtb_Gain1_idx_0 = Lander_Code_Model_2020a_norm
    (Lander_Code_Model_2020a_B.sincos_o2);

  /* Sum: '<S5>/Sum' incorporates:
   *  Constant: '<S3>/Constant'
   *  Inport: '<Root>/Thrust'
   *  MATLAB Function: '<S3>/MATLAB Function1'
   *  MATLAB Function: '<S5>/MATLAB Function'
   *  Product: '<S3>/Multiply'
   *  Product: '<S5>/Product'
   *  Sum: '<S3>/Summing Forces on Lander'
   */
  Lander_Code_Model_2020a_B.rtb_Sum_p_idx_0 =
    Lander_Code_Model_2020a_B.F_r_tmp_c / (Lander_Code_Model_2020a_norm(b_a_0) *
    Lander_Code_Model_2020a_B.rtb_Gain1_idx_0) *
    Lander_Code_Model_2020a_U.Thrust +
    (Lander_Code_Model_2020a_B.rtb_Sum_p_idx_0 *
     Lander_Code_Model_2020a_B.GravitationalForce +
     Lander_Code_Model_2020a_P.Constant_Value);
  Lander_Code_Model_2020a_B.Costheta = Lander_Code_Model_2020a_B.rtb_Gain1_idx_1
    / (Lander_Code_Model_2020a_norm(a_0) *
       Lander_Code_Model_2020a_B.rtb_Gain1_idx_0) *
    Lander_Code_Model_2020a_U.Thrust + (Lander_Code_Model_2020a_B.Costheta *
    Lander_Code_Model_2020a_B.GravitationalForce +
    Lander_Code_Model_2020a_P.Constant_Value);

  /* Sum: '<S1>/Sum' incorporates:
   *  Constant: '<S3>/Density (kg//m^3)1'
   *  Constant: '<S3>/Drag Coefficient1'
   *  Constant: '<S3>/Wetted Area (m^2)1'
   *  Constant: '<S8>/Constant'
   *  Gain: '<S3>/Gain3'
   *  Inport: '<Root>/Thrust'
   *  Integrator: '<S1>/p,q,r '
   *  Integrator: '<S1>/ub,vb,wb'
   *  MATLAB Function: '<S3>/MATLAB Function1'
   *  MATLAB Function: '<S5>/MATLAB Function'
   *  Math: '<S3>/Vz^2'
   *  Product: '<S1>/Product'
   *  Product: '<S22>/i x j'
   *  Product: '<S22>/j x k'
   *  Product: '<S22>/k x i'
   *  Product: '<S23>/i x k'
   *  Product: '<S23>/j x i'
   *  Product: '<S23>/k x j'
   *  Product: '<S3>/Aero Drag Eqn1'
   *  Product: '<S3>/Multiply'
   *  Product: '<S5>/Product'
   *  Sum: '<S3>/Summing Forces on Lander'
   *  Sum: '<S5>/Sum'
   *  Sum: '<S9>/Sum'
   */
  Lander_Code_Model_2020a_B.Sum[0] = (Lander_Code_Model_2020a_X.ubvbwb_CSTATE[1]
    * Lander_Code_Model_2020a_X.pqr_CSTATE[2] -
    Lander_Code_Model_2020a_X.ubvbwb_CSTATE[2] *
    Lander_Code_Model_2020a_X.pqr_CSTATE[1]) +
    Lander_Code_Model_2020a_B.rtb_Sum_p_idx_0 /
    Lander_Code_Model_2020a_P.uDOFEulerAngles_mass_0;
  Lander_Code_Model_2020a_B.Sum[1] = (Lander_Code_Model_2020a_X.ubvbwb_CSTATE[2]
    * Lander_Code_Model_2020a_X.pqr_CSTATE[0] -
    Lander_Code_Model_2020a_X.ubvbwb_CSTATE[0] *
    Lander_Code_Model_2020a_X.pqr_CSTATE[2]) +
    Lander_Code_Model_2020a_B.Costheta /
    Lander_Code_Model_2020a_P.uDOFEulerAngles_mass_0;
  Lander_Code_Model_2020a_B.Sum[2] = ((Lander_Code_Model_2020a_P.Cd *
    Lander_Code_Model_2020a_P.Awet * Lander_Code_Model_2020a_P.rho *
    (Lander_Code_Model_2020a_B.Product[2] * Lander_Code_Model_2020a_B.Product[2])
    * Lander_Code_Model_2020a_P.Gain3_Gain + Lander_Code_Model_2020a_B.Cospsi *
    Lander_Code_Model_2020a_B.GravitationalForce) +
    Lander_Code_Model_2020a_B.F_r_tmp / (Lander_Code_Model_2020a_norm(b) *
    Lander_Code_Model_2020a_B.rtb_Gain1_idx_0) *
    Lander_Code_Model_2020a_U.Thrust) /
    Lander_Code_Model_2020a_P.uDOFEulerAngles_mass_0 +
    (Lander_Code_Model_2020a_X.ubvbwb_CSTATE[0] *
     Lander_Code_Model_2020a_X.pqr_CSTATE[1] -
     Lander_Code_Model_2020a_X.ubvbwb_CSTATE[1] *
     Lander_Code_Model_2020a_X.pqr_CSTATE[0]);
  for (i = 0; i < 3; i++) {
    /* Product: '<S18>/Product' incorporates:
     *  Integrator: '<S1>/p,q,r '
     */
    Lander_Code_Model_2020a_B.sincos_o2[i] =
      Lander_Code_Model_2020a_B.Selector[i + 6] *
      Lander_Code_Model_2020a_X.pqr_CSTATE[2] +
      (Lander_Code_Model_2020a_B.Selector[i + 3] *
       Lander_Code_Model_2020a_X.pqr_CSTATE[1] +
       Lander_Code_Model_2020a_B.Selector[i] *
       Lander_Code_Model_2020a_X.pqr_CSTATE[0]);

    /* Product: '<S19>/Product' incorporates:
     *  Integrator: '<S1>/p,q,r '
     */
    Lander_Code_Model_2020a_B.sincos_o1[i] =
      Lander_Code_Model_2020a_B.Selector1[i + 6] *
      Lander_Code_Model_2020a_X.pqr_CSTATE[2] +
      (Lander_Code_Model_2020a_B.Selector1[i + 3] *
       Lander_Code_Model_2020a_X.pqr_CSTATE[1] +
       Lander_Code_Model_2020a_B.Selector1[i] *
       Lander_Code_Model_2020a_X.pqr_CSTATE[0]);
  }

  /* Sum: '<S17>/Sum' incorporates:
   *  Integrator: '<S1>/p,q,r '
   *  Product: '<S20>/i x j'
   *  Product: '<S20>/j x k'
   *  Product: '<S20>/k x i'
   *  Product: '<S21>/i x k'
   *  Product: '<S21>/j x i'
   *  Product: '<S21>/k x j'
   */
  Lander_Code_Model_2020a_B.Cospsi = Lander_Code_Model_2020a_X.pqr_CSTATE[1] *
    Lander_Code_Model_2020a_B.sincos_o2[2];
  Lander_Code_Model_2020a_B.F_r_tmp = Lander_Code_Model_2020a_X.pqr_CSTATE[2] *
    Lander_Code_Model_2020a_B.sincos_o2[0];
  Lander_Code_Model_2020a_B.rtb_Gain1_idx_0 =
    Lander_Code_Model_2020a_X.pqr_CSTATE[0] *
    Lander_Code_Model_2020a_B.sincos_o2[1];
  Lander_Code_Model_2020a_B.rtb_Gain1_idx_1 =
    Lander_Code_Model_2020a_X.pqr_CSTATE[2] *
    Lander_Code_Model_2020a_B.sincos_o2[1];
  Lander_Code_Model_2020a_B.F_r_tmp_c = Lander_Code_Model_2020a_X.pqr_CSTATE[0] *
    Lander_Code_Model_2020a_B.sincos_o2[2];
  Lander_Code_Model_2020a_B.unnamed_idx_2 =
    Lander_Code_Model_2020a_X.pqr_CSTATE[1] *
    Lander_Code_Model_2020a_B.sincos_o2[0];

  /* Sum: '<S7>/Sum2' incorporates:
   *  Constant: '<S5>/Constant'
   *  Product: '<S5>/Product1'
   *  Product: '<S5>/Product2'
   *  Sum: '<S17>/Sum'
   */
  Lander_Code_Model_2020a_B.d_k = Lander_Code_Model_2020a_B.sincos_o1[0];
  Lander_Code_Model_2020a_B.d1 = Lander_Code_Model_2020a_B.sincos_o1[1];
  Lander_Code_Model_2020a_B.d2 = Lander_Code_Model_2020a_B.sincos_o1[2];
  Lander_Code_Model_2020a_B.sincos_o1[0] = (Lander_Code_Model_2020a_P.l_r2cg *
    Lander_Code_Model_2020a_B.Costheta - Lander_Code_Model_2020a_B.d_k) -
    (Lander_Code_Model_2020a_B.Cospsi -
     Lander_Code_Model_2020a_B.rtb_Gain1_idx_1);
  Lander_Code_Model_2020a_B.sincos_o1[1] = (Lander_Code_Model_2020a_P.l_r2cg *
    Lander_Code_Model_2020a_B.rtb_Sum_p_idx_0 - Lander_Code_Model_2020a_B.d1) -
    (Lander_Code_Model_2020a_B.F_r_tmp - Lander_Code_Model_2020a_B.F_r_tmp_c);
  Lander_Code_Model_2020a_B.sincos_o1[2] =
    (Lander_Code_Model_2020a_B.MatrixConcatenate[2] -
     Lander_Code_Model_2020a_B.d2) - (Lander_Code_Model_2020a_B.rtb_Gain1_idx_0
    - Lander_Code_Model_2020a_B.unnamed_idx_2);

  /* Product: '<S7>/Product2' */
  rt_mrdivide_U1d1x3_U2d_9vOrDY_j(Lander_Code_Model_2020a_B.sincos_o1,
    Lander_Code_Model_2020a_B.Selector2, Lander_Code_Model_2020a_B.Product2);

  /* Fcn: '<S15>/phidot' incorporates:
   *  Fcn: '<S15>/psidot'
   *  Integrator: '<S1>/p,q,r '
   */
  Lander_Code_Model_2020a_B.rtb_Sum_p_idx_0 =
    Lander_Code_Model_2020a_X.pqr_CSTATE[1] *
    Lander_Code_Model_2020a_B.rtb_Sum_p_tmp;

  /* SignalConversion generated from: '<S6>/phi theta psi' incorporates:
   *  Fcn: '<S15>/phidot'
   *  Fcn: '<S15>/psidot'
   *  Fcn: '<S15>/thetadot'
   *  Integrator: '<S1>/p,q,r '
   */
  Lander_Code_Model_2020a_B.TmpSignalConversionAtphithetaps[0] =
    (Lander_Code_Model_2020a_B.rtb_Sum_p_idx_0 +
     Lander_Code_Model_2020a_X.pqr_CSTATE[2] *
     Lander_Code_Model_2020a_B.rtb_sincos_o2_tmp) *
    (Lander_Code_Model_2020a_B.rtb_Sum_p_idx_1_tmp /
     Lander_Code_Model_2020a_B.Selector_tmp) +
    Lander_Code_Model_2020a_X.pqr_CSTATE[0];
  Lander_Code_Model_2020a_B.TmpSignalConversionAtphithetaps[1] =
    Lander_Code_Model_2020a_X.pqr_CSTATE[1] *
    Lander_Code_Model_2020a_B.rtb_sincos_o2_tmp -
    Lander_Code_Model_2020a_X.pqr_CSTATE[2] *
    Lander_Code_Model_2020a_B.rtb_Sum_p_tmp;
  Lander_Code_Model_2020a_B.TmpSignalConversionAtphithetaps[2] =
    (Lander_Code_Model_2020a_B.rtb_Sum_p_idx_0 +
     Lander_Code_Model_2020a_X.pqr_CSTATE[2] *
     Lander_Code_Model_2020a_B.rtb_sincos_o2_tmp) /
    Lander_Code_Model_2020a_B.Selector_tmp;

  /* Outport: '<Root>/Gimble_Angles' */
  Lander_Code_Model_2020a_Y.Gimble_Angles[0] = Lander_Code_Model_2020a_B.Gain;
  Lander_Code_Model_2020a_Y.Gimble_Angles[1] = Lander_Code_Model_2020a_B.Fcn;
  if (rtmIsMajorTimeStep(Lander_Code_Model_2020a_M)) {
    /* Gain: '<S59>/Integral Gain' */
    Lander_Code_Model_2020a_B.IntegralGain =
      Lander_Code_Model_2020a_P.PIDController1_I *
      Lander_Code_Model_2020a_B.Sum2_idx_0;

    /* Gain: '<S107>/Integral Gain' */
    rtb_IntegralGain_e = Lander_Code_Model_2020a_P.PIDController2_I *
      Lander_Code_Model_2020a_B.Sum2_idx_2;

    /* Gain: '<S157>/Integral Gain' */
    rtb_IntegralGain_h = Lander_Code_Model_2020a_P.PIDController3_I *
      Lander_Code_Model_2020a_B.Sum2_idx_1;

    /* Outport: '<Root>/Reaction Wheel Torque' */
    Lander_Code_Model_2020a_Y.ReactionWheelTorque =
      Lander_Code_Model_2020a_B.MatrixConcatenate[2];
  }

  if (rtmIsMajorTimeStep(Lander_Code_Model_2020a_M)) {
    if (rtmIsMajorTimeStep(Lander_Code_Model_2020a_M)) {
      /* Update for DiscreteIntegrator: '<S160>/Integrator' */
      Lander_Code_Model_2020a_DW.Integrator_DSTATE +=
        Lander_Code_Model_2020a_P.Integrator_gainval * rtb_IntegralGain_h;

      /* Update for Delay: '<S153>/UD' */
      Lander_Code_Model_2020a_DW.UD_DSTATE = Lander_Code_Model_2020a_B.Tsamp;

      /* Update for DiscreteIntegrator: '<S62>/Integrator' */
      Lander_Code_Model_2020a_DW.Integrator_DSTATE_m +=
        Lander_Code_Model_2020a_P.Integrator_gainval_h *
        Lander_Code_Model_2020a_B.IntegralGain;

      /* Update for Delay: '<S55>/UD' */
      Lander_Code_Model_2020a_DW.UD_DSTATE_k = Lander_Code_Model_2020a_B.Tsamp_j;

      /* Update for DiscreteIntegrator: '<S110>/Integrator' */
      Lander_Code_Model_2020a_DW.Integrator_DSTATE_g +=
        Lander_Code_Model_2020a_P.Integrator_gainval_o * rtb_IntegralGain_e;

      /* Update for DiscreteIntegrator: '<S105>/Filter' */
      Lander_Code_Model_2020a_DW.Filter_DSTATE +=
        Lander_Code_Model_2020a_P.Filter_gainval *
        Lander_Code_Model_2020a_B.FilterCoefficient;
    }
  }                                    /* end MajorTimeStep */

  if (rtmIsMajorTimeStep(Lander_Code_Model_2020a_M)) {
    rt_ertODEUpdateContinuousStates(&Lander_Code_Model_2020a_M->solverInfo);

    /* Update absolute time for base rate */
    /* The "clockTick0" counts the number of times the code of this task has
     * been executed. The absolute time is the multiplication of "clockTick0"
     * and "Timing.stepSize0". Size of "clockTick0" ensures timer will not
     * overflow during the application lifespan selected.
     */
    ++Lander_Code_Model_2020a_M->Timing.clockTick0;
    Lander_Code_Model_2020a_M->Timing.t[0] = rtsiGetSolverStopTime
      (&Lander_Code_Model_2020a_M->solverInfo);

    {
      /* Update absolute timer for sample time: [0.1s, 0.0s] */
      /* The "clockTick1" counts the number of times the code of this task has
       * been executed. The resolution of this integer timer is 0.1, which is the step size
       * of the task. Size of "clockTick1" ensures timer will not overflow during the
       * application lifespan selected.
       */
      Lander_Code_Model_2020a_M->Timing.clockTick1++;
    }
  }                                    /* end MajorTimeStep */
//  Lander_Code_Model_2020a_M. // TODO some constant states
// Lander_Code_Model_2020a_M_. //  TODO similar to above
//Lander_Code_Model_2020a_P.uDOFEulerAngles_eul_0 // TODO? contains filter params and maybe euler angle?
//Lander_Code_Model_2020a_U.ReferenceEulerAngles // TODO contains reference euler angles
//Lander_Code_Model_2020a_X.phithetapsi_CSTATE // Current states of orientation and roll rate.
//Lander_Code_Model_2020a_B.FilterCoefficient // TODO filter coefficients
//Lander_Code_Model_2020a_DW.Filter_DSTATE // TODO intermediate states?
//Lander_Code_Model_2020a_Y.Gimble_Angles // TODO Gimble angles? In euler coords?
}

/* Derivatives for root system: '<Root>' */
void Lander_Code_Model_2020a_derivatives(void)
{
  XDot_Lander_Code_Model_2020a_T *_rtXdot;
  _rtXdot = ((XDot_Lander_Code_Model_2020a_T *)
             Lander_Code_Model_2020a_M->derivs);

  /* Derivatives for Integrator: '<S6>/phi theta psi' */
  _rtXdot->phithetapsi_CSTATE[0] =
    Lander_Code_Model_2020a_B.TmpSignalConversionAtphithetaps[0];

  /* Derivatives for Integrator: '<S1>/ub,vb,wb' */
  _rtXdot->ubvbwb_CSTATE[0] = Lander_Code_Model_2020a_B.Sum[0];

  /* Derivatives for Integrator: '<S1>/p,q,r ' */
  _rtXdot->pqr_CSTATE[0] = Lander_Code_Model_2020a_B.Product2[0];

  /* Derivatives for Integrator: '<S1>/xe,ye,ze' */
  _rtXdot->xeyeze_CSTATE[0] = Lander_Code_Model_2020a_B.Product[0];

  /* Derivatives for Integrator: '<S6>/phi theta psi' */
  _rtXdot->phithetapsi_CSTATE[1] =
    Lander_Code_Model_2020a_B.TmpSignalConversionAtphithetaps[1];

  /* Derivatives for Integrator: '<S1>/ub,vb,wb' */
  _rtXdot->ubvbwb_CSTATE[1] = Lander_Code_Model_2020a_B.Sum[1];

  /* Derivatives for Integrator: '<S1>/p,q,r ' */
  _rtXdot->pqr_CSTATE[1] = Lander_Code_Model_2020a_B.Product2[1];

  /* Derivatives for Integrator: '<S1>/xe,ye,ze' */
  _rtXdot->xeyeze_CSTATE[1] = Lander_Code_Model_2020a_B.Product[1];

  /* Derivatives for Integrator: '<S6>/phi theta psi' */
  _rtXdot->phithetapsi_CSTATE[2] =
    Lander_Code_Model_2020a_B.TmpSignalConversionAtphithetaps[2];

  /* Derivatives for Integrator: '<S1>/ub,vb,wb' */
  _rtXdot->ubvbwb_CSTATE[2] = Lander_Code_Model_2020a_B.Sum[2];

  /* Derivatives for Integrator: '<S1>/p,q,r ' */
  _rtXdot->pqr_CSTATE[2] = Lander_Code_Model_2020a_B.Product2[2];

  /* Derivatives for Integrator: '<S1>/xe,ye,ze' */
  _rtXdot->xeyeze_CSTATE[2] = Lander_Code_Model_2020a_B.Product[2];
}

/* Model initialize function */
void Lander_Code_Model_2020a_initialize(void)
{
  /* Registration code */
  {
    /* Setup solver object */
    rtsiSetSimTimeStepPtr(&Lander_Code_Model_2020a_M->solverInfo,
                          &Lander_Code_Model_2020a_M->Timing.simTimeStep);
    rtsiSetTPtr(&Lander_Code_Model_2020a_M->solverInfo, &rtmGetTPtr
                (Lander_Code_Model_2020a_M));
    rtsiSetStepSizePtr(&Lander_Code_Model_2020a_M->solverInfo,
                       &Lander_Code_Model_2020a_M->Timing.stepSize0);
    rtsiSetdXPtr(&Lander_Code_Model_2020a_M->solverInfo,
                 &Lander_Code_Model_2020a_M->derivs);
    rtsiSetContStatesPtr(&Lander_Code_Model_2020a_M->solverInfo, (real_T **)
                         &Lander_Code_Model_2020a_M->contStates);
    rtsiSetNumContStatesPtr(&Lander_Code_Model_2020a_M->solverInfo,
      &Lander_Code_Model_2020a_M->Sizes.numContStates);
    rtsiSetNumPeriodicContStatesPtr(&Lander_Code_Model_2020a_M->solverInfo,
      &Lander_Code_Model_2020a_M->Sizes.numPeriodicContStates);
    rtsiSetPeriodicContStateIndicesPtr(&Lander_Code_Model_2020a_M->solverInfo,
      &Lander_Code_Model_2020a_M->periodicContStateIndices);
    rtsiSetPeriodicContStateRangesPtr(&Lander_Code_Model_2020a_M->solverInfo,
      &Lander_Code_Model_2020a_M->periodicContStateRanges);
    rtsiSetErrorStatusPtr(&Lander_Code_Model_2020a_M->solverInfo,
                          (&rtmGetErrorStatus(Lander_Code_Model_2020a_M)));
    rtsiSetRTModelPtr(&Lander_Code_Model_2020a_M->solverInfo,
                      Lander_Code_Model_2020a_M);
  }



  rtsiSetSimTimeStep(&Lander_Code_Model_2020a_M->solverInfo, MAJOR_TIME_STEP);
  Lander_Code_Model_2020a_M->intgData.y = Lander_Code_Model_2020a_M->odeY;
  Lander_Code_Model_2020a_M->intgData.f[0] = Lander_Code_Model_2020a_M->odeF[0];
  Lander_Code_Model_2020a_M->intgData.f[1] = Lander_Code_Model_2020a_M->odeF[1];
  Lander_Code_Model_2020a_M->intgData.f[2] = Lander_Code_Model_2020a_M->odeF[2];
  Lander_Code_Model_2020a_M->contStates = ((X_Lander_Code_Model_2020a_T *)
    &Lander_Code_Model_2020a_X);
  Lander_Code_Model_2020a_M->periodicContStateIndices = ((int_T*)
    Lander_Code_Model__PeriodicIndX);
  Lander_Code_Model_2020a_M->periodicContStateRanges = ((real_T*)
    Lander_Code_Model__PeriodicRngX);
  rtsiSetSolverData(&Lander_Code_Model_2020a_M->solverInfo, (void *)
                    &Lander_Code_Model_2020a_M->intgData);
  rtsiSetSolverName(&Lander_Code_Model_2020a_M->solverInfo,"ode3");
  rtmSetTPtr(Lander_Code_Model_2020a_M,
             &Lander_Code_Model_2020a_M->Timing.tArray[0]);
  Lander_Code_Model_2020a_M->Timing.stepSize0 = 0.1;

  /* InitializeConditions for DiscreteIntegrator: '<S160>/Integrator' */
  Lander_Code_Model_2020a_DW.Integrator_DSTATE =
    Lander_Code_Model_2020a_P.PIDController3_InitialCondition;

  /* InitializeConditions for Delay: '<S153>/UD' */
  Lander_Code_Model_2020a_DW.UD_DSTATE =
    Lander_Code_Model_2020a_P.PIDController3_DifferentiatorIC;

  /* InitializeConditions for DiscreteIntegrator: '<S62>/Integrator' */
  Lander_Code_Model_2020a_DW.Integrator_DSTATE_m =
    Lander_Code_Model_2020a_P.PIDController1_InitialCondition;

  /* InitializeConditions for Delay: '<S55>/UD' */
  Lander_Code_Model_2020a_DW.UD_DSTATE_k =
    Lander_Code_Model_2020a_P.PIDController1_DifferentiatorIC;

  /* InitializeConditions for DiscreteIntegrator: '<S110>/Integrator' */
  Lander_Code_Model_2020a_DW.Integrator_DSTATE_g =
    Lander_Code_Model_2020a_P.PIDController2_InitialConditi_o;

  /* InitializeConditions for DiscreteIntegrator: '<S105>/Filter' */
  Lander_Code_Model_2020a_DW.Filter_DSTATE =
    Lander_Code_Model_2020a_P.PIDController2_InitialCondition;

  /* InitializeConditions for Integrator: '<S6>/phi theta psi' */
  Lander_Code_Model_2020a_X.phithetapsi_CSTATE[0] =
    Lander_Code_Model_2020a_P.uDOFEulerAngles_eul_0[0];

  /* InitializeConditions for Integrator: '<S1>/ub,vb,wb' */
  Lander_Code_Model_2020a_X.ubvbwb_CSTATE[0] =
    Lander_Code_Model_2020a_P.uDOFEulerAngles_Vm_0[0];

  /* InitializeConditions for Integrator: '<S1>/p,q,r ' */
  Lander_Code_Model_2020a_X.pqr_CSTATE[0] =
    Lander_Code_Model_2020a_P.uDOFEulerAngles_pm_0[0];

  /* InitializeConditions for Integrator: '<S1>/xe,ye,ze' */
  Lander_Code_Model_2020a_X.xeyeze_CSTATE[0] =
    Lander_Code_Model_2020a_P.uDOFEulerAngles_xme_0[0];

  /* InitializeConditions for Integrator: '<S6>/phi theta psi' */
  Lander_Code_Model_2020a_X.phithetapsi_CSTATE[1] =
    Lander_Code_Model_2020a_P.uDOFEulerAngles_eul_0[1];

  /* InitializeConditions for Integrator: '<S1>/ub,vb,wb' */
  Lander_Code_Model_2020a_X.ubvbwb_CSTATE[1] =
    Lander_Code_Model_2020a_P.uDOFEulerAngles_Vm_0[1];

  /* InitializeConditions for Integrator: '<S1>/p,q,r ' */
  Lander_Code_Model_2020a_X.pqr_CSTATE[1] =
    Lander_Code_Model_2020a_P.uDOFEulerAngles_pm_0[1];

  /* InitializeConditions for Integrator: '<S1>/xe,ye,ze' */
  Lander_Code_Model_2020a_X.xeyeze_CSTATE[1] =
    Lander_Code_Model_2020a_P.uDOFEulerAngles_xme_0[1];

  /* InitializeConditions for Integrator: '<S6>/phi theta psi' */
  Lander_Code_Model_2020a_X.phithetapsi_CSTATE[2] =
    Lander_Code_Model_2020a_P.uDOFEulerAngles_eul_0[2];

  /* InitializeConditions for Integrator: '<S1>/ub,vb,wb' */
  Lander_Code_Model_2020a_X.ubvbwb_CSTATE[2] =
    Lander_Code_Model_2020a_P.uDOFEulerAngles_Vm_0[2];

  /* InitializeConditions for Integrator: '<S1>/p,q,r ' */
  Lander_Code_Model_2020a_X.pqr_CSTATE[2] =
    Lander_Code_Model_2020a_P.uDOFEulerAngles_pm_0[2];

  /* InitializeConditions for Integrator: '<S1>/xe,ye,ze' */
  Lander_Code_Model_2020a_X.xeyeze_CSTATE[2] =
    Lander_Code_Model_2020a_P.uDOFEulerAngles_xme_0[2];

  /* InitializeConditions for root-level periodic continuous states */
  {
    int_T rootPeriodicContStateIndices[3] = { 0, 1, 2 };

    real_T rootPeriodicContStateRanges[6] = { -3.1415926535897931,
      3.1415926535897931, -3.1415926535897931, 3.1415926535897931,
      -3.1415926535897931, 3.1415926535897931 };

    (void) memcpy((void*)Lander_Code_Model__PeriodicIndX,
                  rootPeriodicContStateIndices,
                  3*sizeof(int_T));
    (void) memcpy((void*)Lander_Code_Model__PeriodicRngX,
                  rootPeriodicContStateRanges,
                  6*sizeof(real_T));
  }
}


/* Model terminate function */
void Lander_Code_Model_2020a_terminate(void)
{
  /* (no terminate code required) */
}
/*
 * File trailer for generated code.
 *
 * [EOF]
 */
