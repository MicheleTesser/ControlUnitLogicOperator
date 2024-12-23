/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: rigen_fun_simulink22_data.c
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

/* Block parameters (default storage) */
P_rigen_fun_simulink22_T rigen_fun_simulink22_P = {
  /* Variable: Tmax
   * Referenced by: '<Root>/Saturation'
   */
  15.0,

  /* Variable: csi_opt
   * Referenced by: '<Root>/csi_opt'
   */
  0.7,

  /* Variable: max_curr
   * Referenced by: '<Root>/max_curr'
   */
  75.0,

  /* Variable: rpmFL
   * Referenced by: '<Root>/Constant6'
   */
  8000.0,

  /* Variable: rpmFR
   * Referenced by: '<Root>/Constant'
   */
  8000.0,

  /* Variable: rpmRL
   * Referenced by: '<Root>/Constant1'
   */
  8000.0,

  /* Variable: rpmRR
   * Referenced by: '<Root>/Constant2'
   */
  8000.0,

  /* Variable: tensione
   * Referenced by: '<Root>/tensione'
   */
  510.0,

  /* Expression: 0.25
   * Referenced by: '<Root>/Gain'
   */
  0.25,

  /* Expression: pi/30
   * Referenced by: '<Root>/rpm2rad//s'
   */
  0.10471975511965977,

  /* Expression: 0.231
   * Referenced by: '<S2>/Rbatt'
   */
  0.231,

  /* Expression: 564.94
   * Referenced by: '<S2>/max_voltage'
   */
  564.94,

  /* Expression: 0.1
   * Referenced by: '<S2>/Constant2'
   */
  0.1,

  /* Expression: 1/0.98
   * Referenced by: '<S2>/Gain'
   */
  1.0204081632653061,

  /* Expression: 0.5
   * Referenced by: '<Root>/Gain1'
   */
  0.5,

  /* Expression: 0.135/2
   * Referenced by: '<S1>/R'
   */
  0.0675,

  /* Expression: 0.0296
   * Referenced by: '<S1>/flux_PM'
   */
  0.0296,

  /* Expression: 5
   * Referenced by: '<Root>/rad//s2rad//s elettrici'
   */
  5.0,

  /* Expression: 1.5
   * Referenced by: '<S4>/Gain'
   */
  1.5,

  /* Expression: 0.12e-3
   * Referenced by: '<S1>/Ld'
   */
  0.00012,

  /* Expression: 0.24e-3
   * Referenced by: '<S1>/Lq'
   */
  0.00024,

  /* Expression: -5.919761121102884
   * Referenced by: '<S1>/tan_alfa'
   */
  -5.9197611211028844,

  /* Expression: 1
   * Referenced by: '<S3>/Constant'
   */
  1.0,

  /* Expression: 0.132/2
   * Referenced by: '<S3>/R'
   */
  0.066,

  /* Expression: 1.5
   * Referenced by: '<S3>/Gain1'
   */
  1.5,

  /* Expression: -1
   * Referenced by: '<S1>/Gain'
   */
  -1.0,

  /* Expression: 4
   * Referenced by: '<S1>/Gain1'
   */
  4.0,

  /* Expression: 2
   * Referenced by: '<S1>/Gain2'
   */
  2.0,

  /* Expression: 1.5
   * Referenced by: '<S1>/Gain3'
   */
  1.5,

  /* Expression: 0
   * Referenced by: '<Root>/Saturation'
   */
  0.0,

  /* Expression: -1
   * Referenced by: '<Root>/Gain3'
   */
  -1.0,

  /* Expression: 1
   * Referenced by: '<Root>/Constant7'
   */
  1.0
};

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
