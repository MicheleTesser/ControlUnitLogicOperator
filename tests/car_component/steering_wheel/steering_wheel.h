#ifndef __DUMMY__
#define __DUMMY__

#include <stdint.h>
#include "../src/cores/core_utility/mission_reader/mission_reader.h"

typedef struct __attribute__((aligned(8))) SteeringWheel_h{
  const uint8_t private_data[32];
}SteeringWheel_h;

int8_t
steering_wheel_start(struct SteeringWheel_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t
steering_wheel_select_mission(struct SteeringWheel_h* const restrict self,
    const enum CAR_MISSIONS mission)__attribute__((__nonnull__(1)));

int8_t
steering_wheel_stop(struct SteeringWheel_h* const restrict self)__attribute__((__nonnull__(1)));

#endif // !__DUMMY__
