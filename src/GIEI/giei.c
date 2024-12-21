#include "giei.h"
#include "./amk/amk.h"
#include "../driver_input/driver_input.h"
#include "../board_conf/id_conf.h"
#include "../lib/raceup_board/raceup_board.h"
#include "../emergency_fault/emergency_fault.h"
#include "engine_common.h"
#include "power_maps/power_maps.h"
#include <stdint.h>


//private

#define SPEED_LIMIT                         18000           //INFO: Typical value: 15000
#define DEFAULT_MAX_POS_TORQUE              15.0f
#define DEFAULT_MAX_NEG_TORQUE              -15.f
#define DEFAULT_REGEN                       0
#define DEFAULT_REPARTITION                 0.5f
#define DEFAULT_POWER_LIMIT                 70000.0f       //INFO: Watt


static struct __GIEI{
    time_var_microseconds sound_start_at;
    float limit_power;
    float limit_pos_torque;
    float limit_neg_torque;
    float limit_regen;
    float limit_max_speed;
    float settings_power_repartition;

    uint8_t activate_torque_vectoring :1;
}GIEI;


//public

int8_t GIEI_initialize(void)
{
    GIEI.limit_power = DEFAULT_POWER_LIMIT;
    GIEI.limit_pos_torque = DEFAULT_MAX_POS_TORQUE;
    GIEI.limit_neg_torque = DEFAULT_MAX_NEG_TORQUE;
    GIEI.limit_regen = DEFAULT_REGEN;
    GIEI.limit_max_speed = SPEED_LIMIT;
    GIEI.settings_power_repartition = DEFAULT_REPARTITION;
    GIEI.activate_torque_vectoring = 0;

    amk_module_init();
    giei_power_map_init();

    return 0;
}

enum RUNNING_STATUS GIEI_check_running_condition(void)
{
    static uint8_t rtd_done = 0;
    const time_var_microseconds sound_duration = 3 SECONDS;
    enum RUNNING_STATUS rt = SYSTEM_OFF;

    if ((timer_time_now() - GIEI.sound_start_at) > sound_duration) {
        gpio_set_high(READY_TO_DRIVE_OUT_SOUND);
        gpio_set_high(READY_TO_DRIVE_OUT_LED);
    }
    rt = amk_rtd_procedure();
    if (rt == RUNNING && !rtd_done) {
        rtd_done =1;
        gpio_set_low(READY_TO_DRIVE_OUT_SOUND);
        gpio_set_low(READY_TO_DRIVE_OUT_LED);
        GIEI.sound_start_at = timer_time_now();
    }else if (rt != RUNNING) {
        rtd_done =0;
        gpio_set_high(READY_TO_DRIVE_OUT_SOUND);
        gpio_set_high(READY_TO_DRIVE_OUT_LED);
    }
    return rt;
}

int8_t GIEI_recv_data(const CanMessage* const restrict mex)
{
    amk_update_status(mex);
    return 0;
}

int8_t GIEI_set_limits(const enum GIEI_LIMITS category, const float value)
{
    switch (category) {
        case POWER_LIMIT:
            GIEI.limit_power = value;
            break;
        case MAX_POS_TORQUE_LIMIT:
            GIEI.limit_pos_torque = value;
            break;
        case MAX_NEG_TORQUE_LIMIT:
            GIEI.limit_neg_torque= value;
            break;
        case MOTOR_REPARTIION:
            GIEI.settings_power_repartition = value;
            break;
        case TORQUE_VECTORING_ACTIVATION:
            GIEI.activate_torque_vectoring = value;
            break;
    }
    return 0;
}

int8_t GIEI_input(const float throttle, const float brake, const float regen)
{
    return 0;
}


//debug

uint8_t DEBUG_GIEI_check_limits(float power_limit, float pos_torque, 
        float neg_torque, float repartition, float tv_on)
{
    return 
        GIEI.limit_power = power_limit &&
        GIEI.limit_pos_torque == pos_torque &&
        GIEI.limit_neg_torque == neg_torque &&
        GIEI.settings_power_repartition == repartition &&
        GIEI.activate_torque_vectoring == tv_on;
}
