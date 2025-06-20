#ifndef __GIEI_MAPS__
#define __GIEI_MAPS__

#include <stdint.h>

typedef struct __attribute__((aligned(4))) DrivingMaps_h{
  const uint8_t private_data[256];
}DrivingMaps_h;

enum CAR_PARAMETERS{
    MAX_POS_TORQUE,
    MAX_NEG_TORQUE,
    TV_ON,
    TORQUE_REPARTITION,
    REGEN_SCALE,
    POWER_KW,
};

int8_t
driving_maps_init(DrivingMaps_h* const restrict self )__attribute__((__nonnull__(1)));

int8_t
driving_map_update(DrivingMaps_h* const restrict self )__attribute__((__nonnull__(1)));

float
driving_map_get_parameter(const DrivingMaps_h* const restrict self ,
        const enum CAR_PARAMETERS param)__attribute__((__nonnull__(1)));

#endif // !__GIEI_MAPS__
