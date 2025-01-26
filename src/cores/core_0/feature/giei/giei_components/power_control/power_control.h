#ifndef __GIEI_POWER_CONTROL__
#define __GIEI_POWER_CONTROL__

#include "../../../engines/engines.h"

void powerControl(const float total_power, const float power_limit, 
        float posTorquesNM[__NUM_OF_ENGINES__]);

#endif // !__GIEI_POWER_CONTROL__
