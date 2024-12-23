/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: rigen_fun_simulink22.c
 *
 * Code generated for Simulink model 'rigen_fun_simulink22'.
 *
 * Model version                  : 1.1
 * Simulink Coder version         : 9.8 (R2022b) 13-May-2022
 * C/C++ source code generated on : Wed Nov 20 09:59:02 2024
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Texas Instruments->C2000
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. RAM efficiency
 * Validation result: Not run
 */

#include "rigen_fun_simulink22.h"
#include "rt_nonfinite.h"
#include <math.h>
#include "rtwtypes.h"
#include <string.h>

/* External outputs (root outports fed by signals with default storage) */
ExtY_rigen_fun_simulink22_T rigen_fun_simulink22_Y;
ExtU_rigen_fun_simulink22_T rigen_fun_simulink22_U;

/* Real-time model */
static RT_MODEL_rigen_fun_simulink22_T rigen_fun_simulink22_M_;
RT_MODEL_rigen_fun_simulink22_T *const rigen_fun_simulink22_M =
  &rigen_fun_simulink22_M_;

/* Model step function */
void rigen_fun_simulink22_step(void)
{
  real_T rtb_Gain1_a;
  real_T rtb_Gain_m;
  real_T rtb_Power_front;

  /* Product: '<Root>/Product1' incorporates:
   *  Constant: '<Root>/Constant'
   *  Constant: '<Root>/Constant1'
   *  Constant: '<Root>/Constant2'
   *  Constant: '<Root>/Constant6'
   *  Gain: '<Root>/Gain'
   *  Gain: '<Root>/rpm2rad//s'
   *  Sum: '<Root>/Sum'
   */
  rigen_fun_simulink22_Y.TorqueRR = (((rigen_fun_simulink22_U.rpmFL +
          rigen_fun_simulink22_U.rpmFR) + rigen_fun_simulink22_U.rpmRL) +
          rigen_fun_simulink22_U.rpmRR) * rigen_fun_simulink22_P.Gain_Gain *
    rigen_fun_simulink22_P.rpm2rads_Gain;

  /* Product: '<S2>/Divide' incorporates:
   *  Constant: '<Root>/tensione'
   *  Constant: '<S2>/Constant2'
   *  Constant: '<S2>/Rbatt'
   *  Constant: '<S2>/max_voltage'
   *  Sum: '<S2>/Sum'
   *  Sum: '<S2>/Sum1'
   */
  rtb_Power_front = (rigen_fun_simulink22_U.voltage -
                     rigen_fun_simulink22_P.max_voltage_Value) /
    (rigen_fun_simulink22_P.Rbatt_Value + rigen_fun_simulink22_P.Constant2_Value);

  /* MinMax: '<S2>/MinMax' incorporates:
   *  Constant: '<Root>/max_curr'
   */
  if ((rigen_fun_simulink22_P.max_curr <= rtb_Power_front) || rtIsNaN
      (rtb_Power_front)) {
    rtb_Power_front = rigen_fun_simulink22_P.max_curr;
  }

  /* Product: '<Root>/Product' incorporates:
   *  Constant: '<Root>/csi_opt'
   *  Constant: '<Root>/max_curr'
   *  Constant: '<Root>/tensione'
   *  Constant: '<S2>/Rbatt'
   *  Gain: '<Root>/Gain1'
   *  Gain: '<S2>/Gain'
   *  MinMax: '<S2>/MinMax'
   *  Product: '<S2>/Product'
   *  Product: '<S2>/Product1'
   *  Sum: '<S2>/Sum2'
   */
  rtb_Power_front = (rtb_Power_front * rigen_fun_simulink22_P.Rbatt_Value +
          rigen_fun_simulink22_U.voltage) *
    rigen_fun_simulink22_P.max_curr * rigen_fun_simulink22_P.Gain_Gain_h *
    (rigen_fun_simulink22_P.Gain1_Gain * rigen_fun_simulink22_P.csi_opt);

  /* Gain: '<Root>/rad//s2rad//s elettrici' */
  rtb_Gain1_a = rigen_fun_simulink22_P.rads2radselettrici_Gain *
    rigen_fun_simulink22_Y.TorqueRR;

  /* Gain: '<S4>/Gain' incorporates:
   *  Constant: '<S1>/flux_PM'
   *  Product: '<S4>/Product'
   */
  rtb_Gain_m = rigen_fun_simulink22_P.flux_PM_Value * rtb_Gain1_a *
    rigen_fun_simulink22_P.Gain_Gain_e;

  /* Gain: '<S3>/Gain1' incorporates:
   *  Constant: '<S1>/Ld'
   *  Constant: '<S1>/Lq'
   *  Constant: '<S1>/tan_alfa'
   *  Constant: '<S3>/Constant'
   *  Constant: '<S3>/R'
   *  Math: '<S3>/Square'
   *  Product: '<S3>/Divide'
   *  Product: '<S3>/Divide1'
   *  Product: '<S3>/Product'
   *  Product: '<S3>/Product1'
   *  Sum: '<S3>/Subtract'
   *  Sum: '<S3>/Sum'
   *  Sum: '<S3>/Sum1'
   */
  rtb_Gain1_a = ((rigen_fun_simulink22_P.Constant_Value /
                  (rigen_fun_simulink22_P.tan_alfa_Value *
                   rigen_fun_simulink22_P.tan_alfa_Value) +
                  rigen_fun_simulink22_P.Constant_Value) *
                 rigen_fun_simulink22_P.R_Value_m +
                 (rigen_fun_simulink22_P.Ld_Value -
                  rigen_fun_simulink22_P.Lq_Value) * rtb_Gain1_a /
                 rigen_fun_simulink22_P.tan_alfa_Value) *
    rigen_fun_simulink22_P.Gain1_Gain_g;

  /* Product: '<S1>/Divide' incorporates:
   *  Gain: '<S1>/Gain'
   *  Gain: '<S1>/Gain1'
   *  Gain: '<S1>/Gain2'
   *  Math: '<S1>/Square'
   *  Product: '<S1>/Product'
   *  Sqrt: '<S1>/Square Root'
   *  Sum: '<S1>/Subtract'
   *  Sum: '<S1>/Subtract1'
   */
  rtb_Gain1_a = (sqrt(rtb_Gain_m * rtb_Gain_m -
                      rigen_fun_simulink22_P.Gain_Gain_o * rtb_Power_front *
                      rtb_Gain1_a * rigen_fun_simulink22_P.Gain1_Gain_l) -
                 rtb_Gain_m) / (rigen_fun_simulink22_P.Gain2_Gain * rtb_Gain1_a);

  /* Math: '<S1>/Square2' */
  rtb_Gain_m = rtb_Gain1_a * rtb_Gain1_a;

  /* Product: '<S1>/Divide1' incorporates:
   *  Constant: '<S1>/tan_alfa'
   */
  rtb_Gain1_a /= rigen_fun_simulink22_P.tan_alfa_Value;

  /* Product: '<Root>/Divide2' incorporates:
   *  Constant: '<S1>/R'
   *  Gain: '<S1>/Gain3'
   *  Math: '<S1>/Square1'
   *  Product: '<S1>/Product1'
   *  Sum: '<Root>/Sum3'
   *  Sum: '<S1>/Sum'
   */
  rtb_Power_front = ((rtb_Gain1_a * rtb_Gain1_a + rtb_Gain_m) *
                     rigen_fun_simulink22_P.R_Value *
                     rigen_fun_simulink22_P.Gain3_Gain + rtb_Power_front) * (1.0
    / rigen_fun_simulink22_Y.TorqueRR);

  /* Saturate: '<Root>/Saturation' */
  if (rtb_Power_front > rigen_fun_simulink22_P.Tmax) {
    rtb_Power_front = rigen_fun_simulink22_P.Tmax;
  } else if (rtb_Power_front < rigen_fun_simulink22_P.Saturation_LowerSat) {
    rtb_Power_front = rigen_fun_simulink22_P.Saturation_LowerSat;
  }

  /* Product: '<Root>/Product1' incorporates:
   *  Gain: '<Root>/Gain3'
   *  Saturate: '<Root>/Saturation'
   */
  rigen_fun_simulink22_Y.TorqueRR = rigen_fun_simulink22_P.Gain3_Gain_f *
    rtb_Power_front;

  /* Outport: '<Root>/TorqueFL' */
  rigen_fun_simulink22_Y.TorqueFL = rigen_fun_simulink22_Y.TorqueRR;

  /* Outport: '<Root>/TorqueFR' */
  rigen_fun_simulink22_Y.TorqueFR = rigen_fun_simulink22_Y.TorqueRR;

  /* Product: '<Root>/Product1' incorporates:
   *  Constant: '<Root>/Constant7'
   *  Constant: '<Root>/csi_opt'
   *  Product: '<Root>/Divide4'
   *  Sum: '<Root>/Sum1'
   */
  rigen_fun_simulink22_Y.TorqueRR *= 1.0 / rigen_fun_simulink22_P.csi_opt *
    (rigen_fun_simulink22_P.Constant7_Value - rigen_fun_simulink22_P.csi_opt);

  /* Outport: '<Root>/TorqueRL' */
  rigen_fun_simulink22_Y.TorqueRL = rigen_fun_simulink22_Y.TorqueRR;
}

/* Model initialize function */
void rigen_fun_simulink22_initialize(void)
{
  /* Registration code */

  /* initialize non-finites */
  rt_InitInfAndNaN(sizeof(real_T));

  /* initialize error status */
  rtmSetErrorStatus(rigen_fun_simulink22_M, (NULL));

  /* external outputs */
  (void)memset(&rigen_fun_simulink22_Y, 0, sizeof(ExtY_rigen_fun_simulink22_T));
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
