/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: Torque_Vectoring_2024_forsecontiu.c
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
#include <math.h>
#include "rtwtypes.h"
#include "math.h"

/* Block signals and states (default storage) */
DW rtDW;

/* External inputs (root inport signals with default storage) */
ExtU rtU;

/* External outputs (root outports fed by signals with default storage) */
ExtY rtY;

/* Real-time model */
static RT_MODEL rtM_;
RT_MODEL *const rtM = &rtM_;
extern real_T rt_powd_snf(real_T u0, real_T u1);
static real_T rtGetInf(void);
static real32_T rtGetInfF(void);
static real_T rtGetMinusInf(void);
static real32_T rtGetMinusInfF(void);
static real_T rtGetNaN(void);
static real32_T rtGetNaNF(void);
extern real_T rtInf;
extern real_T rtMinusInf;
extern real_T rtNaN;
extern real32_T rtInfF;
extern real32_T rtMinusInfF;
extern real32_T rtNaNF;
static boolean_T rtIsInf(real_T value);
static boolean_T rtIsInfF(real32_T value);
static boolean_T rtIsNaN(real_T value);
static boolean_T rtIsNaNF(real32_T value);
real_T rtNaN = -(real_T)NAN;
real_T rtInf = (real_T)INFINITY;
real_T rtMinusInf = -(real_T)INFINITY;
real32_T rtNaNF = -(real32_T)NAN;
real32_T rtInfF = (real32_T)INFINITY;
real32_T rtMinusInfF = -(real32_T)INFINITY;

/* Return rtInf needed by the generated code. */
static real_T rtGetInf(void)
{
  return rtInf;
}

/* Get rtInfF needed by the generated code. */
static real32_T rtGetInfF(void)
{
  return rtInfF;
}

/* Return rtMinusInf needed by the generated code. */
static real_T rtGetMinusInf(void)
{
  return rtMinusInf;
}

/* Return rtMinusInfF needed by the generated code. */
static real32_T rtGetMinusInfF(void)
{
  return rtMinusInfF;
}

/* Return rtNaN needed by the generated code. */
static real_T rtGetNaN(void)
{
  return rtNaN;
}

/* Return rtNaNF needed by the generated code. */
static real32_T rtGetNaNF(void)
{
  return rtNaNF;
}

/* Test if value is infinite */
static boolean_T rtIsInf(real_T value)
{
  return (boolean_T)((value==rtInf || value==rtMinusInf) ? 1U : 0U);
}

/* Test if single-precision value is infinite */
static boolean_T rtIsInfF(real32_T value)
{
  return (boolean_T)(((value)==rtInfF || (value)==rtMinusInfF) ? 1U : 0U);
}

/* Test if value is not a number */
static boolean_T rtIsNaN(real_T value)
{
  return (boolean_T)(isnan(value) != 0);
}

/* Test if single-precision value is not a number */
static boolean_T rtIsNaNF(real32_T value)
{
  return (boolean_T)(isnan(value) != 0);
}

real_T rt_powd_snf(real_T u0, real_T u1)
{
  real_T y;
  if (rtIsNaN(u0) || rtIsNaN(u1)) {
    y = (rtNaN);
  } else {
    real_T tmp;
    real_T tmp_0;
    tmp = fabs(u0);
    tmp_0 = fabs(u1);
    if (rtIsInf(u1)) {
      if (tmp == 1.0) {
        y = 1.0;
      } else if (tmp > 1.0) {
        if (u1 > 0.0) {
          y = (rtInf);
        } else {
          y = 0.0;
        }
      } else if (u1 > 0.0) {
        y = 0.0;
      } else {
        y = (rtInf);
      }
    } else if (tmp_0 == 0.0) {
      y = 1.0;
    } else if (tmp_0 == 1.0) {
      if (u1 > 0.0) {
        y = u0;
      } else {
        y = 1.0 / u0;
      }
    } else if (u1 == 2.0) {
      y = u0 * u0;
    } else if ((u1 == 0.5) && (u0 >= 0.0)) {
      y = sqrt(u0);
    } else if ((u0 < 0.0) && (u1 > floor(u1))) {
      y = (rtNaN);
    } else {
      y = pow(u0, u1);
    }
  }

  return y;
}

