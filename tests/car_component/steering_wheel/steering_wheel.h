#ifndef __STEERING_WHEEL__
#define __STEERING_WHEEL__

#include <stdint.h>
#include "../src/cores/core_utility/mission_reader/mission_reader.h"

typedef struct __attribute__((aligned(4))){
  const uint8_t private_data[20];
}SteeringWheel_h;

//INFO: copied from src/cores/core_0/feature/maps/maps.c
enum MAPS_TYPE{
    MAPS_TYPE_POWER=0,
    MAPS_TYPE_REGEN,
    MAPS_TYPE_TV_REPARTITION,

    __NUM_OF_INPUT_MAPS_TYPE__
};

int8_t
steering_wheel_start(SteeringWheel_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t
steering_wheel_select_mission(SteeringWheel_h* const restrict self,
    const enum CAR_MISSIONS mission)__attribute__((__nonnull__(1)));

int8_t
steering_wheel_select_map(SteeringWheel_h* const restrict self,
    const enum MAPS_TYPE maps_type, const uint8_t value)__attribute__((__nonnull__(1)));

int8_t
steering_wheel_stop(SteeringWheel_h* const restrict self)__attribute__((__nonnull__(1)));

#endif // !__STEERING_WHEEL__
