#ifndef __GIEI_POWER_CONTROL__
#define __GIEI_POWER_CONTROL__

#include "../engine_common.h"

void powerControl(const float total_power, const float power_limit, 
        float posTorquesNM[NUM_OF_EGINES]);

#endif // !__GIEI_POWER_CONTROL__
