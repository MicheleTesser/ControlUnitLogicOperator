#ifndef __GIEI_POWER_MAPS__
#define __GIEI_POWER_MAPS__

#include <stdint.h>

enum MAP_CATEGORY {
    POWER =0,
    POS_TORQUE =1,
    NEG_TORQUE =2,
    REGEN =3,
    POWER_REPARTITION =4,
};

int8_t giei_power_map_init(void);
int8_t giei_set_run_map(const enum MAP_CATEGORY category, const uint8_t map_index);
float giei_get_active_map(const enum MAP_CATEGORY category);

#endif // !__GIEI_POWER_MAPS__
