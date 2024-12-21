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
    struct car_map active_maps;
}MAPS;

static void trap_change_map(void) TRAP_ATTRIBUTE
{
    GIEI_set_limits(POWER_LIMIT, MAPS.active_maps.power->power_kw);
    GIEI_set_limits(MAX_POS_TORQUE_LIMIT, MAPS.active_maps.power->torque_pos);
    GIEI_set_limits(MAX_NEG_TORQUE_LIMIT, MAPS.active_maps.regen->max_neg_torque);
    GIEI_set_limits(MOTOR_REPARTIION, MAPS.active_maps.repartition->repartition);
    GIEI_set_limits(TORQUE_VECTORING_ACTIVATION, MAPS.active_maps.repartition->torque_vectoring_on);
}

static inline void set_pow_map(const uint8_t map_i, const float kw, const float torque)
{
    struct power_map_settings* map = &MAPS.power[map_i];
    map->power_kw=kw;
    map->torque_pos=torque;
}

static inline void set_regen_map(const uint8_t map_i, const float regen_scale, const float torque)
{
    struct regen_map_settings* map = &MAPS.regen[map_i];
    map->regen_scale = regen_scale;
    map->max_neg_torque = torque;
}

static inline void set_repartition_map(const uint8_t map_i, const float repartition, const uint8_t tv)
{
    struct tv_repartition_map* map = &MAPS.repartions[map_i];
    map->repartition = repartition;
    map->torque_vectoring_on = tv;
}

static void init_power_maps(void)
{
    set_pow_map(0, 77, 21);
    set_pow_map(1, 75, 20);
    set_pow_map(2, 70, 18);
    set_pow_map(3, 60, 16);
    set_pow_map(4, 50, 15);
    set_pow_map(5, 40, 15);
    set_pow_map(6, 35, 13);
    set_pow_map(7, 30, 13);
    set_pow_map(8, 15, 12);
    set_pow_map(9, 10, 10);
}

static void init_regen_maps(void)
{
    set_regen_map(0, 0, 0);
    set_regen_map(1, 20, -8);
    set_regen_map(2, 30, -10);
    set_regen_map(3, 40, -12);
    set_regen_map(4, 50, -15);
    set_regen_map(5, 60, -17);
    set_regen_map(6, 70, -18);
    set_regen_map(7, 80, -19);
    set_regen_map(8, 90, -20);
    set_regen_map(9, 100, -21);
}

static void init_repartition_maps(void)
{
    set_repartition_map(0, 50, 1);
    set_repartition_map(1, 100, 0);
    set_repartition_map(2, 82, 18);
    set_repartition_map(3, 80, 20);
    set_repartition_map(4, 78, 22);
    set_repartition_map(5, 75, 25);
    set_repartition_map(6, 70, 30);
    set_repartition_map(7, 60, 40);
    set_repartition_map(8, 50, 50);
    set_repartition_map(9, 50, 50);
}

//public


int8_t giei_power_map_init(void)
{
    init_power_maps();
    init_regen_maps();
    init_repartition_maps();
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

const struct car_map* giei_get_active_maps(void)
{
    return &MAPS.active_maps;
}
