#ifndef __CORE_1_FEATURE__
#define __CORE_1_FEATURE__

#include <stdint.h>

typedef struct Core1Feature_h{
    const uint8_t private_data[1];
}Core1Feature_h;

int8_t
core_1_feature_init(Core1Feature_h* const restrict self )__attribute__((__nonnull__(1)));

int8_t
core_1_feature_update(Core1Feature_h* const restrict self )__attribute__((__nonnull__(1)));

#endif // !__CORE_1_FEATURE__
