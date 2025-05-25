#ifndef __REGEN_ALG__
#define __REGEN_ALG__
#include "../../../engines/engines.h"
#include <stdint.h>

struct RegenAlgInput {
    const float front_left_velocity;
    const float front_right_velocity;
    const float rear_left_velocity;
    const float rear_right_velocity;
    const float battery_pack_tension;
};

void regen_alg_init(void);
void regen_alg_compute(const struct RegenAlgInput* const restrict input,
        float o_negTorquesNM[__NUM_OF_ENGINES__]);

#endif // !__REGEN_ALG__
