#include "giei.h"
#include "./amk/amk.h"
#include "../driver_input/driver_input.h"
#include "../board_conf/id_conf.h"
#include "../lib/raceup_board/raceup_board.h"

static struct{
    time_var_microseconds sound_start_at;
    uint8_t running;
}GIEI;

//private
static uint8_t GIEI_get_hv_status(void)
{
    return 0;
}

static uint8_t GIEI_get_rf_status(void)
{
    return 0;
}


static uint8_t GIEI_update(void)
{
    const time_var_microseconds sound_duration = 3 * 1000 * 1000;
    if ((timer_time_now() - GIEI.sound_start_at) > sound_duration) {
        gpio_set_high(READY_TO_DRIVE_OUT_SOUND);
    }

    if (GIEI.running) {
    
    }else{
        stop_engines();
    }

    return 0;
}

//public

uint8_t GIEI_check_running_condition(void)
{
    const uint8_t brake_treshold_percentage = 10;

    GIEI_update();
    if(!GIEI.running && (driver_get_amount(BRAKE) > brake_treshold_percentage) 
            && GIEI_get_hv_status() && GIEI_get_rf_status()){
        gpio_set_low(READY_TO_DRIVE_OUT_LED);
        GIEI.running =1;
        GIEI.sound_start_at = timer_time_now();
        gpio_set_low(READY_TO_DRIVE_OUT_LED);
        gpio_set_low(READY_TO_DRIVE_OUT_SOUND);
    }else if (GIEI.running) {
        if (!GIEI_get_hv_status() || !GIEI_get_rf_status()) {
            gpio_set_high(READY_TO_DRIVE_OUT_LED);
            GIEI.running =0;
            gpio_set_high(READY_TO_DRIVE_OUT_LED);
            gpio_set_high(READY_TO_DRIVE_OUT_SOUND);
        }
    }
    return GIEI.running;
}
