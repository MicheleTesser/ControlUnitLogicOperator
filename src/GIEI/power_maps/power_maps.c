#include "power_maps.h"
#include "../../GIEI/giei.h"
#include "../../board_conf/id_conf.h"
#include "../../lib/raceup_board/raceup_board.h"
#include <stddef.h>
#include <stdint.h>

//private


#define MAPS_NUMBER 10
static struct{
    struct power_map_settings power[MAPS_NUMBER];
    struct regen_map_settings regen[MAPS_NUMBER];
    struct tv_repartition_map repartions[MAPS_NUMBER];
    union car_map active_maps;
}MAPS;

static void trap_change_map(void){
    GIEI_set_limits(POWER_LIMIT, MAPS.active_maps.power->power_kw);
    GIEI_set_limits(MAX_POS_TORQUE_LIMIT, MAPS.active_maps.power->torque_pos);
    GIEI_set_limits(MAX_NEG_TORQUE_LIMIT, MAPS.active_maps.regen->max_neg_torque);
    GIEI_set_limits(MOTOR_REPARTIION, MAPS.active_maps.repartition->repartition);
    GIEI_set_limits(TORQUE_VECTORING_ACTIVATION, MAPS.active_maps.repartition->torque_vectoring_on);
}

//public


int8_t giei_power_map_init(void)
{
    MAPS.active_maps.power= &MAPS.power[0];
    MAPS.active_maps.regen= &MAPS.regen[0];
    MAPS.active_maps.repartition= &MAPS.repartions[0];

    hardware_trap_attach_fun(TRAP_CHANGE_MAP, trap_change_map);

    return 0;
}

int8_t giei_set_run_map(const enum MAP_CATEGORY category, const uint8_t map_index)
{
    int8_t err=0;
    if (map_index > MAPS_NUMBER) {
        goto invalid_map_index;   
    }

    switch (category) {
        case MAP_POWER:
            MAPS.active_maps.power = &MAPS.power[map_index];
            break;
        case MAP_REGEN:
            MAPS.active_maps.regen = &MAPS.regen[map_index];
            break;
        case MAP_POWER_REPARTITION:
            MAPS.active_maps.repartition = &MAPS.repartions[map_index];
            break;
    }
    return 0;

invalid_map_index:
    err--;

    return err;
}

union car_map* giei_get_active_maps(void)
{
    return &MAPS.active_maps;
}
