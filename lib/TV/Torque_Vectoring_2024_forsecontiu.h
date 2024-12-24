/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: Torque_Vectoring_2024_forsecontiu.h
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

#ifndef Torque_Vectoring_2024_forsecontiu_h_
#define Torque_Vectoring_2024_forsecontiu_h_
#ifndef Torque_Vectoring_2024_forsecontiu_COMMON_INCLUDES_
#define Torque_Vectoring_2024_forsecontiu_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "math.h"
#endif                  /* Torque_Vectoring_2024_forsecontiu_COMMON_INCLUDES_ */

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

/* Forward declaration for rtModel */
typedef struct tag_RTM RT_MODEL;

/* Block signals and states (default storage) for system '<Root>' */
typedef struct {
  real_T Integrator_DSTATE[4];         /* '<S65>/Integrator' */
  real_T Filter_DSTATE[4];             /* '<S60>/Filter' */
  real_T Memory_PreviousInput[4];      /* '<S14>/Memory' */
  real_T Memory1_PreviousInput[2];     /* '<S3>/Memory1' */
  real_T Memory_PreviousInput_n[4];    /* '<S21>/Memory' */
  real_T Memory2_PreviousInput[2];     /* '<S3>/Memory2' */
  real_T Memory4_PreviousInput;        /* '<S3>/Memory4' */
  real_T Divide_DWORK4;                /* '<S14>/Divide' */
  real_T Memory_PreviousInput_e;       /* '<Root>/Memory' */
  real_T Memory_PreviousInput_k;       /* '<S24>/Memory' */
  boolean_T Delay1_DSTATE[100];        /* '<S24>/Delay1' */
} DW;

/* Invariant block signals (default storage) */
typedef struct {
  const real_T Gain;                   /* '<S12>/Gain' */
} ConstB;

/* Constant parameters (default storage) */
typedef struct {
  /* Expression: [1 0]
   * Referenced by: '<S14>/Constant4'
   */
  real_T Constant4_Value[2];

  /* Expression: Q
   * Referenced by: '<S14>/Constant7'
   */
  real_T Constant7_Value_d[4];
} ConstP;

/* External inputs (root inport signals with default storage) */
typedef struct {
  real_T ax;                           /* '<Root>/ax' */
  real_T ay;                           /* '<Root>/ay' */
  real_T yaw_r;                        /* '<Root>/yaw_r' */
  real_T throttle;                     /* '<Root>/throttle' */
  real_T regenpaddle;                  /* '<Root>/regen paddle' */
  real_T brakepressurefront;           /* '<Root>/brake pressure front' */
  real_T brakepressurerear;            /* '<Root>/brake pressure rear' */
  real_T steering;                     /* '<Root>/steering' */
  real_T rpm[4];                       /* '<Root>/rpm' */
  real_T voltage;                      /* '<Root>/voltage' */
} ExtU;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  real_T T_pos[4];                     /* '<Root>/T_pos' */
  real_T T_neg[4];                     /* '<Root>/T_neg' */
} ExtY;

/* Parameters (default storage) */
struct P_ {
  real_T Pmax;                         /* Variable: Pmax
                                        * Referenced by: '<S22>/Total Power Limit'
                                        */
  real_T TC_map[3];                    /* Variable: TC_map
                                        * Referenced by:
                                        *   '<S58>/Derivative Gain'
                                        *   '<S62>/Integral Gain'
                                        *   '<S70>/Proportional Gain'
                                        */
  real_T T_max;                        /* Variable: T_max
                                        * Referenced by:
                                        *   '<Root>/Constant1'
                                        *   '<S9>/Gain'
                                        */
  real_T kRamp;                        /* Variable: kRamp
                                        * Referenced by: '<S24>/Constant'
                                        */
  real_T offset_brake_csi;             /* Variable: offset_brake_csi
                                        * Referenced by:
                                        *   '<Root>/Constant2'
                                        *   '<Root>/Gain12'
                                        */
  real_T offset_cornering;             /* Variable: offset_cornering
                                        * Referenced by:
                                        *   '<S82>/Constant1'
                                        *   '<S82>/Multiply'
                                        */
  real_T offset_throttle_csi;          /* Variable: offset_throttle_csi
                                        * Referenced by:
                                        *   '<S9>/Constant'
                                        *   '<S9>/Gain1'
                                        *   '<S9>/Gain4'
                                        */
};

