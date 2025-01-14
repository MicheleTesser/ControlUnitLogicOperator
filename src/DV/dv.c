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

static struct {
    struct DvStatus* p_dv_status;
}DV;

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

    switch (dv_status_get(DV.p_dv_status)) {
        case AS_OFF:
            gpio_set_high(ASSI_LIGHT_BLU);
            gpio_set_high(ASSI_LIGHT_YELLOW);
            gpio_set_high(AS_EMERGENCY_SOUND);
            break;
        case AS_READY:
            gpio_set_high(ASSI_LIGHT_BLU);
            gpio_set_low(ASSI_LIGHT_YELLOW);
            gpio_set_high(AS_EMERGENCY_SOUND);
            res_start_time();
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
    return 0;
}

//INFO: Flowchart T 14.9.2
static int8_t dv_update_status(void)
{
    if (is_emergency_state()) {
        dv_status_set(DV.p_dv_status, AS_EMERGENCY);
    }
    const enum RUNNING_STATUS giei_status = GIEI_check_running_condition();
    if (!ebs_on()) 
    {
        if (get_current_mission() > MANUALY &&  asb_consistency_check() && giei_status >= TS_READY)
        {
            if (giei_status == RUNNING)
            {
                dv_status_set(DV.p_dv_status, AS_DRIVING);
            }
            else if(driver_get_amount(BRAKE) > 50)
            {
                dv_status_set(DV.p_dv_status, AS_READY);
            }
            else
            {
                dv_status_set(DV.p_dv_status, AS_OFF);
            }
        }
        else
        {
            dv_status_set(DV.p_dv_status, AS_OFF);
        }
    }
    else if (mission_status() == MISSION_FINISHED && !GIEI_get_speed() && sdc_closed())
    {
        dv_status_set(DV.p_dv_status, AS_FINISHED);
    }
    else
    {
        dv_status_set(DV.p_dv_status, AS_EMERGENCY);
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
    DV.p_dv_status = dv_status_class_init();

    return 0;
}

int8_t dv_compute(void)
{
    dv_update_status();
    dv_update_led();
    if (dv_status_get(DV.p_dv_status) == AS_DRIVING && get_current_mission() > MANUALY) {
        dv_stw_alg_compute(0, 0); //TODO: not yet implemented
        return 0;
    }
    return -1;
}
