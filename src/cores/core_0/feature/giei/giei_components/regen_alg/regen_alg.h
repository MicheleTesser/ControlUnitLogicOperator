#ifndef __REGEN_ALG__
#define __REGEN_ALG__
#include "../../../engines/engines.h"
#include <stdint.h>

struct RegenAlgInput {
    const int16_t front_left_velocity;
    const int16_t front_right_velocity;
    const int16_t rear_left_velocity;
    const int16_t rear_right_velocity;
    const uint16_t battery_pack_tension;
};

void regen_alg_init(void);
void regen_alg_compute(const struct RegenAlgInput* const restrict input,
        float o_negTorquesNM[__NUM_OF_ENGINES__]);

#endif // !__REGEN_ALG__
