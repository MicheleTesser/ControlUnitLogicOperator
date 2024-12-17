#include "giei.h"
#include "./amk/amk.h"
#include "../driver_input/driver_input.h"
#include "../board_conf/id_conf.h"
#include "../lib/raceup_board/raceup_board.h"
#include "../emergency_fault/emergency_fault.h"
#include "engine_common.h"
#include <stdint.h>
#include <string.h>

#define SPEED_LIMIT                 18000   // Typical value: 15000
#define DEFAULT_MAX_POS_TORQUE              15.0f
#define DEFAULT_MAX_NEG_TORQUE              -15.f
#define DEFAULT_REGEN                       0
#define DEFAULT_REPARTITION                 0.5f
#define DEFAULT_POWER_LIMIT                 70000.0f       //Watt


static struct{
    time_var_microseconds sound_start_at;
    float limit_power;
    float limit_pos_torque;
    float limit_neg_torque;
    float limit_regen;
    float limit_max_speed;
    float settings_power_repartition;
    uint8_t activate_torque_vectoring :1;
    uint8_t running :1;
}GIEI;

//private
static uint8_t GIEI_get_hv_status(void)
{
    return 
        inverter_hv_status() &&
        gpio_read_state(AIR_PRECHARGE_INIT) && 
        gpio_read_state(AIR_PRECHARGE_DONE);
}

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
    GIEI.running =0;

    return 0;
}
int8_t GIEI_check_running_condition(void)
{
    const uint8_t brake_treshold_percentage = 10;
    const time_var_microseconds sound_duration = 3 * 1000 * 1000;

    if ((timer_time_now() - GIEI.sound_start_at) > sound_duration) {
        gpio_set_high(READY_TO_DRIVE_OUT_SOUND);
    }
    if (!GIEI.running && GIEI_get_hv_status()) 
    {
        //starting
        // printf("try starting brake amount: %f\n",driver_get_amount(BRAKE));
        if (driver_get_amount(BRAKE) > brake_treshold_percentage && 
                gpio_read_state(READY_TO_DRIVE_INPUT_BUTTON))
        {
            //starting ok
            if (gpio_read_state(AIR_PRECHARGE_INIT) && gpio_read_state(AIR_PRECHARGE_DONE)) 
            {
                gpio_set_low(READY_TO_DRIVE_OUT_LED);
                gpio_set_low(READY_TO_DRIVE_OUT_SOUND);
                GIEI.sound_start_at = timer_time_now();
                GIEI.running =1;
            //starting failed. Precharge not finished. opening scs to stop precharge
            }else if(!gpio_read_state(AIR_PRECHARGE_INIT) || !gpio_read_state(AIR_PRECHARGE_DONE)){
                one_emergency_raised();
            }
        //reset scs. can start again the precharge
        }else if(!GIEI_get_hv_status()){
            one_emergency_solved();
        }
    //exiting from R2D
    }else if (GIEI.running && (!GIEI_get_hv_status() || !gpio_read_state(READY_TO_DRIVE_INPUT_BUTTON))) 
    {
        GIEI.running =0;
        gpio_set_high(READY_TO_DRIVE_OUT_LED);
        for (uint8_t i=FRONT_LEFT; i<=REAR_RIGHT; i++) {
            stop_engine(i);
        }
    }

    //continue with what you were doing
    return GIEI.running;
}

int8_t GIEI_recv_data(const CanMessage* const restrict mex)
{
    update_status(mex);
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
