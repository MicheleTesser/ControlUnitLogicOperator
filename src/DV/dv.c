#include "dv.h"
#include "../lib/raceup_board/raceup_board.h"
#include "../board_conf/gpio/gpio.h"
#include <stdint.h>

//private

static struct {
    enum AS_STATUS status;
}DV;

//public

int8_t dv_class_init(void)
{
    DV.status = AS_OFF;
    return 0;
}

int8_t dv_set_status(const enum AS_STATUS status)
{
    DV.status = status;
    return 0;
}

int8_t dv_update_led(void)
{
    static time_var_microseconds driving_last_time_on =0;
    static time_var_microseconds emergency_last_time_on =0;
    static time_var_microseconds emergency_sound_last_time_on =0;
    static uint8_t sound_start = 0;

    if ((timer_time_now() - emergency_sound_last_time_on) > 8 SECONDS) {
        gpio_set_high(AS_EMERGENCY_SOUND);
        emergency_sound_last_time_on = timer_time_now();
    }

    switch (DV.status) {
        case AS_OFF:
            gpio_set_high(ASSI_LIGHT_BLU);
            gpio_set_high(ASSI_LIGHT_YELLOW);
            break;
        case AS_READY:
            gpio_set_high(ASSI_LIGHT_BLU);
            gpio_set_low(ASSI_LIGHT_YELLOW);
            break;
        case AS_DRIVING:
            gpio_set_high(ASSI_LIGHT_BLU);
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
            gpio_set_low(ASSI_LIGHT_BLU);
            gpio_set_high(ASSI_LIGHT_YELLOW);
            break;
    }
    return 0;
}
