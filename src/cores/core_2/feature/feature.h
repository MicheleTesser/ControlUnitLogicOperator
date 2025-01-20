#ifndef __CORE_2_FEATURES__

#include "DV/dv.h"
#include "dv_driver_input/dv_driver_input.h"
#include "mission/mission.h"

#endif // !__CORE_2_FEATURES__

#ifndef __CORE_2_FEATURE__
#define __CORE_2_FEATURE__

#include <stdint.h>

typedef struct Core2Feature_h{
    const uint8_t private_data[2];
}Core2Feature_h;

int8_t
core_2_feature_init(Core2Feature_h* const restrict self __attribute__((__nonnull__)));

int8_t
core_2_feature_update(Core2Feature_h* const restrict self __attribute__((__nonnull__)));

#endif // !__CORE_2_FEATURE__
