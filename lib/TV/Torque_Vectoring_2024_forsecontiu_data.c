/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: Torque_Vectoring_2024_forsecontiu_data.c
 *
 * Code generated for Simulink model 'Torque_Vectoring_2024_forsecontiu'.
 *
 * Model version                  : 3.0
 * Simulink Coder version         : 24.1 (R2024a) 19-Nov-2023
 * C/C++ source code generated on : Fri Aug 16 09:58:35 2024
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Texas Instruments->C2000
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. RAM efficiency
 * Validation result: Not run
 */

#include "Torque_Vectoring_2024_forsecontiu.h"

/* Invariant block signals (default storage) */
const ConstB rtConstB = {
  -0.0                                 /* '<S12>/Gain' */
};

/* Block parameters (default storage) */
P rtP = {
  /* Variable: Pmax
   * Referenced by: '<S22>/Total Power Limit'
   */
  45000.0,

  /* Variable: TC_map
   * Referenced by:
   *   '<S58>/Derivative Gain'
   *   '<S62>/Integral Gain'
   *   '<S70>/Proportional Gain'
   */
  { 0.0, 0.0, 0.0 },

  /* Variable: T_max
   * Referenced by:
   *   '<Root>/Constant1'
   *   '<S9>/Gain'
   */
  15.0,

  /* Variable: kRamp
   * Referenced by: '<S24>/Constant'
   */
  35.0,

  /* Variable: offset_brake_csi
   * Referenced by:
   *   '<Root>/Constant2'
   *   '<Root>/Gain12'
   */
  0.0,

  /* Variable: offset_cornering
   * Referenced by:
   *   '<S82>/Constant1'
   *   '<S82>/Multiply'
   */
  0.5,

  /* Variable: offset_throttle_csi
   * Referenced by:
   *   '<S9>/Constant'
   *   '<S9>/Gain1'
   *   '<S9>/Gain4'
   */
  0.3
};

/* Constant parameters (default storage) */
const ConstP rtConstP = {
  /* Expression: [1 0]
   * Referenced by: '<S14>/Constant4'
   */
  { 1.0, 0.0 },

  /* Expression: Q
   * Referenced by: '<S14>/Constant7'
   */
  { 0.1, 0.0, 0.0, 0.1 }
};

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
