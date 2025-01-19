#ifndef __GIEI_MAPS__
#define __GIEI_MAPS__

#include <stdint.h>

typedef struct DrivingMaps_h{
    const uint8_t private_data[1];
}DrivingMaps_h;

enum MAPS_TYPE{
    MAPS_TYPE_POWER,
    MAPS_TYPE_REGEN,
    MAPS_TYPE_TV_REPARTITION,
};

enum CAR_PARAMETERS{
    MAX_POS_TORQUE,
    MAX_NEG_TORQUE,
    TV_ON,
    TORQUE_REPARTITION,
    REGEN_SCALE,
    POWER_KW,
};

int8_t
driving_maps_init(DrivingMaps_h* const restrict self __attribute__((__nonnull__)));

float
driving_map_get_parameter(const DrivingMaps_h* const restrict self __attribute__((__nonnull__)),
        const enum CAR_PARAMETERS param);

int8_t
driving_map_change(DrivingMaps_h* const restrict self,
        const enum MAPS_TYPE map_type,
        const uint8_t map);


#endif // !__GIEI_MAPS__
