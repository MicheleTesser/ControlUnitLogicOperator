/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: rigen_fun_simulink22.h
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

#ifndef RTW_HEADER_rigen_fun_simulink22_h_
#define RTW_HEADER_rigen_fun_simulink22_h_
#ifndef rigen_fun_simulink22_COMMON_INCLUDES_
#define rigen_fun_simulink22_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                               /* rigen_fun_simulink22_COMMON_INCLUDES_ */

#include "rigen_fun_simulink22_types.h"
#include "rtGetInf.h"
#include <stddef.h>
#include "rt_nonfinite.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

extern void configureCPU2Peripherals(uint32_T gpioNumber, uint32_T gpGRegValA,
  uint32_T gpRegValA);

typedef struct {
    real_T voltage;
    real_T rpmFR;
    real_T rpmFL;
    real_T rpmRR;
    real_T rpmRL;
} ExtU_rigen_fun_simulink22_T;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  real_T TorqueFL;                     /* '<Root>/TorqueFL' */
  real_T TorqueFR;                     /* '<Root>/TorqueFR' */
  real_T TorqueRR;                     /* '<Root>/TorqueRR' */
  real_T TorqueRL;                     /* '<Root>/TorqueRL' */
} ExtY_rigen_fun_simulink22_T;

/* Parameters (default storage) */
struct P_rigen_fun_simulink22_T_ {
  real_T Tmax;                         /* Variable: Tmax
                                        * Referenced by: '<Root>/Saturation'
                                        */
  real_T csi_opt;                      /* Variable: csi_opt
                                        * Referenced by: '<Root>/csi_opt'
                                        */
  real_T max_curr;                     /* Variable: max_curr
                                        * Referenced by: '<Root>/max_curr'
                                        */
  real_T rpmFL;                        /* Variable: rpmFL
                                        * Referenced by: '<Root>/Constant6'
                                        */
  real_T rpmFR;                        /* Variable: rpmFR
                                        * Referenced by: '<Root>/Constant'
                                        */
  real_T rpmRL;                        /* Variable: rpmRL
                                        * Referenced by: '<Root>/Constant1'
                                        */
  real_T rpmRR;                        /* Variable: rpmRR
                                        * Referenced by: '<Root>/Constant2'
                                        */
  real_T tensione;                     /* Variable: tensione
                                        * Referenced by: '<Root>/tensione'
                                        */
  real_T Gain_Gain;                    /* Expression: 0.25
                                        * Referenced by: '<Root>/Gain'
                                        */
  real_T rpm2rads_Gain;                /* Expression: pi/30
                                        * Referenced by: '<Root>/rpm2rad//s'
                                        */
  real_T Rbatt_Value;                  /* Expression: 0.231
                                        * Referenced by: '<S2>/Rbatt'
                                        */
  real_T max_voltage_Value;            /* Expression: 564.94
                                        * Referenced by: '<S2>/max_voltage'
                                        */
  real_T Constant2_Value;              /* Expression: 0.1
                                        * Referenced by: '<S2>/Constant2'
                                        */
  real_T Gain_Gain_h;                  /* Expression: 1/0.98
                                        * Referenced by: '<S2>/Gain'
                                        */
  real_T Gain1_Gain;                   /* Expression: 0.5
                                        * Referenced by: '<Root>/Gain1'
                                        */
  real_T R_Value;                      /* Expression: 0.135/2
                                        * Referenced by: '<S1>/R'
                                        */
  real_T flux_PM_Value;                /* Expression: 0.0296
                                        * Referenced by: '<S1>/flux_PM'
                                        */
  real_T rads2radselettrici_Gain;      /* Expression: 5
                                        * Referenced by: '<Root>/rad//s2rad//s elettrici'
                                        */
  real_T Gain_Gain_e;                  /* Expression: 1.5
                                        * Referenced by: '<S4>/Gain'
                                        */
  real_T Ld_Value;                     /* Expression: 0.12e-3
                                        * Referenced by: '<S1>/Ld'
                                        */
  real_T Lq_Value;                     /* Expression: 0.24e-3
                                        * Referenced by: '<S1>/Lq'
                                        */
  real_T tan_alfa_Value;               /* Expression: -5.919761121102884
                                        * Referenced by: '<S1>/tan_alfa'
                                        */
  real_T Constant_Value;               /* Expression: 1
                                        * Referenced by: '<S3>/Constant'
                                        */
  real_T R_Value_m;                    /* Expression: 0.132/2
                                        * Referenced by: '<S3>/R'
                                        */
  real_T Gain1_Gain_g;                 /* Expression: 1.5
                                        * Referenced by: '<S3>/Gain1'
                                        */
  real_T Gain_Gain_o;                  /* Expression: -1
                                        * Referenced by: '<S1>/Gain'
                                        */
  real_T Gain1_Gain_l;                 /* Expression: 4
                                        * Referenced by: '<S1>/Gain1'
                                        */
  real_T Gain2_Gain;                   /* Expression: 2
                                        * Referenced by: '<S1>/Gain2'
                                        */
  real_T Gain3_Gain;                   /* Expression: 1.5
                                        * Referenced by: '<S1>/Gain3'
                                        */
  real_T Saturation_LowerSat;          /* Expression: 0
                                        * Referenced by: '<Root>/Saturation'
                                        */
  real_T Gain3_Gain_f;                 /* Expression: -1
                                        * Referenced by: '<Root>/Gain3'
                                        */
  real_T Constant7_Value;              /* Expression: 1
                                        * Referenced by: '<Root>/Constant7'
                                        */
};

/* Real-time Model Data Structure */
struct tag_RTM_rigen_fun_simulink22_T {
  const char_T * volatile errorStatus;
};

/* Block parameters (default storage) */
extern P_rigen_fun_simulink22_T rigen_fun_simulink22_P;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY_rigen_fun_simulink22_T rigen_fun_simulink22_Y;
extern ExtU_rigen_fun_simulink22_T rigen_fun_simulink22_U;

/* Model entry point functions */
extern void rigen_fun_simulink22_initialize(void);
extern void rigen_fun_simulink22_step(void);

/* Real-time Model object */
extern RT_MODEL_rigen_fun_simulink22_T *const rigen_fun_simulink22_M;
extern volatile boolean_T stopRequested;
extern volatile boolean_T runModel;

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
 * '<Root>' : 'rigen_fun_simulink22'
 * '<S1>'   : 'rigen_fun_simulink22/Power loss for a front motor'
 * '<S2>'   : 'rigen_fun_simulink22/Subsystem'
 * '<S3>'   : 'rigen_fun_simulink22/Power loss for a front motor/a coefficient generation'
 * '<S4>'   : 'rigen_fun_simulink22/Power loss for a front motor/b coefficient generation'
 */
#endif                                 /* RTW_HEADER_rigen_fun_simulink22_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
