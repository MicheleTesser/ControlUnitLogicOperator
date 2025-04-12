#ifndef __CORE_2_FEATURE__
#define __CORE_2_FEATURE__

#include <stdint.h>

typedef struct __attribute__((aligned(8))){
  const uint8_t private_data[400];
}Core2Feature_h;

int8_t
core_2_feature_init(Core2Feature_h* const restrict self )__attribute__((__nonnull__(1)));

int8_t
core_2_feature_update(Core2Feature_h* const restrict self )__attribute__((__nonnull__(1)));

#endif // !__CORE_2_FEATURE__
