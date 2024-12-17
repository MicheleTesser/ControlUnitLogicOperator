#ifndef __GIEI_POWER_MAPS__
#define __GIEI_POWER_MAPS__

#include <stdint.h>

enum MAP_CATEGORY {
    MAP_POWER =0,
    MAP_REGEN =3,
    MAP_POWER_REPARTITION =4,
};

struct power_map_settings{
    float power_kw;
    float torque_pos;
};

struct regen_map_settings{
    float regen_scale;
    float max_neg_torque;
};

struct tv_repartition_map{
    float repartition;
    uint8_t torque_vectoring_on;
};

union car_map{
    struct power_map_settings *power;
    struct regen_map_settings *regen;
    struct tv_repartition_map *repartition;
};

int8_t giei_power_map_init(void);
int8_t giei_set_run_map(const enum MAP_CATEGORY category, const uint8_t map_index);
const union car_map* giei_get_active_maps(void);

#endif // !__GIEI_POWER_MAPS__
