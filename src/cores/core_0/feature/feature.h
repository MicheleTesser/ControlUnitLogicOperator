#ifndef __CORE_0_FEATURE__
#define __CORE_0_FEATURE__

#include <stdint.h>

typedef struct Core0Feature_h{
    const uint8_t private_data[1];
}Core0Feature_h;

int8_t
core_0_feature_init(Core0Feature_h* const restrict self __attribute__((__nonnull__)));

int8_t
core_0_feature_update(Core0Feature_h* const restrict self __attribute__((__nonnull__)));

#endif // !__CORE_0_FEATURE__
