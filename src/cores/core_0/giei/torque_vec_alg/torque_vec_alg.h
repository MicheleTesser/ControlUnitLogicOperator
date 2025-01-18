#ifndef __CAR_TV_ALG__
#define __CAR_TV_ALG__

#include "../engines/engine_common.h"

struct TVInputArgs{
  float ax;                           /* '<Root>/ax' */
  float ay;                           /* '<Root>/ay' */
  float yaw_r;                        /* '<Root>/yaw_r' */
  float throttle;                     /* '<Root>/throttle' */
  float regenpaddle;                  /* '<Root>/regen paddle' */
  float brakepressurefront;           /* '<Root>/brake pressure front' */
  float brakepressurerear;            /* '<Root>/brake pressure rear' */
  float steering;                     /* '<Root>/steering' */
  float rpm[4];                       /* '<Root>/rpm' */
  float voltage;                      /* '<Root>/voltage' */
};

void tv_alg_init(void);
void tv_alg_compute(const struct TVInputArgs* const restrict input,
        float o_posTorquesNM[NUM_OF_EGINES]);

#endif // !__CAR_TV_ALG__
