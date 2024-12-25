/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: rt_nonfinite.c
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

#include "rtGetNaN.h"
#include "rtGetInf.h"
#include <stddef.h>
#include "rtwtypes.h"
#include "rt_nonfinite.h"
#define NumBitsPerChar                 16U

real_T RigenrtInf;
real_T RigenrtMinusInf;
real_T RigenrtNaN;
real32_T RigenrtInfF;
real32_T RigenrtMinusInfF;
real32_T RigenrtNaNF;

/*
 * Initialize the RigenrtInf, RigenrtMinusInf, and RigenrtNaN needed by the
 * generated code. NaN is initialized as non-signaling. Assumes IEEE.
 */
void Rigenrt_InitInfAndNaN(size_t realSize)
{
  (void) (realSize);
  RigenrtNaN = RigenrtGetNaN();
  RigenrtNaNF = RigenrtGetNaNF();
  RigenrtInf = RigenrtGetInf();
  RigenrtInfF = RigenrtGetInfF();
  RigenrtMinusInf = RigenrtGetMinusInf();
  RigenrtMinusInfF = RigenrtGetMinusInfF();
}

/* Test if value is infinite */
boolean_T RigenrtIsInf(real_T value)
{
  return (boolean_T)((value==RigenrtInf || value==RigenrtMinusInf) ? 1U : 0U);
}

/* Test if single-precision value is infinite */
boolean_T RigenrtIsInfF(real32_T value)
{
  return (boolean_T)(((value)==RigenrtInfF || (value)==RigenrtMinusInfF) ? 1U : 0U);
}

/* Test if value is not a number */
boolean_T RigenrtIsNaN(real_T value)
{
  boolean_T result = (boolean_T) 0;
  size_t bitsPerReal = sizeof(real_T) * (NumBitsPerChar);
  if (bitsPerReal == 32U) {
    result = RigenrtIsNaNF((real32_T)value);
  } else {
    union {
      LittleEndianIEEEDouble bitVal;
      real_T fltVal;
    } tmpVal;

    tmpVal.fltVal = value;
    result = (boolean_T)((tmpVal.bitVal.words.wordH & 0x7FF00000) == 0x7FF00000 &&
                         ( (tmpVal.bitVal.words.wordH & 0x000FFFFF) != 0 ||
                          (tmpVal.bitVal.words.wordL != 0) ));
  }

  return result;
}

/* Test if single-precision value is not a number */
boolean_T RigenrtIsNaNF(real32_T value)
{
  IEEESingle tmp;
  tmp.wordL.wordLreal = value;
  return (boolean_T)( (tmp.wordL.wordLuint & 0x7F800000) == 0x7F800000 &&
                     (tmp.wordL.wordLuint & 0x007FFFFF) != 0 );
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
