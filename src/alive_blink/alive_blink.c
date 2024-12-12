#include "alive_blink.h"
#include "../lib/raceup_board/raceup_board.h"
#include "../board_conf/id_conf.h"
#include <stdint.h>

#define SEC
#define MILLIS
#define MICROS

static struct{
    time_var_microseconds time_last_led_1;
    time_var_microseconds frequency_led;
    uint8_t pin_blink;
}timers[3];
static uint8_t next =0;


alive_blink_fd i_m_alive_init(
        const time_var_microseconds curr_time, 
        const time_var_microseconds frequency,
        const uint8_t pin_blink)
{
    
    timers[next].time_last_led_1 = curr_time;
    timers[next].frequency_led = frequency;
    timers[next].pin_blink = pin_blink;

    return next++;
}

void i_m_alive(alive_blink_fd id)
{
    time_var_microseconds * const time_last_led_1 = &timers[id].time_last_led_1;
    const time_var_microseconds * const frequency_led = &timers[id].frequency_led;
    const uint8_t pin_blink = timers[id].pin_blink;

    time_var_microseconds curr_time = timer_time_now();
    if (is_time_passed(*time_last_led_1, curr_time, *frequency_led)) {
        gpio_toggle(pin_blink);
        *time_last_led_1 = curr_time;
    }
}
