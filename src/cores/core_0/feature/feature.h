#ifndef __CORE_0_FEATURE__
#define __CORE_0_FEATURE__

#include <stdint.h>
#include "../../../lib/raceup_board/raceup_board.h"

typedef struct __attribute__((aligned(4))){
  const uint8_t private_data[656];
}Core0Feature_h;

int8_t
core_0_feature_init(Core0Feature_h* const restrict self )__attribute__((__nonnull__(1)));

int8_t
core_0_feature_update(Core0Feature_h* const restrict self )__attribute__((__nonnull__(1)));

int8_t
core_0_feature_compute_power(Core0Feature_h* const restrict self )__attribute__((__nonnull__(1)));

#endif // !__CORE_0_FEATURE__