/* Model step function */
void Torque_Vectoring_2024_forsecontiu_step(void)
{
  real_T rtb_A_k[4];
  real_T rtb_Saturation1_a[4];
  real_T rtb_K_k[2];
  real_T rtb_Add1_b;
  real_T rtb_Csi;
  real_T rtb_Divide1_e_idx_0;
  real_T rtb_Divide1_e_idx_1;
  real_T rtb_Divide1_e_idx_2;
  real_T rtb_Divide1_e_idx_3;
  real_T rtb_Divide_am;
  real_T rtb_Exp4_0;
  real_T rtb_Exp4_idx_0;
  real_T rtb_Exp4_idx_1;
  real_T rtb_Exp4_idx_2;
  real_T rtb_Iq_sqr;
  real_T rtb_Min_g_idx_1;
  real_T rtb_Product11_0;
  real_T rtb_Product12_0;
  real_T rtb_Product13_0;
  real_T rtb_Product1_p_idx_0;
  real_T rtb_Product1_p_idx_1;
  real_T rtb_Product1_p_idx_1_tmp;
  real_T rtb_Product1_p_idx_2;
  real_T rtb_Product2_e;
  real_T rtb_Product4_0;
  real_T rtb_Product5_o_idx_0;
  real_T rtb_Product7_0;
  real_T rtb_Product8_0;
  real_T rtb_Product_dt;
  real_T rtb_Product_idx_0;
  real_T rtb_Product_idx_1;
  real_T rtb_Product_idx_2;
  real_T rtb_Saturation;
  real_T rtb_Saturation1;
  real_T rtb_Saturation2;
  real_T rtb_Saturation_a_idx_0;
  real_T rtb_Saturation_a_idx_1;
  real_T rtb_Saturation_a_idx_2;
  real_T rtb_Saturation_g;
  real_T rtb_Saturation_k_0;
  real_T rtb_Saturation_k_idx_0;
  real_T rtb_Saturation_k_idx_1;
  real_T rtb_Saturation_k_idx_2;
  real_T rtb_Sum1_j;
  real_T rtb_SumofElements2;
  real_T rtb_vims_idx_0;
  real_T rtb_vims_idx_1;
  real_T rtb_vims_idx_2;
  real_T rtb_vims_idx_3;
  real_T tmp;
  real_T u;
  int16_T i;
  int16_T rtb_Saturation1_n_tmp;
  boolean_T rtb_Compare_j;
  boolean_T rtb_fw_inactive;

  /* SignalConversion generated from: '<S14>/Vector Concatenate' incorporates:
   *  Constant: '<S14>/Constant'
   */
  rtb_A_k[0] = 1.0;

  /* Gain: '<S14>/Gain' incorporates:
   *  Memory: '<S3>/Memory4'
   */
  rtb_A_k[1] = -0.01 * rtDW.Memory4_PreviousInput;

  /* Gain: '<S14>/Gain1' incorporates:
   *  Memory: '<S3>/Memory4'
   */
  rtb_A_k[2] = 0.01 * rtDW.Memory4_PreviousInput;

  /* SignalConversion generated from: '<S14>/Vector Concatenate1' incorporates:
   *  Constant: '<S14>/Constant'
   */
  rtb_A_k[3] = 1.0;

  /* Product: '<S14>/Product2' incorporates:
   *  Concatenate: '<S14>/Vector Concatenate2'
   *  Math: '<S14>/Transpose'
   *  Memory: '<S14>/Memory'
   */
  rtb_Saturation1 = rtb_A_k[2];
  rtb_Saturation = rtb_A_k[1];
  for (i = 0; i < 2; i++) {
    rtb_Sum1_j = rtDW.Memory_PreviousInput[i + 2];
    rtb_Saturation2 = rtDW.Memory_PreviousInput[i];
    rtb_Saturation1_a[i] = rtb_Sum1_j * rtb_Saturation1 + rtb_Saturation2;
    rtb_Saturation1_a[i + 2] = rtb_Saturation2 * rtb_Saturation + rtb_Sum1_j;
  }

  /* Product: '<S14>/Product4' */
  rtb_Product1_p_idx_0 = 0.0;

  /* Product: '<S14>/Product2' */
  rtb_Divide1_e_idx_3 = rtb_Saturation1_a[1];
  rtb_Add1_b = rtb_Saturation1_a[0];
  rtb_Saturation2 = rtb_Saturation1_a[3];
  rtb_vims_idx_1 = rtb_Saturation1_a[2];
  for (i = 0; i < 2; i++) {
    /* Sum: '<S14>/Add' incorporates:
     *  Concatenate: '<S14>/Vector Concatenate2'
     *  Constant: '<S14>/Constant7'
     *  Product: '<S14>/Product2'
     *  Saturate: '<S7>/Saturation1'
     */
    rtb_Saturation1 = rtb_A_k[i + 2];
    rtb_Saturation = rtb_A_k[i];
    rtb_Sum1_j = (rtb_Saturation1 * rtb_Divide1_e_idx_3 + rtb_Saturation *
                  rtb_Add1_b) + rtConstP.Constant7_Value_d[i];
    rtb_Saturation1_a[i] = rtb_Sum1_j;

    /* Product: '<S14>/Product4' incorporates:
     *  Product: '<S14>/Product3'
     */
    rtb_vims_idx_2 = rtb_Sum1_j;

    /* Sum: '<S14>/Add' incorporates:
     *  Constant: '<S14>/Constant7'
     *  Product: '<S14>/Product2'
     *  Saturate: '<S7>/Saturation1'
     */
    rtb_Sum1_j = (rtb_Saturation1 * rtb_Saturation2 + rtb_Saturation *
                  rtb_vims_idx_1) + rtConstP.Constant7_Value_d[i + 2];
    rtb_Saturation1_a[i + 2] = rtb_Sum1_j;

    /* Product: '<S14>/Product4' incorporates:
     *  Constant: '<S14>/Constant4'
     *  Math: '<S14>/Transpose2'
     *  Product: '<S14>/Product3'
     */
    rtb_vims_idx_2 += rtb_Sum1_j * 0.0;
    rtb_K_k[i] = rtb_vims_idx_2;
    rtb_Product1_p_idx_0 += rtConstP.Constant4_Value[i] * rtb_vims_idx_2;
  }

  /* Product: '<S14>/Divide' incorporates:
   *  Constant: '<S14>/Constant5'
   *  Product: '<S14>/Product3'
   *  Product: '<S14>/Product4'
   *  Sum: '<S14>/Add2'
   */
  rtb_K_k[0] /= rtb_Product1_p_idx_0 + 0.01;
  rtb_K_k[1] /= rtb_Product1_p_idx_0 + 0.01;

  /* Sum: '<S14>/Add1' incorporates:
   *  Constant: '<S14>/Constant4'
   *  Constant: '<S14>/Constant6'
   *  Product: '<S14>/Divide'
   *  Product: '<S14>/Product5'
   */
  tmp = 1.0 - rtb_K_k[0];
  rtb_Saturation_k_idx_1 = 0.0 - rtb_K_k[1];
  rtb_Saturation_k_idx_2 = 0.0 - rtb_K_k[0] * 0.0;
  rtb_Product1_p_idx_0 = rtb_K_k[1] * 0.0;
  for (i = 0; i < 2; i++) {
    /* Product: '<S14>/Product6' incorporates:
     *  Constant: '<S14>/Constant6'
     *  Memory: '<S14>/Memory'
     *  Saturate: '<S7>/Saturation1'
     *  Sum: '<S14>/Add1'
     */
    rtb_Saturation1_n_tmp = i << 1U;
    rtb_Sum1_j = rtb_Saturation1_a[rtb_Saturation1_n_tmp + 1];
    rtb_Saturation1 = rtb_Saturation1_a[rtb_Saturation1_n_tmp];
    rtDW.Memory_PreviousInput[rtb_Saturation1_n_tmp] = rtb_Sum1_j *
      rtb_Saturation_k_idx_2 + rtb_Saturation1 * tmp;
    rtDW.Memory_PreviousInput[rtb_Saturation1_n_tmp + 1] = (1.0 -
      rtb_Product1_p_idx_0) * rtb_Sum1_j + rtb_Saturation1 *
      rtb_Saturation_k_idx_1;
  }

  /* Saturate: '<Root>/Saturation1' incorporates:
   *  Inport: '<Root>/ay'
   */
  if (rtU.ay > 25.506000000000004) {
    rtb_Saturation1 = 25.506000000000004;
  } else if (rtU.ay < -25.506000000000004) {
    rtb_Saturation1 = -25.506000000000004;
  } else {
    rtb_Saturation1 = rtU.ay;
  }

  /* End of Saturate: '<Root>/Saturation1' */

  /* Gain: '<S11>/Gain3' */
  rtb_Add1_b = 28.092598525950262 * rtb_Saturation1;

  /* Saturate: '<Root>/Saturation' incorporates:
   *  Inport: '<Root>/ax'
   */
  if (rtU.ax > 17.658) {
    rtb_Saturation = 17.658;
  } else if (rtU.ax < -20.601000000000003) {
    rtb_Saturation = -20.601000000000003;
  } else {
    rtb_Saturation = rtU.ax;
  }

  /* End of Saturate: '<Root>/Saturation' */

  /* Gain: '<S11>/Gain2' */
  rtb_Sum1_j = 27.043973941368076 * rtb_Saturation;

  /* Gain: '<S11>/Fl' incorporates:
   *  Math: '<S11>/Square'
   *  Memory: '<Root>/Memory'
   */
  rtb_Divide_am = rtDW.Memory_PreviousInput_e * rtDW.Memory_PreviousInput_e *
    2.75625;

  /* Gain: '<S11>/Gain1' */
  rtb_Product2_e = 0.26535596091205216 * rtb_Divide_am;

  /* Sum: '<S11>/Add' incorporates:
   *  Constant: '<S11>/Constant'
   */
  rtb_Saturation_g = ((610.8564375 - rtb_Add1_b) - rtb_Sum1_j) + rtb_Product2_e;

  /* Sum: '<S11>/Add1' incorporates:
   *  Constant: '<S11>/Constant'
   */
  rtb_Add1_b = ((rtb_Add1_b + 610.8564375) - rtb_Sum1_j) + rtb_Product2_e;

  /* Gain: '<S11>/Gain4' */
  rtb_Product2_e = 40.392586510900976 * rtb_Saturation1;

  /* Gain: '<S11>/Gain' */
  rtb_Divide_am *= 0.23464403908794787;

  /* Sum: '<S11>/Add2' incorporates:
   *  Constant: '<S11>/Constant1'
   */
  rtb_Product_dt = ((746.6023125 - rtb_Product2_e) + rtb_Sum1_j) + rtb_Divide_am;

  /* Sum: '<S11>/Add3' incorporates:
   *  Constant: '<S11>/Constant1'
   */
  rtb_Product2_e = ((rtb_Product2_e + 746.6023125) + rtb_Sum1_j) + rtb_Divide_am;

  /* Saturate: '<Root>/Saturation6' incorporates:
   *  Inport: '<Root>/steering'
   */
  if (rtU.steering > 100.0) {
    rtb_Product1_p_idx_0 = 100.0;
  } else if (rtU.steering < -100.0) {
    rtb_Product1_p_idx_0 = -100.0;
  } else {
    rtb_Product1_p_idx_0 = rtU.steering;
  }

  /* Gain: '<S2>/Gain3' incorporates:
   *  Saturate: '<Root>/Saturation6'
   */
  rtb_Divide_am = 0.00030229102644541785 * rtb_Product1_p_idx_0;

  /* Math: '<S2>/Math Function1' incorporates:
   *  Constant: '<S2>/Constant1'
   *  Math: '<S2>/Math Function5'
   */
  rtb_Saturation2 = rt_powd_snf(rtb_Divide_am, 4.0);

  /* Math: '<S2>/Square2' incorporates:
   *  Math: '<S2>/Square1'
   */
  rtb_Sum1_j = rtb_Divide_am * rtb_Divide_am;

  /* Gain: '<S2>/Gain2' incorporates:
   *  Constant: '<S2>/Constant2'
   *  Gain: '<S2>/Gain9'
   *  Math: '<S2>/Math Function2'
   */
  rtb_vims_idx_1 = 63090.0 * rt_powd_snf(rtb_Divide_am, 3.0);

  /* Gain: '<S2>/Gain1' incorporates:
   *  Constant: '<S2>/Constant3'
   *  Gain: '<S2>/Gain10'
   *  Math: '<S2>/Math Function3'
   */
  rtb_vims_idx_2 = 2.497E+7 * rt_powd_snf(rtb_Divide_am, 5.0);

  /* Gain: '<S2>/Gain' incorporates:
   *  Constant: '<S2>/Constant7'
   *  Gain: '<S2>/Gain1'
   *  Gain: '<S2>/Gain11'
   *  Gain: '<S2>/Gain2'
   *  Gain: '<S2>/Gain4'
   *  Gain: '<S2>/Gain5'
   *  Gain: '<S2>/Gain6'
   *  Gain: '<S2>/Gain7'
   *  Gain: '<S2>/Gain8'
   *  Math: '<S2>/Math Function1'
   *  Math: '<S2>/Square2'
   *  Sum: '<S2>/Sum1'
   *  Sum: '<S2>/Sum2'
   */
  tmp = (((((-1.217E+6 * rtb_Saturation2 + rtb_vims_idx_2) + rtb_vims_idx_1) +
           rtb_Sum1_j * -3034.0) + 988.7 * rtb_Divide_am) - 0.0009525) *
    0.017453292519943295;
  rtb_Divide1_e_idx_0 = (((((988.7 * rtb_Divide_am + 0.0009525) + rtb_Sum1_j *
    3034.0) + rtb_vims_idx_1) + 1.217E+6 * rtb_Saturation2) + rtb_vims_idx_2) *
    0.017453292519943295;

  /* Saturate: '<Root>/Saturation5' incorporates:
   *  Inport: '<Root>/rpm'
   */
  if (rtU.rpm[0] > 20000.0) {
    rtb_Sum1_j = 20000.0;
  } else if (rtU.rpm[0] < 0.0) {
    rtb_Sum1_j = 0.0;
  } else {
    rtb_Sum1_j = rtU.rpm[0];
  }

  rtb_Saturation1_a[0] = rtb_Sum1_j;

  /* Sum: '<S5>/Add' incorporates:
   *  Constant: '<S5>/Constant'
   *  Gain: '<S5>/Gain2'
   *  SignalConversion generated from: '<S10>/Math Function'
   */
  rtb_Divide1_e_idx_3 = -3.533E-6 * rtb_Saturation_g + 0.2032;

  /* Saturate: '<S5>/Saturation' */
  if (rtb_Divide1_e_idx_3 > 0.2032) {
    rtb_Divide1_e_idx_3 = 0.2032;
  } else if (rtb_Divide1_e_idx_3 < 0.19304) {
    rtb_Divide1_e_idx_3 = 0.19304;
  }

  rtb_Saturation_a_idx_0 = rtb_Divide1_e_idx_3;

  /* Product: '<Root>/Product' incorporates:
   *  Gain: '<Root>/Gain'
   *  Gain: '<Root>/reduction'
   */
  rtb_Saturation_k_0 = 0.10471975511965977 * rtb_Sum1_j * 0.078947160665366667 *
    rtb_Divide1_e_idx_3;
  rtb_Saturation_k_idx_0 = rtb_Saturation_k_0;

  /* Trigonometry: '<S15>/Cos' incorporates:
   *  Constant: '<S12>/Constant2'
   *  Gain: '<S2>/Gain'
   *  Sum: '<S12>/Add'
   *  Trigonometry: '<S8>/Cos'
   */
  rtb_Divide1_e_idx_1 = cos(tmp - -0.017453292519943295);

  /* Product: '<S15>/Product1' incorporates:
   *  Trigonometry: '<S15>/Cos'
   */
  rtb_Product1_p_idx_0 = rtb_Divide1_e_idx_1 * rtb_Saturation_k_0;

  /* Saturate: '<Root>/Saturation5' incorporates:
   *  Inport: '<Root>/rpm'
   */
  if (rtU.rpm[1] > 20000.0) {
    rtb_Sum1_j = 20000.0;
  } else if (rtU.rpm[1] < 0.0) {
    rtb_Sum1_j = 0.0;
  } else {
    rtb_Sum1_j = rtU.rpm[1];
  }

  rtb_Saturation1_a[1] = rtb_Sum1_j;

  /* Sum: '<S5>/Add' incorporates:
   *  Constant: '<S5>/Constant'
   *  Gain: '<S5>/Gain2'
   *  SignalConversion generated from: '<S10>/Math Function'
   */
  rtb_Divide1_e_idx_3 = -3.533E-6 * rtb_Add1_b + 0.2032;

  /* Saturate: '<S5>/Saturation' */
  if (rtb_Divide1_e_idx_3 > 0.2032) {
    rtb_Divide1_e_idx_3 = 0.2032;
  } else if (rtb_Divide1_e_idx_3 < 0.19304) {
    rtb_Divide1_e_idx_3 = 0.19304;
  }

  rtb_Saturation_a_idx_1 = rtb_Divide1_e_idx_3;

  /* Product: '<Root>/Product' incorporates:
   *  Gain: '<Root>/Gain'
   *  Gain: '<Root>/reduction'
   */
  rtb_Saturation_k_0 = 0.10471975511965977 * rtb_Sum1_j * 0.078947160665366667 *
    rtb_Divide1_e_idx_3;
  rtb_Saturation_k_idx_1 = rtb_Saturation_k_0;

  /* Trigonometry: '<S15>/Cos' incorporates:
   *  Gain: '<S2>/Gain'
   *  Sum: '<S12>/Add1'
   *  Trigonometry: '<S8>/Cos'
   */
  rtb_Product1_p_idx_1_tmp = cos(rtb_Divide1_e_idx_0 - 0.017453292519943295);

  /* Product: '<S15>/Product1' incorporates:
   *  Trigonometry: '<S15>/Cos'
   */
  rtb_Product1_p_idx_1 = rtb_Product1_p_idx_1_tmp * rtb_Saturation_k_0;

  /* Saturate: '<Root>/Saturation5' incorporates:
   *  Inport: '<Root>/rpm'
   */
  if (rtU.rpm[2] > 20000.0) {
    rtb_Sum1_j = 20000.0;
  } else if (rtU.rpm[2] < 0.0) {
    rtb_Sum1_j = 0.0;
  } else {
    rtb_Sum1_j = rtU.rpm[2];
  }

  rtb_Saturation1_a[2] = rtb_Sum1_j;

  /* Sum: '<S5>/Add' incorporates:
   *  Constant: '<S5>/Constant'
   *  Gain: '<S5>/Gain2'
   *  SignalConversion generated from: '<S10>/Math Function'
   */
  rtb_Divide1_e_idx_3 = -3.533E-6 * rtb_Product_dt + 0.2032;

  /* Saturate: '<S5>/Saturation' */
  if (rtb_Divide1_e_idx_3 > 0.2032) {
    rtb_Divide1_e_idx_3 = 0.2032;
  } else if (rtb_Divide1_e_idx_3 < 0.19304) {
    rtb_Divide1_e_idx_3 = 0.19304;
  }

  rtb_Saturation_a_idx_2 = rtb_Divide1_e_idx_3;

  /* Product: '<Root>/Product' incorporates:
   *  Gain: '<Root>/Gain'
   *  Gain: '<Root>/reduction'
   */
  rtb_Saturation_k_0 = 0.10471975511965977 * rtb_Sum1_j * 0.078947160665366667 *
    rtb_Divide1_e_idx_3;
  rtb_Saturation_k_idx_2 = rtb_Saturation_k_0;

  /* Trigonometry: '<S15>/Cos' incorporates:
   *  SignalConversion generated from: '<S15>/Cos'
   *  Trigonometry: '<S8>/Cos'
   */
  rtb_Product5_o_idx_0 = cos(rtConstB.Gain);

  /* Product: '<S15>/Product1' incorporates:
   *  Trigonometry: '<S15>/Cos'
   */
  rtb_Product1_p_idx_2 = rtb_Product5_o_idx_0 * rtb_Saturation_k_0;

  /* Saturate: '<Root>/Saturation5' incorporates:
   *  Inport: '<Root>/rpm'
   */
  if (rtU.rpm[3] > 20000.0) {
    rtb_Sum1_j = 20000.0;
  } else if (rtU.rpm[3] < 0.0) {
    rtb_Sum1_j = 0.0;
  } else {
    rtb_Sum1_j = rtU.rpm[3];
  }

  /* Sum: '<S5>/Add' incorporates:
   *  Constant: '<S5>/Constant'
   *  Gain: '<S5>/Gain2'
   *  SignalConversion generated from: '<S10>/Math Function'
   */
  rtb_Divide1_e_idx_3 = -3.533E-6 * rtb_Product2_e + 0.2032;

  /* Saturate: '<S5>/Saturation' */
  if (rtb_Divide1_e_idx_3 > 0.2032) {
    rtb_Divide1_e_idx_3 = 0.2032;
  } else if (rtb_Divide1_e_idx_3 < 0.19304) {
    rtb_Divide1_e_idx_3 = 0.19304;
  }

  /* Product: '<Root>/Product' incorporates:
   *  Gain: '<Root>/Gain'
   *  Gain: '<Root>/reduction'
   */
  rtb_Saturation_k_0 = 0.10471975511965977 * rtb_Sum1_j * 0.078947160665366667 *
    rtb_Divide1_e_idx_3;

  /* Saturate: '<Root>/Saturation2' incorporates:
   *  Inport: '<Root>/yaw_r'
   */
  if (rtU.yaw_r > 6.2831853071795862) {
    rtb_Saturation2 = 6.2831853071795862;
  } else if (rtU.yaw_r < -6.2831853071795862) {
    rtb_Saturation2 = -6.2831853071795862;
  } else {
    rtb_Saturation2 = rtU.yaw_r;
  }

  /* End of Saturate: '<Root>/Saturation2' */

  /* Sum: '<S15>/Add' incorporates:
   *  Gain: '<S15>/Multiply'
   *  Gain: '<S15>/Multiply1'
   *  Gain: '<S15>/Multiply2'
   *  Gain: '<S15>/Multiply3'
   *  Product: '<S15>/Product1'
   */
  rtb_vims_idx_0 = 0.615 * rtb_Saturation2 + rtb_Product1_p_idx_0;
  rtb_vims_idx_1 = -0.615 * rtb_Saturation2 + rtb_Product1_p_idx_1;
  rtb_vims_idx_2 = 0.6 * rtb_Saturation2 + rtb_Product1_p_idx_2;
  rtb_vims_idx_3 = -0.6 * rtb_Saturation2 + rtb_Saturation_k_0;

  /* Sum: '<S21>/Add1' incorporates:
   *  Memory: '<S3>/Memory1'
   */
  u = rtDW.Memory1_PreviousInput[0] - rtb_vims_idx_0;

  /* Sum: '<S21>/Add2' incorporates:
   *  Gain: '<S21>/Gain1'
   *  Memory: '<S21>/Memory'
   *  Sum: '<S21>/Add'
   *  Sum: '<S21>/Add1'
   */
  rtb_Divide_am = rtb_Saturation - (rtb_vims_idx_0 -
    rtDW.Memory_PreviousInput_n[0]) * 100.0;

  /* Math: '<S21>/Exp4' incorporates:
   *  Gain: '<S21>/Gain'
   *  Math: '<S21>/Exp'
   *  Math: '<S21>/Exp1'
   *  Product: '<S21>/Divide'
   *  Product: '<S21>/Divide1'
   *  Sum: '<S21>/Add3'
   *
   * About '<S21>/Exp4':
   *  Operator: exp
   *
   * About '<S21>/Exp':
   *  Operator: magnitude^2
   *
   * About '<S21>/Exp1':
   *  Operator: magnitude^2
   */
  rtb_Exp4_0 = exp(((0.0 - u * u * 0.44444444444444442) - rtb_Divide_am *
                    rtb_Divide_am / 49.0) * 0.5);
  rtb_Exp4_idx_0 = rtb_Exp4_0;

  /* Product: '<S15>/Product' incorporates:
   *  Sum: '<S21>/Add1'
   */
  rtb_Product_idx_0 = rtb_vims_idx_0 * rtb_Exp4_0;

  /* Saturate: '<Root>/Saturation10' incorporates:
   *  Constant: '<S12>/Constant2'
   *  Gain: '<S2>/Gain'
   *  Sum: '<S12>/Add'
   */
  if (tmp - -0.017453292519943295 > 0.35) {
    rtb_Divide1_e_idx_2 = 0.35;
  } else if (tmp - -0.017453292519943295 < -0.35) {
    rtb_Divide1_e_idx_2 = -0.35;
  } else {
    rtb_Divide1_e_idx_2 = tmp - -0.017453292519943295;
  }

  /* Math: '<S10>/Square' incorporates:
   *  SignalConversion generated from: '<S10>/Math Function'
   */
  rtb_Csi = rtb_Saturation_g * rtb_Saturation_g;

  /* Math: '<S10>/Math Function' incorporates:
   *  Constant: '<S10>/Constant'
   *  SignalConversion generated from: '<S10>/Math Function'
   */
  rtb_Product1_p_idx_0 = rt_powd_snf(rtb_Saturation_g, 3.0);

  /* Gain: '<S10>/Gain2' */
  rtb_Product1_p_idx_2 = -1.8682E-5 * rtb_Product1_p_idx_0;

  /* Math: '<S10>/Math Function1' incorporates:
   *  Constant: '<S10>/Constant1'
   *  SignalConversion generated from: '<S10>/Math Function'
   */
  rtb_Product1_p_idx_1 = rt_powd_snf(rtb_Saturation_g, 4.0);

  /* Gain: '<S10>/Gain3' */
  rtb_Iq_sqr = 1.8267E-8 * rtb_Product1_p_idx_1;

  /* Math: '<S10>/Math Function3' incorporates:
   *  Constant: '<S10>/Constant2'
   *  SignalConversion generated from: '<S10>/Math Function'
   */
  rtb_Divide_am = rt_powd_snf(rtb_Saturation_g, 5.0);

  /* Gain: '<S10>/Gain4' */
  rtb_Exp4_0 = -8.8652E-12 * rtb_Divide_am;

  /* Product: '<S10>/Product13' incorporates:
   *  Saturate: '<S10>/Saturation'
   */
  rtb_Product13_0 = rtb_Divide1_e_idx_2 * rtb_Product1_p_idx_0;

  /* Product: '<S10>/Product12' incorporates:
   *  Saturate: '<S10>/Saturation'
   */
  rtb_Product12_0 = rtb_Divide1_e_idx_2 * rtb_Product1_p_idx_1;

  /* Product: '<S10>/Product11' incorporates:
   *  Saturate: '<S10>/Saturation'
   */
  rtb_Product11_0 = rtb_Divide1_e_idx_2 * rtb_Divide_am;

  /* Math: '<S10>/Square1' incorporates:
   *  Saturate: '<S10>/Saturation'
   */
  u = rtb_Divide1_e_idx_2 * rtb_Divide1_e_idx_2;

  /* Product: '<S10>/Product8' */
  rtb_Product8_0 = u * rtb_Product1_p_idx_0;

  /* Product: '<S10>/Product7' */
  rtb_Product7_0 = u * rtb_Product1_p_idx_1;

  /* Math: '<S10>/Math Function4' incorporates:
   *  Constant: '<S10>/Constant4'
   *  Saturate: '<S10>/Saturation'
   */
  rtb_Divide_am = rt_powd_snf(rtb_Divide1_e_idx_2, 3.0);

  /* Product: '<S10>/Product4' */
  rtb_Product4_0 = rtb_Divide_am * rtb_Product1_p_idx_0;

  /* Math: '<S10>/Math Function5' incorporates:
   *  Constant: '<S10>/Constant5'
   *  Saturate: '<S10>/Saturation'
   */
  rtb_Product1_p_idx_1 = rt_powd_snf(rtb_Divide1_e_idx_2, 4.0);

  /* Math: '<S10>/Math Function7' incorporates:
   *  Constant: '<S10>/Constant6'
   *  Saturate: '<S10>/Saturation'
   */
  rtb_Product1_p_idx_0 = rt_powd_snf(rtb_Divide1_e_idx_2, 5.0);

  /* Sum: '<S10>/Add' incorporates:
   *  Constant: '<S10>/Constant3'
   *  Constant: '<S10>/Constant7'
   *  Constant: '<S10>/Constant8'
   *  Gain: '<S10>/Gain'
   *  Gain: '<S10>/Gain1'
   *  Gain: '<S10>/Gain10'
   *  Gain: '<S10>/Gain12'
   *  Gain: '<S10>/Gain13'
   *  Gain: '<S10>/Gain14'
   *  Gain: '<S10>/Gain15'
   *  Gain: '<S10>/Gain16'
   *  Gain: '<S10>/Gain17'
   *  Gain: '<S10>/Gain18'
   *  Gain: '<S10>/Gain19'
   *  Gain: '<S10>/Gain20'
   *  Gain: '<S10>/Gain21'
   *  Gain: '<S10>/Gain22'
   *  Gain: '<S10>/Gain23'
   *  Gain: '<S10>/Gain24'
   *  Gain: '<S10>/Gain25'
   *  Gain: '<S10>/Gain26'
   *  Gain: '<S10>/Gain27'
   *  Gain: '<S10>/Gain5'
   *  Gain: '<S10>/Gain6'
   *  Gain: '<S10>/Gain7'
   *  Gain: '<S10>/Gain8'
   *  Gain: '<S10>/Gain9'
   *  Math: '<S10>/Math Function2'
   *  Math: '<S10>/Math Function6'
   *  Product: '<S10>/Product'
   *  Product: '<S10>/Product10'
   *  Product: '<S10>/Product14'
   *  Product: '<S10>/Product15'
   *  Product: '<S10>/Product2'
   *  Product: '<S10>/Product3'
   *  Product: '<S10>/Product5'
   *  Product: '<S10>/Product6'
   *  Product: '<S10>/Product9'
   *  Saturate: '<S10>/Saturation'
   *  SignalConversion generated from: '<S10>/Math Function'
   */
  rtb_SumofElements2 = ((((((((((((((((((((((((((0.2483 * rtb_Saturation_g +
    0.0088 * rtb_Csi) + rtb_Product1_p_idx_2) + rtb_Iq_sqr) + rtb_Exp4_0) +
    1.7349E-15 * rt_powd_snf(rtb_Saturation_g, 6.0)) + rtb_Divide1_e_idx_2 *
    rtb_Saturation_g * -0.00098788) + rtb_Divide1_e_idx_2 * rtb_Csi * 5.2321E-5)
    + -3.0722E-8 * rtb_Product13_0) + -2.1326E-11 * rtb_Product12_0) + 1.417E-14
    * rtb_Product11_0) + u * rtb_Saturation_g * -34.6685) + u * rtb_Csi *
    2.9871E-5) + 1.7183E-5 * rtb_Product8_0) + -6.5431E-9 * rtb_Product7_0) +
    rtb_Divide_am * rtb_Saturation_g * -0.202) + rtb_Divide_am * rtb_Csi *
    0.00013761) + 1.2868E-9 * rtb_Product4_0) + rtb_Product1_p_idx_1 *
    rtb_Saturation_g * 202.6574) + rtb_Product1_p_idx_1 * rtb_Csi * -0.0863) +
    rtb_Product1_p_idx_0 * rtb_Saturation_g * -0.1108) + 1.146 *
    rtb_Divide1_e_idx_2) + -12185.0 * u) + -10.876 * rtb_Divide_am) + 260290.0 *
    rtb_Product1_p_idx_1) + 346.9631 * rtb_Product1_p_idx_0) + -1.4028E+6 *
                        rt_powd_snf(rtb_Divide1_e_idx_2, 6.0)) + 300.2078;

  /* Sum: '<S21>/Add1' incorporates:
   *  Memory: '<S3>/Memory1'
   */
  u = rtDW.Memory1_PreviousInput[0] - rtb_vims_idx_1;

  /* Sum: '<S21>/Add2' incorporates:
   *  Gain: '<S21>/Gain1'
   *  Memory: '<S21>/Memory'
   *  Sum: '<S21>/Add'
   *  Sum: '<S21>/Add1'
   */
  rtb_Divide_am = rtb_Saturation - (rtb_vims_idx_1 -
    rtDW.Memory_PreviousInput_n[1]) * 100.0;

  /* Math: '<S21>/Exp4' incorporates:
   *  Gain: '<S21>/Gain'
   *  Math: '<S21>/Exp'
   *  Math: '<S21>/Exp1'
   *  Product: '<S21>/Divide'
   *  Product: '<S21>/Divide1'
   *  Sum: '<S21>/Add3'
   *
   * About '<S21>/Exp4':
   *  Operator: exp
   *
   * About '<S21>/Exp':
   *  Operator: magnitude^2
   *
   * About '<S21>/Exp1':
   *  Operator: magnitude^2
   */
  rtb_Exp4_0 = exp(((0.0 - u * u * 0.44444444444444442) - rtb_Divide_am *
                    rtb_Divide_am / 49.0) * 0.5);
  rtb_Exp4_idx_1 = rtb_Exp4_0;

  /* Product: '<S15>/Product' incorporates:
   *  Sum: '<S21>/Add1'
   */
  rtb_Product_idx_1 = rtb_vims_idx_1 * rtb_Exp4_0;

  /* Saturate: '<Root>/Saturation10' incorporates:
   *  Gain: '<S2>/Gain'
   *  Sum: '<S12>/Add1'
   */
  if (rtb_Divide1_e_idx_0 - 0.017453292519943295 > 0.35) {
    rtb_Divide1_e_idx_2 = 0.35;
  } else if (rtb_Divide1_e_idx_0 - 0.017453292519943295 < -0.35) {
    rtb_Divide1_e_idx_2 = -0.35;
  } else {
    rtb_Divide1_e_idx_2 = rtb_Divide1_e_idx_0 - 0.017453292519943295;
  }

  /* Math: '<S10>/Square' incorporates:
   *  SignalConversion generated from: '<S10>/Math Function'
   */
  rtb_Csi = rtb_Add1_b * rtb_Add1_b;

  /* Math: '<S10>/Math Function' incorporates:
   *  Constant: '<S10>/Constant'
   *  SignalConversion generated from: '<S10>/Math Function'
   */
  rtb_Product1_p_idx_0 = rt_powd_snf(rtb_Add1_b, 3.0);

  /* Gain: '<S10>/Gain2' */
  rtb_Product1_p_idx_2 = -1.8682E-5 * rtb_Product1_p_idx_0;

  /* Math: '<S10>/Math Function1' incorporates:
   *  Constant: '<S10>/Constant1'
   *  SignalConversion generated from: '<S10>/Math Function'
   */
  rtb_Product1_p_idx_1 = rt_powd_snf(rtb_Add1_b, 4.0);

  /* Gain: '<S10>/Gain3' */
  rtb_Iq_sqr = 1.8267E-8 * rtb_Product1_p_idx_1;

  /* Math: '<S10>/Math Function3' incorporates:
   *  Constant: '<S10>/Constant2'
   *  SignalConversion generated from: '<S10>/Math Function'
   */
  rtb_Divide_am = rt_powd_snf(rtb_Add1_b, 5.0);

  /* Gain: '<S10>/Gain4' */
  rtb_Exp4_0 = -8.8652E-12 * rtb_Divide_am;

  /* Product: '<S10>/Product13' incorporates:
   *  Saturate: '<S10>/Saturation'
   */
  rtb_Product13_0 = rtb_Divide1_e_idx_2 * rtb_Product1_p_idx_0;

  /* Product: '<S10>/Product12' incorporates:
   *  Saturate: '<S10>/Saturation'
   */
  rtb_Product12_0 = rtb_Divide1_e_idx_2 * rtb_Product1_p_idx_1;

  /* Product: '<S10>/Product11' incorporates:
   *  Saturate: '<S10>/Saturation'
   */
  rtb_Product11_0 = rtb_Divide1_e_idx_2 * rtb_Divide_am;

  /* Math: '<S10>/Square1' incorporates:
   *  Saturate: '<S10>/Saturation'
   */
  u = rtb_Divide1_e_idx_2 * rtb_Divide1_e_idx_2;

  /* Product: '<S10>/Product8' */
  rtb_Product8_0 = u * rtb_Product1_p_idx_0;

  /* Product: '<S10>/Product7' */
  rtb_Product7_0 = u * rtb_Product1_p_idx_1;

  /* Math: '<S10>/Math Function4' incorporates:
   *  Constant: '<S10>/Constant4'
   *  Saturate: '<S10>/Saturation'
   */
  rtb_Divide_am = rt_powd_snf(rtb_Divide1_e_idx_2, 3.0);

  /* Product: '<S10>/Product4' */
  rtb_Product4_0 = rtb_Divide_am * rtb_Product1_p_idx_0;

  /* Math: '<S10>/Math Function5' incorporates:
   *  Constant: '<S10>/Constant5'
   *  Saturate: '<S10>/Saturation'
   */
  rtb_Product1_p_idx_1 = rt_powd_snf(rtb_Divide1_e_idx_2, 4.0);

  /* Math: '<S10>/Math Function7' incorporates:
   *  Constant: '<S10>/Constant6'
   *  Saturate: '<S10>/Saturation'
   */
  rtb_Product1_p_idx_0 = rt_powd_snf(rtb_Divide1_e_idx_2, 5.0);

  /* Sum: '<S10>/Add' incorporates:
   *  Constant: '<S10>/Constant3'
   *  Constant: '<S10>/Constant7'
   *  Constant: '<S10>/Constant8'
   *  Gain: '<S10>/Gain'
   *  Gain: '<S10>/Gain1'
   *  Gain: '<S10>/Gain10'
   *  Gain: '<S10>/Gain12'
   *  Gain: '<S10>/Gain13'
   *  Gain: '<S10>/Gain14'
   *  Gain: '<S10>/Gain15'
   *  Gain: '<S10>/Gain16'
   *  Gain: '<S10>/Gain17'
   *  Gain: '<S10>/Gain18'
   *  Gain: '<S10>/Gain19'
   *  Gain: '<S10>/Gain20'
   *  Gain: '<S10>/Gain21'
   *  Gain: '<S10>/Gain22'
   *  Gain: '<S10>/Gain23'
   *  Gain: '<S10>/Gain24'
   *  Gain: '<S10>/Gain25'
   *  Gain: '<S10>/Gain26'
   *  Gain: '<S10>/Gain27'
   *  Gain: '<S10>/Gain5'
   *  Gain: '<S10>/Gain6'
   *  Gain: '<S10>/Gain7'
   *  Gain: '<S10>/Gain8'
   *  Gain: '<S10>/Gain9'
   *  Math: '<S10>/Math Function2'
   *  Math: '<S10>/Math Function6'
   *  Product: '<S10>/Product'
   *  Product: '<S10>/Product10'
   *  Product: '<S10>/Product14'
   *  Product: '<S10>/Product15'
   *  Product: '<S10>/Product2'
   *  Product: '<S10>/Product3'
   *  Product: '<S10>/Product5'
   *  Product: '<S10>/Product6'
   *  Product: '<S10>/Product9'
   *  Saturate: '<S10>/Saturation'
   *  SignalConversion generated from: '<S10>/Math Function'
   */
  rtb_Min_g_idx_1 = ((((((((((((((((((((((((((0.2483 * rtb_Add1_b + 0.0088 *
    rtb_Csi) + rtb_Product1_p_idx_2) + rtb_Iq_sqr) + rtb_Exp4_0) + 1.7349E-15 *
    rt_powd_snf(rtb_Add1_b, 6.0)) + rtb_Divide1_e_idx_2 * rtb_Add1_b *
    -0.00098788) + rtb_Divide1_e_idx_2 * rtb_Csi * 5.2321E-5) + -3.0722E-8 *
    rtb_Product13_0) + -2.1326E-11 * rtb_Product12_0) + 1.417E-14 *
    rtb_Product11_0) + u * rtb_Add1_b * -34.6685) + u * rtb_Csi * 2.9871E-5) +
    1.7183E-5 * rtb_Product8_0) + -6.5431E-9 * rtb_Product7_0) + rtb_Divide_am *
    rtb_Add1_b * -0.202) + rtb_Divide_am * rtb_Csi * 0.00013761) + 1.2868E-9 *
    rtb_Product4_0) + rtb_Product1_p_idx_1 * rtb_Add1_b * 202.6574) +
    rtb_Product1_p_idx_1 * rtb_Csi * -0.0863) + rtb_Product1_p_idx_0 *
    rtb_Add1_b * -0.1108) + 1.146 * rtb_Divide1_e_idx_2) + -12185.0 * u) +
                        -10.876 * rtb_Divide_am) + 260290.0 *
                       rtb_Product1_p_idx_1) + 346.9631 * rtb_Product1_p_idx_0)
                     + -1.4028E+6 * rt_powd_snf(rtb_Divide1_e_idx_2, 6.0)) +
    300.2078;

  /* Sum: '<S21>/Add1' incorporates:
   *  Memory: '<S3>/Memory1'
   */
  u = rtDW.Memory1_PreviousInput[0] - rtb_vims_idx_2;

  /* Sum: '<S21>/Add2' incorporates:
   *  Gain: '<S21>/Gain1'
   *  Memory: '<S21>/Memory'
   *  Sum: '<S21>/Add'
   *  Sum: '<S21>/Add1'
   */
  rtb_Divide_am = rtb_Saturation - (rtb_vims_idx_2 -
    rtDW.Memory_PreviousInput_n[2]) * 100.0;

  /* Math: '<S21>/Exp4' incorporates:
   *  Gain: '<S21>/Gain'
   *  Math: '<S21>/Exp'
   *  Math: '<S21>/Exp1'
   *  Product: '<S21>/Divide'
   *  Product: '<S21>/Divide1'
   *  Sum: '<S21>/Add3'
   *
   * About '<S21>/Exp4':
   *  Operator: exp
   *
   * About '<S21>/Exp':
   *  Operator: magnitude^2
   *
   * About '<S21>/Exp1':
   *  Operator: magnitude^2
   */
  rtb_Exp4_0 = exp(((0.0 - u * u * 0.44444444444444442) - rtb_Divide_am *
                    rtb_Divide_am / 49.0) * 0.5);
  rtb_Exp4_idx_2 = rtb_Exp4_0;

  /* Product: '<S15>/Product' incorporates:
   *  Sum: '<S21>/Add1'
   */
  rtb_Product_idx_2 = rtb_vims_idx_2 * rtb_Exp4_0;

  /* Saturate: '<Root>/Saturation10' incorporates:
   *  SignalConversion generated from: '<S15>/Cos'
   */
  if (rtConstB.Gain > 0.35) {
    rtb_Divide1_e_idx_2 = 0.35;
  } else if (rtConstB.Gain < -0.35) {
    rtb_Divide1_e_idx_2 = -0.35;
  } else {
    rtb_Divide1_e_idx_2 = rtConstB.Gain;
  }

  /* Math: '<S10>/Square' incorporates:
   *  SignalConversion generated from: '<S10>/Math Function'
   */
  rtb_Csi = rtb_Product_dt * rtb_Product_dt;

  /* Math: '<S10>/Math Function' incorporates:
   *  Constant: '<S10>/Constant'
   *  SignalConversion generated from: '<S10>/Math Function'
   */
  rtb_Product1_p_idx_0 = rt_powd_snf(rtb_Product_dt, 3.0);

  /* Gain: '<S10>/Gain2' */
  rtb_Product1_p_idx_2 = -1.8682E-5 * rtb_Product1_p_idx_0;

  /* Math: '<S10>/Math Function1' incorporates:
   *  Constant: '<S10>/Constant1'
   *  SignalConversion generated from: '<S10>/Math Function'
   */
  rtb_Product1_p_idx_1 = rt_powd_snf(rtb_Product_dt, 4.0);

  /* Gain: '<S10>/Gain3' */
  rtb_Iq_sqr = 1.8267E-8 * rtb_Product1_p_idx_1;

  /* Math: '<S10>/Math Function3' incorporates:
   *  Constant: '<S10>/Constant2'
   *  SignalConversion generated from: '<S10>/Math Function'
   */
  rtb_Divide_am = rt_powd_snf(rtb_Product_dt, 5.0);

  /* Gain: '<S10>/Gain4' */
  rtb_Exp4_0 = -8.8652E-12 * rtb_Divide_am;

  /* Product: '<S10>/Product13' incorporates:
   *  Saturate: '<S10>/Saturation'
   */
  rtb_Product13_0 = rtb_Divide1_e_idx_2 * rtb_Product1_p_idx_0;

  /* Product: '<S10>/Product12' incorporates:
   *  Saturate: '<S10>/Saturation'
   */
  rtb_Product12_0 = rtb_Divide1_e_idx_2 * rtb_Product1_p_idx_1;

  /* Product: '<S10>/Product11' incorporates:
   *  Saturate: '<S10>/Saturation'
   */
  rtb_Product11_0 = rtb_Divide1_e_idx_2 * rtb_Divide_am;

  /* Math: '<S10>/Square1' incorporates:
   *  Saturate: '<S10>/Saturation'
   */
  u = rtb_Divide1_e_idx_2 * rtb_Divide1_e_idx_2;

  /* Product: '<S10>/Product8' */
  rtb_Product8_0 = u * rtb_Product1_p_idx_0;

  /* Product: '<S10>/Product7' */
  rtb_Product7_0 = u * rtb_Product1_p_idx_1;

  /* Math: '<S10>/Math Function4' incorporates:
   *  Constant: '<S10>/Constant4'
   *  Saturate: '<S10>/Saturation'
   */
  rtb_Divide_am = rt_powd_snf(rtb_Divide1_e_idx_2, 3.0);

  /* Product: '<S10>/Product4' */
  rtb_Product4_0 = rtb_Divide_am * rtb_Product1_p_idx_0;

  /* Math: '<S10>/Math Function5' incorporates:
   *  Constant: '<S10>/Constant5'
   *  Saturate: '<S10>/Saturation'
   */
  rtb_Product1_p_idx_1 = rt_powd_snf(rtb_Divide1_e_idx_2, 4.0);

  /* Math: '<S10>/Math Function7' incorporates:
   *  Constant: '<S10>/Constant6'
   *  Saturate: '<S10>/Saturation'
   */
  rtb_Product1_p_idx_0 = rt_powd_snf(rtb_Divide1_e_idx_2, 5.0);

  /* Sum: '<S10>/Add' incorporates:
   *  Constant: '<S10>/Constant3'
   *  Constant: '<S10>/Constant7'
   *  Constant: '<S10>/Constant8'
   *  Gain: '<S10>/Gain'
   *  Gain: '<S10>/Gain1'
   *  Gain: '<S10>/Gain10'
   *  Gain: '<S10>/Gain12'
   *  Gain: '<S10>/Gain13'
   *  Gain: '<S10>/Gain14'
   *  Gain: '<S10>/Gain15'
   *  Gain: '<S10>/Gain16'
   *  Gain: '<S10>/Gain17'
   *  Gain: '<S10>/Gain18'
   *  Gain: '<S10>/Gain19'
   *  Gain: '<S10>/Gain20'
   *  Gain: '<S10>/Gain21'
   *  Gain: '<S10>/Gain22'
   *  Gain: '<S10>/Gain23'
   *  Gain: '<S10>/Gain24'
   *  Gain: '<S10>/Gain25'
   *  Gain: '<S10>/Gain26'
   *  Gain: '<S10>/Gain27'
   *  Gain: '<S10>/Gain5'
   *  Gain: '<S10>/Gain6'
   *  Gain: '<S10>/Gain7'
   *  Gain: '<S10>/Gain8'
   *  Gain: '<S10>/Gain9'
   *  Math: '<S10>/Math Function2'
   *  Math: '<S10>/Math Function6'
   *  Product: '<S10>/Product'
   *  Product: '<S10>/Product10'
   *  Product: '<S10>/Product14'
   *  Product: '<S10>/Product15'
   *  Product: '<S10>/Product2'
   *  Product: '<S10>/Product3'
   *  Product: '<S10>/Product5'
   *  Product: '<S10>/Product6'
   *  Product: '<S10>/Product9'
   *  Saturate: '<S10>/Saturation'
   *  SignalConversion generated from: '<S10>/Math Function'
   */
  rtb_Divide1_e_idx_2 = ((((((((((((((((((((((((((0.2483 * rtb_Product_dt +
    0.0088 * rtb_Csi) + rtb_Product1_p_idx_2) + rtb_Iq_sqr) + rtb_Exp4_0) +
    1.7349E-15 * rt_powd_snf(rtb_Product_dt, 6.0)) + rtb_Divide1_e_idx_2 *
    rtb_Product_dt * -0.00098788) + rtb_Divide1_e_idx_2 * rtb_Csi * 5.2321E-5) +
    -3.0722E-8 * rtb_Product13_0) + -2.1326E-11 * rtb_Product12_0) + 1.417E-14 *
    rtb_Product11_0) + u * rtb_Product_dt * -34.6685) + u * rtb_Csi * 2.9871E-5)
    + 1.7183E-5 * rtb_Product8_0) + -6.5431E-9 * rtb_Product7_0) + rtb_Divide_am
    * rtb_Product_dt * -0.202) + rtb_Divide_am * rtb_Csi * 0.00013761) +
    1.2868E-9 * rtb_Product4_0) + rtb_Product1_p_idx_1 * rtb_Product_dt *
    202.6574) + rtb_Product1_p_idx_1 * rtb_Csi * -0.0863) + rtb_Product1_p_idx_0
    * rtb_Product_dt * -0.1108) + 1.146 * rtb_Divide1_e_idx_2) + -12185.0 * u) +
    -10.876 * rtb_Divide_am) + 260290.0 * rtb_Product1_p_idx_1) + 346.9631 *
    rtb_Product1_p_idx_0) + -1.4028E+6 * rt_powd_snf(rtb_Divide1_e_idx_2, 6.0))
    + 300.2078;

  /* Sum: '<S21>/Add1' incorporates:
   *  Memory: '<S3>/Memory1'
   */
  u = rtDW.Memory1_PreviousInput[0] - rtb_vims_idx_3;

  /* Sum: '<S21>/Add2' incorporates:
   *  Gain: '<S21>/Gain1'
   *  Memory: '<S21>/Memory'
   *  Sum: '<S21>/Add'
   *  Sum: '<S21>/Add1'
   */
  rtb_Divide_am = rtb_Saturation - (rtb_vims_idx_3 -
    rtDW.Memory_PreviousInput_n[3]) * 100.0;

  /* Math: '<S21>/Exp4' incorporates:
   *  Gain: '<S21>/Gain'
   *  Math: '<S21>/Exp'
   *  Math: '<S21>/Exp1'
   *  Product: '<S21>/Divide'
   *  Product: '<S21>/Divide1'
   *  Sum: '<S21>/Add3'
   *
   * About '<S21>/Exp4':
   *  Operator: exp
   *
   * About '<S21>/Exp':
   *  Operator: magnitude^2
   *
   * About '<S21>/Exp1':
   *  Operator: magnitude^2
   */
  rtb_Exp4_0 = exp(((0.0 - u * u * 0.44444444444444442) - rtb_Divide_am *
                    rtb_Divide_am / 49.0) * 0.5);

  /* Math: '<S10>/Square' incorporates:
   *  SignalConversion generated from: '<S10>/Math Function'
   */
  rtb_Csi = rtb_Product2_e * rtb_Product2_e;

  /* Math: '<S10>/Math Function' incorporates:
   *  Constant: '<S10>/Constant'
   *  SignalConversion generated from: '<S10>/Math Function'
   */
  rtb_Product1_p_idx_0 = rt_powd_snf(rtb_Product2_e, 3.0);

  /* Math: '<S10>/Math Function1' incorporates:
   *  Constant: '<S10>/Constant1'
   *  SignalConversion generated from: '<S10>/Math Function'
   */
  rtb_Product1_p_idx_1 = rt_powd_snf(rtb_Product2_e, 4.0);

  /* Math: '<S10>/Math Function3' incorporates:
   *  Constant: '<S10>/Constant2'
   *  SignalConversion generated from: '<S10>/Math Function'
   */
  rtb_Divide_am = rt_powd_snf(rtb_Product2_e, 5.0);

  /* Sum: '<S10>/Add' incorporates:
   *  Constant: '<S10>/Constant3'
   *  Constant: '<S10>/Constant7'
   *  Gain: '<S10>/Gain'
   *  Gain: '<S10>/Gain1'
   *  Gain: '<S10>/Gain12'
   *  Gain: '<S10>/Gain14'
   *  Gain: '<S10>/Gain15'
   *  Gain: '<S10>/Gain16'
   *  Gain: '<S10>/Gain17'
   *  Gain: '<S10>/Gain18'
   *  Gain: '<S10>/Gain19'
   *  Gain: '<S10>/Gain2'
   *  Gain: '<S10>/Gain20'
   *  Gain: '<S10>/Gain21'
   *  Gain: '<S10>/Gain22'
   *  Gain: '<S10>/Gain23'
   *  Gain: '<S10>/Gain24'
   *  Gain: '<S10>/Gain25'
   *  Gain: '<S10>/Gain26'
   *  Gain: '<S10>/Gain27'
   *  Gain: '<S10>/Gain3'
   *  Gain: '<S10>/Gain4'
   *  Gain: '<S10>/Gain5'
   *  Math: '<S10>/Math Function2'
   *  Product: '<S10>/Product'
   *  Product: '<S10>/Product10'
   *  Product: '<S10>/Product11'
   *  Product: '<S10>/Product12'
   *  Product: '<S10>/Product13'
   *  Product: '<S10>/Product14'
   *  Product: '<S10>/Product15'
   *  Product: '<S10>/Product2'
   *  Product: '<S10>/Product3'
   *  Product: '<S10>/Product4'
   *  Product: '<S10>/Product5'
   *  Product: '<S10>/Product6'
   *  Product: '<S10>/Product7'
   *  Product: '<S10>/Product8'
   *  Product: '<S10>/Product9'
   *  SignalConversion generated from: '<S10>/Math Function'
   */
  u = ((((((((((((((((((((0.2483 * rtb_Product2_e + 0.0088 * rtb_Csi) +
    -1.8682E-5 * rtb_Product1_p_idx_0) + 1.8267E-8 * rtb_Product1_p_idx_1) +
                       -8.8652E-12 * rtb_Divide_am) + 1.7349E-15 * rt_powd_snf
                      (rtb_Product2_e, 6.0)) + 0.0 * rtb_Product2_e *
                     -0.00098788) + 0.0 * rtb_Csi * 5.2321E-5) + 0.0 *
                   rtb_Product1_p_idx_0 * -3.0722E-8) + 0.0 *
                  rtb_Product1_p_idx_1 * -2.1326E-11) + 0.0 * rtb_Divide_am *
                 1.417E-14) + 0.0 * rtb_Product2_e * -34.6685) + 0.0 * rtb_Csi *
               2.9871E-5) + 0.0 * rtb_Product1_p_idx_0 * 1.7183E-5) + 0.0 *
             rtb_Product1_p_idx_1 * -6.5431E-9) + 0.0 * rtb_Product2_e * -0.202)
           + 0.0 * rtb_Csi * 0.00013761) + 0.0 * rtb_Product1_p_idx_0 *
          1.2868E-9) + 0.0 * rtb_Product2_e * 202.6574) + 0.0 * rtb_Csi *
        -0.0863) + 0.0 * rtb_Product2_e * -0.1108) + 300.2078;

  /* Saturate: '<Root>/Saturation3' incorporates:
   *  Inport: '<Root>/throttle'
   */
  if (rtU.throttle > 1.0) {
    rtb_Product1_p_idx_0 = 1.0;
  } else if (rtU.throttle < 0.0) {
    rtb_Product1_p_idx_0 = 0.0;
  } else {
    rtb_Product1_p_idx_0 = rtU.throttle;
  }

  /* Gain: '<S9>/Gain' incorporates:
   *  Saturate: '<Root>/Saturation3'
   */
  rtb_Iq_sqr = rtP.T_max * rtb_Product1_p_idx_0;

  /* Sum: '<S1>/Add' incorporates:
   *  Sum: '<S82>/Sum'
   *  Sum: '<S82>/Sum1'
   */
  rtb_Add1_b += rtb_Saturation_g;

  /* Product: '<S1>/Product' incorporates:
   *  Constant: '<S1>/Constant'
   *  Constant: '<S1>/Constant1'
   *  Gain: '<S1>/Gain'
   *  Sum: '<S1>/Add'
   *  Sum: '<S1>/Add2'
   *  Sum: '<S1>/Subtract'
   */
  rtb_Csi = ((rtb_Add1_b - 667.461609) * -0.00049402457842335622 + 2.106129056) *
    rtb_Add1_b;

  /* Sum: '<S1>/Add1' */
  rtb_Divide_am = rtb_Product_dt + rtb_Product2_e;

  /* Product: '<S1>/Divide' incorporates:
   *  Constant: '<S1>/Constant'
   *  Constant: '<S1>/Constant1'
   *  Gain: '<S1>/Gain1'
   *  Product: '<S1>/Product1'
   *  Sum: '<S1>/Add3'
   *  Sum: '<S1>/Add4'
   *  Sum: '<S1>/Subtract1'
   */
  rtb_Csi /= ((rtb_Divide_am - 667.461609) * -0.00049402457842335622 +
              2.106129056) * rtb_Divide_am + rtb_Csi;

  /* Product: '<S9>/Divide' incorporates:
   *  Constant: '<S9>/Constant'
   *  Constant: '<S9>/Constant1'
   *  Gain: '<S9>/Gain1'
   *  Gain: '<S9>/Gain4'
   *  Product: '<S9>/Divide1'
   *  Sum: '<S9>/Add'
   *  Sum: '<S9>/Add1'
   */
  rtb_Divide_am = 1.0 / ((1.0 - rtP.offset_throttle_csi) * (1.0 - rtb_Csi) +
    rtP.offset_throttle_csi) * ((1.0 - rtP.offset_throttle_csi) * rtb_Csi *
    rtb_Iq_sqr);

  /* Product: '<S82>/Divide' */
  rtb_Saturation_g /= rtb_Add1_b;

  /* Product: '<S82>/Divide1' incorporates:
   *  Sum: '<S82>/Add2'
   *  Sum: '<S82>/Sum'
   */
  rtb_Product2_e *= 1.0 / (((rtb_Add1_b + rtb_Product_dt) + rtb_Product2_e) -
    rtb_Add1_b);

  /* Sum: '<S82>/Add3' incorporates:
   *  Constant: '<S82>/Constant'
   *  Constant: '<S82>/Constant1'
   *  Gain: '<S82>/Multiply'
   *  Sum: '<S82>/Add'
   *  Sum: '<S82>/Add1'
   */
  rtb_Product1_p_idx_0 = (1.0 - rtP.offset_cornering) * rtb_Saturation_g +
    rtP.offset_cornering;
  rtb_Product1_p_idx_1 = (1.0 - rtP.offset_cornering) * (1.0 - rtb_Saturation_g)
    + rtP.offset_cornering;
  rtb_Product1_p_idx_2 = (1.0 - rtP.offset_cornering) * (1.0 - rtb_Product2_e) +
    rtP.offset_cornering;
  rtb_Product2_e = (1.0 - rtP.offset_cornering) * rtb_Product2_e +
    rtP.offset_cornering;

  /* MinMax: '<S83>/Max' */
  rtb_Add1_b = fmax(rtb_Product1_p_idx_0, rtb_Product1_p_idx_1);

  /* MinMax: '<S83>/Max1' */
  rtb_Product_dt = fmax(rtb_Product1_p_idx_2, rtb_Product2_e);

  /* Product: '<S9>/Product' incorporates:
   *  Product: '<S83>/Divide'
   *  Product: '<S83>/Divide1'
   *  Product: '<S83>/Divide2'
   *  Product: '<S83>/Divide3'
   */
  rtb_Product1_p_idx_0 = rtb_Product1_p_idx_0 / rtb_Add1_b * rtb_Divide_am;
  rtb_Product1_p_idx_1 = rtb_Product1_p_idx_1 / rtb_Add1_b * rtb_Divide_am;
  rtb_Product1_p_idx_2 = rtb_Product1_p_idx_2 / rtb_Product_dt * rtb_Iq_sqr;
  rtb_Product2_e = rtb_Product2_e / rtb_Product_dt * rtb_Iq_sqr;

  /* Sum: '<S17>/Sum' incorporates:
   *  Concatenate: '<S14>/Vector Concatenate2'
   *  Gain: '<S14>/Gain3'
   *  Memory: '<S3>/Memory1'
   *  Memory: '<S3>/Memory2'
   *  Product: '<S17>/Product'
   *  Product: '<S17>/Product1'
   */
  rtb_Product_dt = (rtDW.Memory1_PreviousInput[1] * rtb_A_k[2] +
                    rtDW.Memory1_PreviousInput[0]) + (0.01 *
    rtDW.Memory2_PreviousInput[0] + 0.0 * rtDW.Memory2_PreviousInput[1]);
  rtb_Divide_am = (rtDW.Memory1_PreviousInput[0] * rtb_A_k[1] +
                   rtDW.Memory1_PreviousInput[1]) + (0.0 *
    rtDW.Memory2_PreviousInput[0] + 0.01 * rtDW.Memory2_PreviousInput[1]);

  /* Sum: '<S16>/Add' incorporates:
   *  Abs: '<S15>/Abs'
   *  Constant: '<S14>/Constant4'
   *  Constant: '<S15>/Constant'
   *  Constant: '<S18>/Constant'
   *  Constant: '<S19>/Constant'
   *  Constant: '<S20>/Constant'
   *  Math: '<S21>/Exp4'
   *  MinMax: '<S15>/Max'
   *  MinMax: '<S15>/Max1'
   *  MinMax: '<S15>/Min'
   *  Product: '<S15>/Divide'
   *  Product: '<S15>/Product'
   *  Product: '<S15>/Product of Elements'
   *  Product: '<S16>/Product'
   *  RelationalOperator: '<S18>/Compare'
   *  RelationalOperator: '<S19>/Compare'
   *  RelationalOperator: '<S20>/Compare'
   *  Sum: '<S15>/Add'
   *  Sum: '<S15>/Sum of Elements'
   *  Sum: '<S15>/Sum of Elements1'
   *  Sum: '<S15>/Sum of Elements2'
   *  Sum: '<S21>/Add1'
   *
   * About '<S21>/Exp4':
   *  Operator: exp
   */
  rtb_Add1_b = (((((rtb_Product_idx_0 + rtb_Product_idx_1) + rtb_Product_idx_2)
                  + rtb_vims_idx_3 * rtb_Exp4_0) / fmax(((rtb_Exp4_idx_0 +
    rtb_Exp4_idx_1) + rtb_Exp4_idx_2) + rtb_Exp4_0, 1.0E-7) * (real_T)(fabs
    (rtb_Saturation) <= 1.8) + (real_T)(rtb_Saturation >= 1.8) * fmin(fmin(fmin
    (rtb_vims_idx_0, rtb_vims_idx_1), rtb_vims_idx_2), rtb_vims_idx_3)) +
                (real_T)(rtb_Saturation < -1.8) * fmax(fmax(fmax(rtb_vims_idx_0,
    rtb_vims_idx_1), rtb_vims_idx_2), rtb_vims_idx_3)) - (0.0 * rtb_Divide_am +
    rtb_Product_dt);

  /* Sum: '<S16>/Add1' incorporates:
   *  Product: '<S14>/Divide'
   *  Product: '<S16>/Product1'
   */
  rtb_K_k[0] = rtb_K_k[0] * rtb_Add1_b + rtb_Product_dt;
  rtb_K_k[1] = rtb_K_k[1] * rtb_Add1_b + rtb_Divide_am;

  /* Gain: '<S8>/Multiply1' */
  rtb_Add1_b = 0.615 * rtb_Saturation2;

  /* Gain: '<S8>/Multiply3' */
  rtb_Iq_sqr = 0.6 * rtb_Saturation2;

  /* Product: '<S3>/Product' incorporates:
   *  Abs: '<S3>/Abs'
   *  Constant: '<S13>/Constant'
   *  Constant: '<S3>/Constant'
   *  RelationalOperator: '<S13>/Compare'
   *  Sum: '<S3>/Add'
   */
  rtb_Exp4_idx_0 = (1.0 - (real_T)(fabs(rtb_Saturation2) <= 0.034906585039886591))
    * rtb_K_k[1];

  /* Sum: '<S8>/Add1' incorporates:
   *  Gain: '<S8>/Multiply7'
   */
  rtb_Saturation_g = -0.69075 * rtb_Saturation2 - rtb_Exp4_idx_0;

  /* Product: '<S8>/Product4' incorporates:
   *  Sum: '<S8>/Add3'
   *  Sum: '<S8>/Add6'
   */
  rtb_A_k[0] = (rtb_K_k[0] - rtb_Add1_b) * rtb_Divide1_e_idx_1;
  rtb_A_k[1] = (rtb_Add1_b + rtb_K_k[0]) * rtb_Product1_p_idx_1_tmp;

  /* Sum: '<S8>/Add' incorporates:
   *  Gain: '<S8>/Multiply4'
   */
  rtb_Add1_b = 0.84425000000000006 * rtb_Saturation2 + rtb_Exp4_idx_0;

  /* Product: '<S8>/Product5' incorporates:
   *  Constant: '<S12>/Constant2'
   *  Gain: '<S30>/Gain'
   *  Product: '<Root>/Product'
   *  Product: '<S8>/Product4'
   *  SignalConversion generated from: '<S15>/Cos'
   *  Sum: '<S12>/Add'
   *  Sum: '<S12>/Add1'
   *  Sum: '<S30>/Sum'
   *  Sum: '<S8>/Add2'
   *  Sum: '<S8>/Add4'
   *  Sum: '<S8>/Add5'
   *  Trigonometry: '<S8>/Sin'
   */
  tmp = (sin(tmp - -0.017453292519943295) * rtb_Add1_b + rtb_A_k[0]) * 1.12 -
    rtb_Saturation_k_idx_0;
  rtb_Saturation_k_idx_1 = (sin(rtb_Divide1_e_idx_0 - 0.017453292519943295) *
    rtb_Add1_b + rtb_A_k[1]) * 1.12 - rtb_Saturation_k_idx_1;
  rtb_Saturation_k_idx_2 = ((rtb_K_k[0] - rtb_Iq_sqr) * rtb_Product5_o_idx_0 +
    sin(rtConstB.Gain) * rtb_Saturation_g) * 1.12 - rtb_Saturation_k_idx_2;
  rtb_Add1_b = ((rtb_Iq_sqr + rtb_K_k[0]) + 0.0 * rtb_Saturation_g) * 1.12 -
    rtb_Saturation_k_0;

  /* Saturate: '<S30>/Saturation' */
  if (tmp <= 0.0) {
    tmp = 0.0;
  }

  rtb_Saturation_k_idx_0 = tmp;

  /* Gain: '<S68>/Filter Coefficient' incorporates:
   *  DiscreteIntegrator: '<S60>/Filter'
   *  Gain: '<S58>/Derivative Gain'
   *  Sum: '<S60>/SumD'
   */
  rtb_Saturation_k_0 = (rtP.TC_map[2] * tmp - rtDW.Filter_DSTATE[0]) * 100.0;
  rtb_Product5_o_idx_0 = rtb_Saturation_k_0;

  /* Saturate: '<S72>/Saturation' incorporates:
   *  DiscreteIntegrator: '<S65>/Integrator'
   *  Gain: '<S70>/Proportional Gain'
   *  Saturate: '<S10>/Saturation1'
   *  Sum: '<S74>/Sum'
   */
  rtb_Divide_am = (rtP.TC_map[0] * tmp + rtDW.Integrator_DSTATE[0]) +
    rtb_Saturation_k_0;

  /* Saturate: '<S10>/Saturation1' incorporates:
   *  Sum: '<S10>/Add'
   */
  if (rtb_SumofElements2 > 2500.0) {
    rtb_SumofElements2 = 2500.0;
  } else if (rtb_SumofElements2 < 50.0) {
    rtb_SumofElements2 = 50.0;
  }

  /* Saturate: '<S72>/Saturation' */
  if (rtb_Divide_am > 1.0) {
    rtb_Divide_am = 1.0;
  } else if (rtb_Divide_am < 0.0) {
    rtb_Divide_am = 0.0;
  }

  /* MinMax: '<Root>/Min' incorporates:
   *  Constant: '<Root>/Constant1'
   *  Constant: '<S30>/Constant'
   *  Gain: '<S10>/Gain11'
   *  Product: '<Root>/Product1'
   *  Product: '<S10>/Product1'
   *  Saturate: '<S10>/Saturation1'
   *  Saturate: '<S72>/Saturation'
   *  Sum: '<S30>/Sum1'
   */
  rtb_SumofElements2 = fmin(fmin(rtP.T_max, rtb_SumofElements2 *
    rtb_Saturation_a_idx_0 * 0.078947160665366667), (1.0 - rtb_Divide_am) *
    rtb_Product1_p_idx_0);
  rtb_Product_dt = rtb_SumofElements2;

  /* Gain: '<Root>/Gain1' */
  rtb_Divide_am = 0.10471975511965977 * rtb_Saturation1_a[0];
  rtb_Product1_p_idx_0 = rtb_Divide_am;

  /* Product: '<S4>/Product2' */
  rtb_Divide1_e_idx_0 = rtb_SumofElements2 * rtb_Divide_am;

  /* Saturate: '<S30>/Saturation' */
  if (rtb_Saturation_k_idx_1 <= 0.0) {
    tmp = 0.0;
  } else {
    tmp = rtb_Saturation_k_idx_1;
  }

  rtb_Saturation_k_idx_1 = tmp;

  /* Gain: '<S68>/Filter Coefficient' incorporates:
   *  DiscreteIntegrator: '<S60>/Filter'
   *  Gain: '<S58>/Derivative Gain'
   *  Sum: '<S60>/SumD'
   */
  rtb_Saturation_k_0 = (rtP.TC_map[2] * tmp - rtDW.Filter_DSTATE[1]) * 100.0;
  rtb_Saturation_a_idx_0 = rtb_Saturation_k_0;

  /* Saturate: '<S72>/Saturation' incorporates:
   *  DiscreteIntegrator: '<S65>/Integrator'
   *  Gain: '<S70>/Proportional Gain'
   *  Saturate: '<S10>/Saturation1'
   *  Sum: '<S74>/Sum'
   */
  rtb_Divide_am = (rtP.TC_map[0] * tmp + rtDW.Integrator_DSTATE[1]) +
    rtb_Saturation_k_0;

  /* Saturate: '<S10>/Saturation1' incorporates:
   *  Sum: '<S10>/Add'
   */
  if (rtb_Min_g_idx_1 > 2500.0) {
    rtb_Min_g_idx_1 = 2500.0;
  } else if (rtb_Min_g_idx_1 < 50.0) {
    rtb_Min_g_idx_1 = 50.0;
  }

  /* Saturate: '<S72>/Saturation' */
  if (rtb_Divide_am > 1.0) {
    rtb_Divide_am = 1.0;
  } else if (rtb_Divide_am < 0.0) {
    rtb_Divide_am = 0.0;
  }

  /* MinMax: '<Root>/Min' incorporates:
   *  Constant: '<Root>/Constant1'
   *  Constant: '<S30>/Constant'
   *  Gain: '<S10>/Gain11'
   *  Product: '<Root>/Product1'
   *  Product: '<S10>/Product1'
   *  Saturate: '<S10>/Saturation1'
   *  Saturate: '<S72>/Saturation'
   *  Sum: '<S30>/Sum1'
   */
  rtb_SumofElements2 = fmin(fmin(rtP.T_max, rtb_Min_g_idx_1 *
    rtb_Saturation_a_idx_1 * 0.078947160665366667), (1.0 - rtb_Divide_am) *
    rtb_Product1_p_idx_1);
  rtb_Min_g_idx_1 = rtb_SumofElements2;

  /* Gain: '<Root>/Gain1' */
  rtb_Divide_am = 0.10471975511965977 * rtb_Saturation1_a[1];
  rtb_Product1_p_idx_1 = rtb_Divide_am;

  /* Product: '<S4>/Product2' */
  rtb_Divide1_e_idx_1 = rtb_SumofElements2 * rtb_Divide_am;

  /* Saturate: '<S30>/Saturation' */
  if (rtb_Saturation_k_idx_2 <= 0.0) {
    tmp = 0.0;
  } else {
    tmp = rtb_Saturation_k_idx_2;
  }

  rtb_Saturation_k_idx_2 = tmp;

  /* Gain: '<S68>/Filter Coefficient' incorporates:
   *  DiscreteIntegrator: '<S60>/Filter'
   *  Gain: '<S58>/Derivative Gain'
   *  Sum: '<S60>/SumD'
   */
  rtb_Saturation_k_0 = (rtP.TC_map[2] * tmp - rtDW.Filter_DSTATE[2]) * 100.0;
  rtb_Saturation_a_idx_1 = rtb_Saturation_k_0;

  /* Saturate: '<S72>/Saturation' incorporates:
   *  DiscreteIntegrator: '<S65>/Integrator'
   *  Gain: '<S70>/Proportional Gain'
   *  Saturate: '<S10>/Saturation1'
   *  Sum: '<S74>/Sum'
   */
  rtb_Divide_am = (rtP.TC_map[0] * tmp + rtDW.Integrator_DSTATE[2]) +
    rtb_Saturation_k_0;

  /* Saturate: '<S10>/Saturation1' incorporates:
   *  Sum: '<S10>/Add'
   */
  if (rtb_Divide1_e_idx_2 > 2500.0) {
    rtb_Divide1_e_idx_2 = 2500.0;
  } else if (rtb_Divide1_e_idx_2 < 50.0) {
    rtb_Divide1_e_idx_2 = 50.0;
  }

  /* Saturate: '<S72>/Saturation' */
  if (rtb_Divide_am > 1.0) {
    rtb_Divide_am = 1.0;
  } else if (rtb_Divide_am < 0.0) {
    rtb_Divide_am = 0.0;
  }

  /* MinMax: '<Root>/Min' incorporates:
   *  Constant: '<Root>/Constant1'
   *  Constant: '<S30>/Constant'
   *  Gain: '<S10>/Gain11'
   *  Product: '<Root>/Product1'
   *  Product: '<S10>/Product1'
   *  Saturate: '<S10>/Saturation1'
   *  Saturate: '<S72>/Saturation'
   *  Sum: '<S30>/Sum1'
   */
  rtb_SumofElements2 = fmin(fmin(rtP.T_max, rtb_Divide1_e_idx_2 *
    rtb_Saturation_a_idx_2 * 0.078947160665366667), (1.0 - rtb_Divide_am) *
    rtb_Product1_p_idx_2);
  rtb_Saturation_a_idx_2 = rtb_SumofElements2;

  /* Gain: '<Root>/Gain1' */
  rtb_Divide_am = 0.10471975511965977 * rtb_Saturation1_a[2];
  rtb_Product1_p_idx_2 = rtb_Divide_am;

  /* Product: '<S4>/Product2' */
  rtb_Divide1_e_idx_2 = rtb_SumofElements2 * rtb_Divide_am;

  /* Saturate: '<S30>/Saturation' */
  if (rtb_Add1_b <= 0.0) {
    tmp = 0.0;
  } else {
    tmp = rtb_Add1_b;
  }

  /* Gain: '<S68>/Filter Coefficient' incorporates:
   *  DiscreteIntegrator: '<S60>/Filter'
   *  Gain: '<S58>/Derivative Gain'
   *  Sum: '<S60>/SumD'
   */
  rtb_Saturation_k_0 = (rtP.TC_map[2] * tmp - rtDW.Filter_DSTATE[3]) * 100.0;

  /* Saturate: '<S72>/Saturation' incorporates:
   *  DiscreteIntegrator: '<S65>/Integrator'
   *  Gain: '<S70>/Proportional Gain'
   *  Saturate: '<S10>/Saturation1'
   *  Sum: '<S74>/Sum'
   */
  rtb_Divide_am = (rtP.TC_map[0] * tmp + rtDW.Integrator_DSTATE[3]) +
    rtb_Saturation_k_0;

  /* Saturate: '<S10>/Saturation1' incorporates:
   *  Sum: '<S10>/Add'
   */
  if (u > 2500.0) {
    u = 2500.0;
  } else if (u < 50.0) {
    u = 50.0;
  }

  /* Saturate: '<S72>/Saturation' */
  if (rtb_Divide_am > 1.0) {
    rtb_Divide_am = 1.0;
  } else if (rtb_Divide_am < 0.0) {
    rtb_Divide_am = 0.0;
  }

  /* MinMax: '<Root>/Min' incorporates:
   *  Constant: '<Root>/Constant1'
   *  Constant: '<S30>/Constant'
   *  Gain: '<S10>/Gain11'
   *  Product: '<Root>/Product1'
   *  Product: '<S10>/Product1'
   *  Saturate: '<S10>/Saturation1'
   *  Saturate: '<S5>/Saturation'
   *  Saturate: '<S72>/Saturation'
   *  Sum: '<S30>/Sum1'
   */
  rtb_SumofElements2 = fmin(fmin(rtP.T_max, u * rtb_Divide1_e_idx_3 *
    0.078947160665366667), (1.0 - rtb_Divide_am) * rtb_Product2_e);

  /* Gain: '<Root>/Gain1' incorporates:
   *  Saturate: '<Root>/Saturation5'
   */
  rtb_Divide_am = 0.10471975511965977 * rtb_Sum1_j;

  /* Product: '<S4>/Product2' */
  rtb_Divide1_e_idx_3 = rtb_SumofElements2 * rtb_Divide_am;

  /* Sum: '<S22>/Add' */
  rtb_Add1_b = ((rtb_Divide1_e_idx_0 + rtb_Divide1_e_idx_1) +
                rtb_Divide1_e_idx_2) + rtb_Divide1_e_idx_3;

  /* MinMax: '<S22>/Min' incorporates:
   *  Constant: '<S22>/Total Power Limit'
   */
  rtb_Saturation_g = fmin(rtb_Add1_b, rtP.Pmax);

  /* MinMax: '<S22>/Max' incorporates:
   *  Constant: '<S22>/Constant'
   */
  rtb_Add1_b = fmax(rtb_Add1_b, 0.1);

  /* Switch: '<S4>/Switch' incorporates:
   *  Gain: '<Root>/Gain1'
   */
  if (rtb_Product1_p_idx_0 != 0.0) {
    /* Outport: '<Root>/T_pos' incorporates:
     *  Constant: '<S4>/Constant'
     *  Constant: '<S4>/Constant1'
     *  MinMax: '<S4>/Max'
     *  Product: '<S22>/Divide'
     *  Product: '<S22>/Divide2'
     *  Product: '<S4>/Divide'
     *  Product: '<S4>/Divide1'
     *  RelationalOperator: '<S4>/Equal'
     */
    rtY.T_pos[0] = rtb_Divide1_e_idx_0 / rtb_Add1_b * rtb_Saturation_g / fmax
      (rtb_Product1_p_idx_0, 0.1) * (real_T)(rtb_Product1_p_idx_0 <=
      2094.3951023931954);
  } else {
    /* Outport: '<Root>/T_pos' */
    rtY.T_pos[0] = rtb_Product_dt;
  }

  if (rtb_Product1_p_idx_1 != 0.0) {
    /* Outport: '<Root>/T_pos' incorporates:
     *  Constant: '<S4>/Constant'
     *  Constant: '<S4>/Constant1'
     *  MinMax: '<S4>/Max'
     *  Product: '<S22>/Divide'
     *  Product: '<S22>/Divide2'
     *  Product: '<S4>/Divide'
     *  Product: '<S4>/Divide1'
     *  RelationalOperator: '<S4>/Equal'
     */
    rtY.T_pos[1] = rtb_Divide1_e_idx_1 / rtb_Add1_b * rtb_Saturation_g / fmax
      (rtb_Product1_p_idx_1, 0.1) * (real_T)(rtb_Product1_p_idx_1 <=
      2094.3951023931954);
  } else {
    /* Outport: '<Root>/T_pos' */
    rtY.T_pos[1] = rtb_Min_g_idx_1;
  }

  if (rtb_Product1_p_idx_2 != 0.0) {
    /* Outport: '<Root>/T_pos' incorporates:
     *  Constant: '<S4>/Constant'
     *  Constant: '<S4>/Constant1'
     *  MinMax: '<S4>/Max'
     *  Product: '<S22>/Divide'
     *  Product: '<S22>/Divide2'
     *  Product: '<S4>/Divide'
     *  Product: '<S4>/Divide1'
     *  RelationalOperator: '<S4>/Equal'
     */
    rtY.T_pos[2] = rtb_Divide1_e_idx_2 / rtb_Add1_b * rtb_Saturation_g / fmax
      (rtb_Product1_p_idx_2, 0.1) * (real_T)(rtb_Product1_p_idx_2 <=
      2094.3951023931954);
  } else {
    /* Outport: '<Root>/T_pos' */
    rtY.T_pos[2] = rtb_Saturation_a_idx_2;
  }

  if (rtb_Divide_am != 0.0) {
    /* Outport: '<Root>/T_pos' incorporates:
     *  Constant: '<S4>/Constant'
     *  Constant: '<S4>/Constant1'
     *  MinMax: '<S4>/Max'
     *  Product: '<S22>/Divide'
     *  Product: '<S22>/Divide2'
     *  Product: '<S4>/Divide'
     *  Product: '<S4>/Divide1'
     *  RelationalOperator: '<S4>/Equal'
     */
    rtY.T_pos[3] = rtb_Divide1_e_idx_3 / rtb_Add1_b * rtb_Saturation_g / fmax
      (rtb_Divide_am, 0.1) * (real_T)(rtb_Divide_am <= 2094.3951023931954);
  } else {
    /* Outport: '<Root>/T_pos' incorporates:
     *  MinMax: '<Root>/Min'
     */
    rtY.T_pos[3] = rtb_SumofElements2;
  }

  /* End of Switch: '<S4>/Switch' */

  /* Saturate: '<Root>/Saturation8' incorporates:
   *  Inport: '<Root>/voltage'
   */
  if (rtU.voltage > 600.0) {
    rtb_Add1_b = 600.0;
  } else if (rtU.voltage < 0.0) {
    rtb_Add1_b = 0.0;
  } else {
    rtb_Add1_b = rtU.voltage;
  }

  /* End of Saturate: '<Root>/Saturation8' */

  /* Saturate: '<Root>/Saturation4' incorporates:
   *  Inport: '<Root>/regen paddle'
   */
  if (rtU.regenpaddle > 1.0) {
    rtb_Iq_sqr = 1.0;
  } else if (rtU.regenpaddle < 0.0) {
    rtb_Iq_sqr = 0.0;
  } else {
    rtb_Iq_sqr = rtU.regenpaddle;
  }

  /* End of Saturate: '<Root>/Saturation4' */

  /* Sum: '<S24>/Add1' incorporates:
   *  Constant: '<S24>/Constant'
   *  Delay: '<S24>/Delay1'
   *  Memory: '<S24>/Memory'
   *  Product: '<S24>/Product2'
   */
  rtb_SumofElements2 = rtP.kRamp * 0.01 * (real_T)rtDW.Delay1_DSTATE[0] +
    rtDW.Memory_PreviousInput_k;

  /* RelationalOperator: '<S23>/Compare' incorporates:
   *  Constant: '<S23>/Constant'
   */
  rtb_Compare_j = (rtb_Iq_sqr > 0.0);

  /* Saturate: '<S24>/Saturation' incorporates:
   *  Constant: '<S24>/Constant1'
   *  Sum: '<S24>/Add'
   */
  if (100.0 - rtb_SumofElements2 > 100.0) {
    rtb_Product1_p_idx_0 = 100.0;
  } else if (100.0 - rtb_SumofElements2 < 65.0) {
    rtb_Product1_p_idx_0 = 65.0;
  } else {
    rtb_Product1_p_idx_0 = 100.0 - rtb_SumofElements2;
  }

  /* MinMax: '<S6>/Imax' incorporates:
   *  Constant: '<S6>/Constant'
   *  Constant: '<S6>/Max battery voltage'
   *  Gain: '<S6>/Gain'
   *  Product: '<S24>/Product'
   *  Product: '<S6>/Product'
   *  Saturate: '<S24>/Saturation'
   *  Sum: '<S6>/Subtract'
   */
  rtb_Saturation_g = fmin(fmin((564.96 - rtb_Add1_b) * 2.730524533762936, 100.0 *
    rtb_Iq_sqr), rtb_Product1_p_idx_0 * (real_T)rtb_Compare_j);

  /* Gain: '<S7>/Gain' incorporates:
   *  Gain: '<S6>/Gain1'
   *  Product: '<S6>/Product1'
   *  Sum: '<S6>/Tot voltage'
   */
  rtb_Divide_am = (0.36623 * rtb_Saturation_g + rtb_Add1_b) * rtb_Saturation_g *
    1.0204081632653061;

  /* RelationalOperator: '<S25>/Equal1' incorporates:
   *  Constant: '<S25>/Constant'
   */
  rtb_fw_inactive = (rtb_Divide_am != 0.0);

  /* Saturate: '<S7>/Saturation1' incorporates:
   *  Saturate: '<Root>/Saturation5'
   */
  if (rtb_Saturation1_a[0] > 2094.3951023931954) {
    rtb_Product1_p_idx_0 = 2094.3951023931954;
  } else {
    rtb_Product1_p_idx_0 = rtb_Saturation1_a[0];
  }

  if (rtb_Saturation1_a[1] > 2094.3951023931954) {
    rtb_Divide1_e_idx_3 = 2094.3951023931954;
  } else {
    rtb_Divide1_e_idx_3 = rtb_Saturation1_a[1];
  }

  if (rtb_Saturation1_a[2] > 2094.3951023931954) {
    rtb_Add1_b = 2094.3951023931954;
  } else {
    rtb_Add1_b = rtb_Saturation1_a[2];
  }

  if (rtb_Sum1_j > 2094.3951023931954) {
    rtb_Sum1_j = 2094.3951023931954;
  }

  /* Gain: '<S7>/Gain1' incorporates:
   *  Saturate: '<S7>/Saturation1'
   *  Sum: '<S7>/Add1'
   */
  rtb_Add1_b = (((rtb_Product1_p_idx_0 + rtb_Divide1_e_idx_3) + rtb_Add1_b) +
                rtb_Sum1_j) * 0.25;

  /* Gain: '<S25>/Gain5' */
  rtb_Product2_e = 5.0 * rtb_Add1_b;

  /* Gain: '<S28>/Gain4' incorporates:
   *  Constant: '<S28>/Constant'
   *  Gain: '<S28>/Gain6'
   *  Sum: '<S28>/Add1'
   */
  rtb_Saturation_g = (2.0271088232297343E-5 * rtb_Product2_e +
                      0.069426173522444928) * 1.5;

  /* Gain: '<S28>/2a' */
  rtb_Product_dt = 2.0 * rtb_Saturation_g;

  /* Gain: '<S28>/Gain3' */
  rtb_Iq_sqr = 0.0444 * rtb_Product2_e;

  /* Sum: '<S28>/Add3' incorporates:
   *  Gain: '<S28>/4a'
   *  Math: '<S28>/b^2'
   *  Product: '<S28>/4ac'
   */
  rtb_Saturation_g = 4.0 * rtb_Saturation_g * rtb_Divide_am + rtb_Iq_sqr *
    rtb_Iq_sqr;

  /* Sqrt: '<S28>/Signed Sqrt'
   *
   * About '<S28>/Signed Sqrt':
   *  Operator: signedSqrt
   */
  if (rtb_Saturation_g < 0.0) {
    rtb_Product1_p_idx_0 = -sqrt(fabs(rtb_Saturation_g));
  } else {
    rtb_Product1_p_idx_0 = sqrt(rtb_Saturation_g);
  }

  /* Product: '<S28>/Iq' incorporates:
   *  Sqrt: '<S28>/Signed Sqrt'
   *  Sum: '<S28>/Subtract'
   *
   * About '<S28>/Signed Sqrt':
   *  Operator: signedSqrt
   */
  rtb_Product_dt = 1.0 / rtb_Product_dt * (rtb_Product1_p_idx_0 - rtb_Iq_sqr);

  /* Gain: '<S28>/Gain1' */
  rtb_Saturation_g = -0.16892573526914451 * rtb_Product_dt;

  /* Math: '<S28>/Power1' */
  rtb_Saturation_g *= rtb_Saturation_g;

  /* MATLAB Function: '<S25>/MATLAB Function' */
  if (rtb_fw_inactive) {
    rtb_Product2_e = 0.0;
    rtb_Iq_sqr = 0.0;
  } else {
    rtb_Product2_e = rtb_Divide_am / 154.5855345755223 / rtb_Product2_e;
    rtb_Iq_sqr = (3.552E-6 - sqrt((0.02032128 - rtb_Product2_e * rtb_Product2_e)
      * 4.32E-8 + 5.0466816E-11)) / 4.32E-8;
    rtb_Product2_e = rtb_Iq_sqr * rtb_Iq_sqr;
    rtb_Iq_sqr = 352800.0 - rtb_Product2_e * rtb_Product2_e;
  }

  /* End of MATLAB Function: '<S25>/MATLAB Function' */

  /* Switch: '<S25>/Switch1' incorporates:
   *  Constant: '<S28>/Constant1'
   *  Gain: '<S25>/Gain7'
   *  Math: '<S28>/Power2'
   *  MinMax: '<S28>/Max'
   *  RelationalOperator: '<S25>/Equal'
   *  Sum: '<S28>/Add2'
   *  Switch: '<S25>/Switch'
   */
  if (0.047619047619047616 * rtb_Divide_am < rtb_Add1_b) {
    rtb_Iq_sqr = fmax(352800.0 - rtb_Saturation_g, rtb_Product_dt *
                      rtb_Product_dt);
    rtb_Product2_e = rtb_Saturation_g;
  }

  /* End of Switch: '<S25>/Switch1' */

  /* Sum: '<S26>/Subtract1' incorporates:
   *  Constant: '<S26>/Constant'
   *  Gain: '<S26>/Gain'
   */
  rtb_Divide1_e_idx_3 = 9.5492965855137211 * rtb_Add1_b - 13000.0;

  /* Saturate: '<S26>/Saturation' */
  if (rtb_Divide1_e_idx_3 < 0.0) {
    rtb_Divide1_e_idx_3 = 0.0;
  }

  /* Product: '<S7>/Divide1' incorporates:
   *  Constant: '<S26>/Constant1'
   *  Constant: '<S7>/Constant'
   *  Constant: '<S7>/Constant4'
   *  Gain: '<S25>/Gain'
   *  Gain: '<S26>/Gain1'
   *  MinMax: '<S7>/Max'
   *  MinMax: '<S7>/Min'
   *  Product: '<S25>/Divide'
   *  Product: '<S7>/Tmax'
   *  RelationalOperator: '<S7>/GreaterThan'
   *  Saturate: '<S26>/Saturation'
   *  Sum: '<S25>/Add'
   *  Sum: '<S26>/Subtract'
   *  Sum: '<S7>/Ptot'
   *  Switch: '<S25>/Switch'
   */
  rtb_Add1_b = fmin(((rtb_Product2_e + rtb_Iq_sqr) * 0.10125 * (real_T)
                     rtb_fw_inactive + rtb_Divide_am) / fmax(rtb_Add1_b, 1.0E-7),
                    21.0 - 0.000857 * rtb_Divide1_e_idx_3) * (real_T)(rtb_Add1_b
    > 0.0);

  /* Sum: '<Root>/Add5' incorporates:
   *  Constant: '<Root>/Constant2'
   *  Gain: '<Root>/Gain12'
   */
  rtb_Saturation_g = (1.0 - rtP.offset_brake_csi) * rtb_Csi +
    rtP.offset_brake_csi;

  /* Saturate: '<Root>/Saturation7' incorporates:
   *  Inport: '<Root>/brake pressure front'
   */
  if (rtU.brakepressurefront > 1.0E+7) {
    rtb_Product1_p_idx_0 = 1.0E+7;
  } else if (rtU.brakepressurefront < 0.0) {
    rtb_Product1_p_idx_0 = 0.0;
  } else {
    rtb_Product1_p_idx_0 = rtU.brakepressurefront;
  }

  /* Saturate: '<Root>/Saturation9' incorporates:
   *  Inport: '<Root>/brake pressure rear'
   */
  if (rtU.brakepressurerear > 1.0E+7) {
    rtb_Divide1_e_idx_3 = 1.0E+7;
  } else if (rtU.brakepressurerear < 0.0) {
    rtb_Divide1_e_idx_3 = 0.0;
  } else {
    rtb_Divide1_e_idx_3 = rtU.brakepressurerear;
  }

  /* Product: '<S27>/Product3' incorporates:
   *  Constant: '<S27>/Constant'
   *  Constant: '<S27>/Constant3'
   *  Constant: '<S27>/Constant5'
   *  Gain: '<S27>/Gain'
   *  Gain: '<S27>/Gain1'
   *  MinMax: '<S27>/Max'
   *  Product: '<S27>/Divide'
   *  Product: '<S27>/Divide1'
   *  Product: '<S27>/Divide2'
   *  Product: '<S27>/Divide3'
   *  Saturate: '<Root>/Saturation7'
   *  Saturate: '<Root>/Saturation9'
   *  Sum: '<S27>/Add'
   *  Sum: '<S27>/Add1'
   *  Sum: '<S27>/Add6'
   */
  rtb_Saturation_g = (1.0 - rtb_Add1_b * rtb_Saturation_g / fmax(((1.0 -
    rtb_Saturation_g) * (0.000207157153 * rtb_Product1_p_idx_0) + rtb_Add1_b) -
    0.00010373444249999999 * rtb_Divide1_e_idx_3 * rtb_Saturation_g, 1.0E-8)) *
    rtb_Add1_b;

  /* Outport: '<Root>/T_neg' incorporates:
   *  Gain: '<S7>/Gain3'
   */
  rtY.T_neg[0] = -rtb_Add1_b;
  rtY.T_neg[1] = -rtb_Add1_b;
  rtY.T_neg[2] = -rtb_Saturation_g;
  rtY.T_neg[3] = -rtb_Saturation_g;

  /* SignalConversion generated from: '<S3>/Vector Concatenate' incorporates:
   *  Memory: '<S3>/Memory2'
   */
  rtDW.Memory2_PreviousInput[0] = rtb_Saturation;

  /* SignalConversion generated from: '<S3>/Vector Concatenate' incorporates:
   *  Memory: '<S3>/Memory2'
   */
  rtDW.Memory2_PreviousInput[1] = rtb_Saturation1;

  /* Update for Memory: '<S3>/Memory4' */
  rtDW.Memory4_PreviousInput = rtb_Saturation2;

  /* Update for Memory: '<Root>/Memory' */
  rtDW.Memory_PreviousInput_e = rtb_K_k[0];

  /* Update for Memory: '<S3>/Memory1' */
  rtDW.Memory1_PreviousInput[0] = rtb_K_k[0];
  rtDW.Memory1_PreviousInput[1] = rtb_Exp4_idx_0;

  /* Update for Memory: '<S21>/Memory' */
  rtDW.Memory_PreviousInput_n[0] = rtb_vims_idx_0;

  /* Update for DiscreteIntegrator: '<S65>/Integrator' incorporates:
   *  Gain: '<S62>/Integral Gain'
   */
  rtb_Sum1_j = rtP.TC_map[1] * rtb_Saturation_k_idx_0 * 0.01 +
    rtDW.Integrator_DSTATE[0];
  rtDW.Integrator_DSTATE[0] = rtb_Sum1_j;
  if (rtb_Sum1_j > 1.0) {
    rtDW.Integrator_DSTATE[0] = 1.0;
  } else if (rtb_Sum1_j < 0.0) {
    rtDW.Integrator_DSTATE[0] = 0.0;
  }

  /* Update for DiscreteIntegrator: '<S60>/Filter' */
  rtDW.Filter_DSTATE[0] += 0.01 * rtb_Product5_o_idx_0;

  /* Update for Memory: '<S21>/Memory' */
  rtDW.Memory_PreviousInput_n[1] = rtb_vims_idx_1;

  /* Update for DiscreteIntegrator: '<S65>/Integrator' incorporates:
   *  Gain: '<S62>/Integral Gain'
   */
  rtb_Sum1_j = rtP.TC_map[1] * rtb_Saturation_k_idx_1 * 0.01 +
    rtDW.Integrator_DSTATE[1];
  rtDW.Integrator_DSTATE[1] = rtb_Sum1_j;
  if (rtb_Sum1_j > 1.0) {
    rtDW.Integrator_DSTATE[1] = 1.0;
  } else if (rtb_Sum1_j < 0.0) {
    rtDW.Integrator_DSTATE[1] = 0.0;
  }

  /* Update for DiscreteIntegrator: '<S60>/Filter' */
  rtDW.Filter_DSTATE[1] += 0.01 * rtb_Saturation_a_idx_0;

  /* Update for Memory: '<S21>/Memory' */
  rtDW.Memory_PreviousInput_n[2] = rtb_vims_idx_2;

  /* Update for DiscreteIntegrator: '<S65>/Integrator' incorporates:
   *  Gain: '<S62>/Integral Gain'
   */
  rtb_Sum1_j = rtP.TC_map[1] * rtb_Saturation_k_idx_2 * 0.01 +
    rtDW.Integrator_DSTATE[2];
  rtDW.Integrator_DSTATE[2] = rtb_Sum1_j;
  if (rtb_Sum1_j > 1.0) {
    rtDW.Integrator_DSTATE[2] = 1.0;
  } else if (rtb_Sum1_j < 0.0) {
    rtDW.Integrator_DSTATE[2] = 0.0;
  }

  /* Update for DiscreteIntegrator: '<S60>/Filter' */
  rtDW.Filter_DSTATE[2] += 0.01 * rtb_Saturation_a_idx_1;

  /* Update for Memory: '<S21>/Memory' */
  rtDW.Memory_PreviousInput_n[3] = rtb_vims_idx_3;

  /* Update for DiscreteIntegrator: '<S65>/Integrator' incorporates:
   *  Gain: '<S62>/Integral Gain'
   *  Saturate: '<S30>/Saturation'
   */
  rtb_Sum1_j = rtP.TC_map[1] * tmp * 0.01 + rtDW.Integrator_DSTATE[3];
  rtDW.Integrator_DSTATE[3] = rtb_Sum1_j;
  if (rtb_Sum1_j > 1.0) {
    rtDW.Integrator_DSTATE[3] = 1.0;
  } else if (rtb_Sum1_j < 0.0) {
    rtDW.Integrator_DSTATE[3] = 0.0;
  }

  /* Update for DiscreteIntegrator: '<S60>/Filter' incorporates:
   *  Gain: '<S68>/Filter Coefficient'
   */
  rtDW.Filter_DSTATE[3] += 0.01 * rtb_Saturation_k_0;

  /* Update for Memory: '<S24>/Memory' */
  rtDW.Memory_PreviousInput_k = rtb_SumofElements2;

  /* Update for Delay: '<S24>/Delay1' */
  for (i = 0; i < 99; i++) {
    rtDW.Delay1_DSTATE[i] = rtDW.Delay1_DSTATE[i + 1];
  }

  rtDW.Delay1_DSTATE[99] = rtb_Compare_j;

  /* End of Update for Delay: '<S24>/Delay1' */
}

/* Model initialize function */
void Torque_Vectoring_2024_forsecontiu_initialize(void)
{
  /* InitializeConditions for Memory: '<S3>/Memory2' */
  rtDW.Memory2_PreviousInput[0] = 1.3;
  rtDW.Memory2_PreviousInput[1] = 0.0;
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
