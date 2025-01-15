#include "dv.h"
#include "../lib/raceup_board/raceup_board.h"
#include "../board_conf/gpio/gpio.h"
#include "../missions/missons.h"
#include "asb/asb.h"
#include "../GIEI/giei.h"
#include "../driver_input/driver_input.h"
#include "../emergency_fault/emergency_fault.h"
#include "asms/asms.h"
#include "dv_status/dv_status.h"
#include "res/res.h"
#include "steering_wheel_alg/stw_alg.h"
#include <stdint.h>

//private

static uint8_t sdc_closed(void)
{
    return  gpio_read_state(AIR_PRECHARGE_INIT) &&
            gpio_read_state(AIR_PRECHARGE_DONE) &&
            !is_emergency_state();
}

static int8_t dv_update_led(void)
{
    static time_var_microseconds driving_last_time_on =0;
    static time_var_microseconds emergency_last_time_on =0;
    static time_var_microseconds emergency_sound_last_time_on =0;
    static uint8_t sound_start = 0;

    if ((timer_time_now() - emergency_sound_last_time_on) > 8 SECONDS) {
        gpio_set_high(AS_EMERGENCY_SOUND);
        emergency_sound_last_time_on = timer_time_now();
    }

    RES_MUT_ACTION({
            DV_STATUS_READ_ONLY_ACTION(
                    switch (dv_status_get(dv_status_read_ptr)) {
                    case AS_OFF:
                    gpio_set_high(ASSI_LIGHT_BLU);
                    gpio_set_high(ASSI_LIGHT_YELLOW);
                    gpio_set_high(AS_EMERGENCY_SOUND);
                    break;
                    case AS_READY:
                    gpio_set_high(ASSI_LIGHT_BLU);
                    gpio_set_low(ASSI_LIGHT_YELLOW);
                    gpio_set_high(AS_EMERGENCY_SOUND);
                    res_start_time(res_mut_ptr);
                    break;
                    case AS_DRIVING:
                    gpio_set_high(ASSI_LIGHT_BLU);
                    gpio_set_high(AS_EMERGENCY_SOUND);
                    if ((timer_time_now() - driving_last_time_on) > 100 MILLIS ) {
                    gpio_toggle(ASSI_LIGHT_YELLOW);
                    driving_last_time_on = timer_time_now();
                    }
                    break;
                    case AS_EMERGENCY:
                    gpio_set_high(ASSI_LIGHT_YELLOW);
                    if ((timer_time_now() - emergency_last_time_on) > 100 MILLIS ) {
                        gpio_toggle(ASSI_LIGHT_BLU);
                        emergency_last_time_on = timer_time_now();
                    }
                    if (!sound_start) {
                        sound_start =1;
                        gpio_set_low(AS_EMERGENCY_SOUND);
                        emergency_sound_last_time_on = timer_time_now();
                    }
                    break;
                    case AS_FINISHED:
                    gpio_set_high(AS_EMERGENCY_SOUND);
                    gpio_set_high(ASSI_LIGHT_YELLOW);
                    gpio_set_low(ASSI_LIGHT_BLU);
                    break;
                    }
            )
    })


    return 0;
}

//INFO: Flowchart T 14.9.2
static int8_t dv_update_status(void)
{
    float giei_speed = 0;
    float driver_brake =0;

    if (is_emergency_state()) {
        DV_STATUS_MUT_ACTION({
            dv_status_set(dv_status_mut_ptr, AS_EMERGENCY);
        })
    }
    const enum RUNNING_STATUS giei_status = GIEI_check_running_condition();

    GIEI_READ_ONLY_ACTION({
        giei_speed = GIEI_get_info(giei_read_ptr, GIEI_INFO_CURRENT_SPEED);
    });

    DRIVER_INPUT_READ_ONLY_ACTION({
        driver_brake = driver_get_amount(driver_input_read_ptr, BRAKE);
    })


    if (!ebs_on()) 
    {
        if (get_current_mission() > MANUALY &&  asb_consistency_check() && giei_status >= TS_READY)
        {
            if (giei_status == RUNNING)
            {
                DV_STATUS_MUT_ACTION({
                    dv_status_set(dv_status_mut_ptr, AS_DRIVING);
                })
            }
            else if(driver_brake > 50)
            {
                DV_STATUS_MUT_ACTION({
                    dv_status_set(dv_status_mut_ptr, AS_READY);
                })
            }
            else
            {
                DV_STATUS_MUT_ACTION({
                    dv_status_set(dv_status_mut_ptr, AS_OFF);
                })
            }
        }
        else
        {
            DV_STATUS_MUT_ACTION({
                dv_status_set(dv_status_mut_ptr, AS_OFF);
            })
        }
    }

    
    else if (mission_status() == MISSION_FINISHED && !giei_speed && sdc_closed())
    {
        DV_STATUS_MUT_ACTION({
            dv_status_set(dv_status_mut_ptr, AS_FINISHED);
        })
    }
    else
    {
        DV_STATUS_MUT_ACTION({
            dv_status_set(dv_status_mut_ptr, AS_EMERGENCY);
        })
    }
    return 0;
}

//public

int8_t dv_class_init(void)
{
    asms_class_init();
    asb_class_init();
    res_class_init();
    dv_stw_alg_init();
    dv_status_class_init();

    return 0;
}

int8_t dv_compute(void)
{
    dv_update_status();
    dv_update_led();
    DV_STATUS_READ_ONLY_ACTION({
        if (dv_status_get(dv_status_read_ptr) == AS_DRIVING && get_current_mission() > MANUALY)
        {
            dv_stw_alg_compute(0, 0); //TODO: not yet implemented
            return 0;
        }
    })
    return -1;
}
