#ifndef __CAR_TV_ALG__
#define __CAR_TV_ALG__

#include "../../../engines/engines.h"

struct TVInputArgs{
  float ax;                           /* '<Root>/ax' */
  float ay;                           /* '<Root>/ay' */
  float yaw_r;                        /* '<Root>/yaw_r' */
  float throttle;                     /* '<Root>/throttle' */
  float brake;                        /* '<Root>/regen paddle' */
  float steering;                     /* '<Root>/steering' */
  float rpm[4];                       /* '<Root>/rpm' */
};

void tv_alg_init(void);
void tv_alg_compute(const struct TVInputArgs* const restrict input,
        float o_posTorquesNM[__NUM_OF_ENGINES__]);

#endif // !__CAR_TV_ALG__
