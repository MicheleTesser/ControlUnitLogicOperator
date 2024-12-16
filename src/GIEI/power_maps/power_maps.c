#include "power_maps.h"
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
    float *active_map_power;
    float *active_map_pos_torque;
    float *active_map_neg_torque;
    float *active_map_regen;
    float *active_map_power_repartition;
}MAPS;

//public

int8_t giei_power_map_init(void)
{
    MAPS.active_map_power = &MAPS.power[0];
    MAPS.active_map_pos_torque = &MAPS.pos_torque[0];
    MAPS.active_map_neg_torque = &MAPS.neg_torque[0];
    MAPS.active_map_regen = &MAPS.regen[0];
    MAPS.active_map_power_repartition = &MAPS.power_repartition[0];

    return 0;
}

int8_t giei_set_run_map(const enum MAP_CATEGORY category, const uint8_t map_index)
{
    int8_t err=0;
    if (map_index > MAPS_NUMBER) {
        goto invalid_map_index;   
    }

    switch (category) {
        case POWER:
            MAPS.active_map_power = &MAPS.power[map_index];
            break;
        case POS_TORQUE:
            MAPS.active_map_neg_torque= &MAPS.pos_torque[map_index];
            break;
        case NEG_TORQUE:
            MAPS.active_map_neg_torque= &MAPS.neg_torque[map_index];
            break;
        case REGEN:
            MAPS.active_map_regen= &MAPS.regen[map_index];
            break;
        case POWER_REPARTITION:
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
        case POWER:
            return *MAPS.active_map_power;
        case POS_TORQUE:
            return *MAPS.active_map_pos_torque;
        case NEG_TORQUE:
            return *MAPS.active_map_neg_torque;
        case REGEN:
            return *MAPS.active_map_regen;
        case POWER_REPARTITION:
            return *MAPS.active_map_power_repartition;
    }
}
