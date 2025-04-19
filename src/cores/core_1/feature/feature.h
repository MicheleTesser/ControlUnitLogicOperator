#ifndef __CORE_1_FEATURE__
#define __CORE_1_FEATURE__

#include "log/log.h"

#include <stdint.h>

typedef struct __attribute__((aligned(4))) Core1Feature_h{
  const uint8_t private_data[148];
}Core1Feature_h;

int8_t
core_1_feature_init(Core1Feature_h* const restrict self, Log_h* const restrict p_log)
  __attribute__((__nonnull__(1,2)));

int8_t
core_1_feature_update(Core1Feature_h* const restrict self )__attribute__((__nonnull__(1)));

#endif // !__CORE_1_FEATURE__
