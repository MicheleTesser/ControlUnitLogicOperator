#include "power_maps.h"
#include "../../GIEI/giei.h"
#include "../../board_conf/id_conf.h"
#include "../../lib/raceup_board/raceup_board.h"
#include <stddef.h>
#include <stdint.h>

//private

#define MAPS_NUMBER 10
static struct{
    float power[MAPS_NUMBER];
    float pos_torque[MAPS_NUMBER];
    float neg_torque[MAPS_NUMBER];
    float power_repartition[MAPS_NUMBER];
    float regen[MAPS_NUMBER];
    uint8_t activate_torque_vectoring;
    float *active_map_power;
    float *active_map_pos_torque;
    float *active_map_neg_torque;
    float *active_map_regen;
    float *active_map_power_repartition;
}MAPS;

static void trap_change_map(void){
    GIEI_set_limits(POWER_LIMIT, *MAPS.active_map_power);
    GIEI_set_limits(MAX_POS_TORQUE_LIMIT, *MAPS.active_map_pos_torque);
    GIEI_set_limits(MAX_NEG_TORQUE_LIMIT, *MAPS.active_map_neg_torque);
    GIEI_set_limits(MOTOR_REPARTIION, *MAPS.active_map_power_repartition);
    GIEI_set_limits(TORQUE_VECTORING_ACTIVATION, MAPS.activate_torque_vectoring);
}

//public


int8_t giei_power_map_init(void)
{
    MAPS.active_map_power = &MAPS.power[0];
    MAPS.active_map_pos_torque = &MAPS.pos_torque[0];
    MAPS.active_map_neg_torque = &MAPS.neg_torque[0];
    MAPS.active_map_regen = &MAPS.regen[0];
    MAPS.active_map_power_repartition = &MAPS.power_repartition[0];
    MAPS.activate_torque_vectoring = 0;

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
            MAPS.active_map_power = &MAPS.power[map_index];
            break;
        case MAP_POS_TORQUE:
            MAPS.active_map_neg_torque= &MAPS.pos_torque[map_index];
            break;
        case MAP_NEG_TORQUE:
            MAPS.active_map_neg_torque= &MAPS.neg_torque[map_index];
            break;
        case MAP_REGEN:
            MAPS.active_map_regen= &MAPS.regen[map_index];
            break;
        case MAP_POWER_REPARTITION:
            MAPS.active_map_power_repartition= &MAPS.power_repartition[map_index];
            break;
    }
    return 0;

invalid_map_index:
    err--;

    return err;
}

float giei_get_active_map(const enum MAP_CATEGORY category)
{
    switch (category) {
        case MAP_POWER:
            return *MAPS.active_map_power;
        case MAP_POS_TORQUE:
            return *MAPS.active_map_pos_torque;
        case MAP_NEG_TORQUE:
            return *MAPS.active_map_neg_torque;
        case MAP_REGEN:
            return *MAPS.active_map_regen;
        case MAP_POWER_REPARTITION:
            return *MAPS.active_map_power_repartition;
    }
}

void giei_toggle_torque_vectoring(void){
    MAPS.activate_torque_vectoring = !MAPS.activate_torque_vectoring;
}