/* Parameters (default storage) */
typedef struct P_ P;

/* Real-time Model Data Structure */
struct tag_RTM {
  const char_T * volatile errorStatus;
};

/* Block parameters (default storage) */
extern P rtP;

/* Block signals and states (default storage) */
extern DW rtDW;

/* External inputs (root inport signals with default storage) */
extern ExtU rtU;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY rtY;
extern const ConstB rtConstB;          /* constant block i/o */

/* Constant parameters (default storage) */
extern const ConstP rtConstP;

/* Model entry point functions */
extern void Torque_Vectoring_2024_forsecontiu_initialize(void);
extern void Torque_Vectoring_2024_forsecontiu_step(void);

/* Real-time Model object */
extern RT_MODEL *const rtM;

/*-
 * These blocks were eliminated from the model due to optimizations:
 *
 * Block '<S22>/Scope' : Unused code path elimination
 * Block '<S25>/Scope1' : Unused code path elimination
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
 * '<Root>' : 'Torque_Vectoring_2024_forsecontiu'
 * '<S1>'   : 'Torque_Vectoring_2024_forsecontiu/Csi'
 * '<S2>'   : 'Torque_Vectoring_2024_forsecontiu/Delta estimators'
 * '<S3>'   : 'Torque_Vectoring_2024_forsecontiu/KF'
 * '<S4>'   : 'Torque_Vectoring_2024_forsecontiu/Power Limitation'
 * '<S5>'   : 'Torque_Vectoring_2024_forsecontiu/Re'
 * '<S6>'   : 'Torque_Vectoring_2024_forsecontiu/Regenerative Power'
 * '<S7>'   : 'Torque_Vectoring_2024_forsecontiu/Regenerative braking'
 * '<S8>'   : 'Torque_Vectoring_2024_forsecontiu/TC'
 * '<S9>'   : 'Torque_Vectoring_2024_forsecontiu/TV '
 * '<S10>'  : 'Torque_Vectoring_2024_forsecontiu/Torque limit'
 * '<S11>'  : 'Torque_Vectoring_2024_forsecontiu/Vertical loads calculator'
 * '<S12>'  : 'Torque_Vectoring_2024_forsecontiu/Wheels Angle '
 * '<S13>'  : 'Torque_Vectoring_2024_forsecontiu/KF/Compare To Constant'
 * '<S14>'  : 'Torque_Vectoring_2024_forsecontiu/KF/Matrices'
 * '<S15>'  : 'Torque_Vectoring_2024_forsecontiu/KF/Measurement'
 * '<S16>'  : 'Torque_Vectoring_2024_forsecontiu/KF/Measurement Update'
 * '<S17>'  : 'Torque_Vectoring_2024_forsecontiu/KF/Time Update'
 * '<S18>'  : 'Torque_Vectoring_2024_forsecontiu/KF/Measurement/Compare To Constant2'
 * '<S19>'  : 'Torque_Vectoring_2024_forsecontiu/KF/Measurement/Compare To Constant3'
 * '<S20>'  : 'Torque_Vectoring_2024_forsecontiu/KF/Measurement/Compare To Constant4'
 * '<S21>'  : 'Torque_Vectoring_2024_forsecontiu/KF/Measurement/Weights'
 * '<S22>'  : 'Torque_Vectoring_2024_forsecontiu/Power Limitation/Subsystem'
 * '<S23>'  : 'Torque_Vectoring_2024_forsecontiu/Regenerative Power/Compare To Zero'
 * '<S24>'  : 'Torque_Vectoring_2024_forsecontiu/Regenerative Power/ramp'
 * '<S25>'  : 'Torque_Vectoring_2024_forsecontiu/Regenerative braking/Engine'
 * '<S26>'  : 'Torque_Vectoring_2024_forsecontiu/Regenerative braking/Torque limit'
 * '<S27>'  : 'Torque_Vectoring_2024_forsecontiu/Regenerative braking/csi_regen'
 * '<S28>'  : 'Torque_Vectoring_2024_forsecontiu/Regenerative braking/Engine/Id Iq'
 * '<S29>'  : 'Torque_Vectoring_2024_forsecontiu/Regenerative braking/Engine/MATLAB Function'
 * '<S30>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller'
 * '<S31>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller'
 * '<S32>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/Anti-windup'
 * '<S33>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/D Gain'
 * '<S34>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/External Derivative'
 * '<S35>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/Filter'
 * '<S36>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/Filter ICs'
 * '<S37>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/I Gain'
 * '<S38>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/Ideal P Gain'
 * '<S39>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/Ideal P Gain Fdbk'
 * '<S40>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/Integrator'
 * '<S41>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/Integrator ICs'
 * '<S42>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/N Copy'
 * '<S43>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/N Gain'
 * '<S44>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/P Copy'
 * '<S45>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/Parallel P Gain'
 * '<S46>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/Reset Signal'
 * '<S47>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/Saturation'
 * '<S48>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/Saturation Fdbk'
 * '<S49>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/Sum'
 * '<S50>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/Sum Fdbk'
 * '<S51>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/Tracking Mode'
 * '<S52>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/Tracking Mode Sum'
 * '<S53>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/Tsamp - Integral'
 * '<S54>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/Tsamp - Ngain'
 * '<S55>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/postSat Signal'
 * '<S56>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/preSat Signal'
 * '<S57>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/Anti-windup/Passthrough'
 * '<S58>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/D Gain/Internal Parameters'
 * '<S59>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/External Derivative/Error'
 * '<S60>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/Filter/Disc. Forward Euler Filter'
 * '<S61>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/Filter ICs/Internal IC - Filter'
 * '<S62>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/I Gain/Internal Parameters'
 * '<S63>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/Ideal P Gain/Passthrough'
 * '<S64>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/Ideal P Gain Fdbk/Disabled'
 * '<S65>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/Integrator/Discrete'
 * '<S66>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/Integrator ICs/Internal IC'
 * '<S67>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/N Copy/Disabled'
 * '<S68>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/N Gain/Internal Parameters'
 * '<S69>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/P Copy/Disabled'
 * '<S70>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/Parallel P Gain/Internal Parameters'
 * '<S71>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/Reset Signal/Disabled'
 * '<S72>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/Saturation/Enabled'
 * '<S73>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/Saturation Fdbk/Disabled'
 * '<S74>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/Sum/Sum_PID'
 * '<S75>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/Sum Fdbk/Disabled'
 * '<S76>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/Tracking Mode/Disabled'
 * '<S77>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/Tracking Mode Sum/Passthrough'
 * '<S78>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/Tsamp - Integral/TsSignalSpecification'
 * '<S79>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/Tsamp - Ngain/Passthrough'
 * '<S80>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/postSat Signal/Forward_Path'
 * '<S81>'  : 'Torque_Vectoring_2024_forsecontiu/TC/PI controller/PID Controller/preSat Signal/Forward_Path'
 * '<S82>'  : 'Torque_Vectoring_2024_forsecontiu/TV /Ripartizione1'
 * '<S83>'  : 'Torque_Vectoring_2024_forsecontiu/TV /Ripartizione1/Rpartizione normalizzata'
 */
#endif                                /* Torque_Vectoring_2024_forsecontiu_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
