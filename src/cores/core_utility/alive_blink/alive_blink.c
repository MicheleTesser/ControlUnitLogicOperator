#include "alive_blink.h"
#include "../../../../lib/raceup_board/raceup_board.h"
#include <string.h>

struct CoreAliveBlink_t{
    enum GPIO_PIN pin_led;
    time_var_microseconds last_time_toggle;
    time_var_microseconds freq;
};

union CoreAliveBlink_h_t_conv{
    CoreAliveBlink_h* const restrict hidden;
    struct CoreAliveBlink_t* const restrict clear;
};

int8_t
core_alive_blink_init(
        CoreAliveBlink_h* const restrict self __attribute__((__nonnull__)),
        enum GPIO_PIN pin_led, const time_var_microseconds freq)
{
    union CoreAliveBlink_h_t_conv conv = {self};
    struct CoreAliveBlink_t* const restrict p_self = conv.clear;

    memset(p_self, 0, sizeof(*p_self));
    p_self->pin_led = pin_led;
    p_self->last_time_toggle = timer_time_now();

    return 0;
}

int8_t
core_alive_blink_update(CoreAliveBlink_h* const restrict self __attribute__((__nonnull__)))
{
    union CoreAliveBlink_h_t_conv conv = {self};
    struct CoreAliveBlink_t* const restrict p_self = conv.clear;
    const time_var_microseconds curr_time = timer_time_now();

    if ((p_self->last_time_toggle - curr_time) > p_self->freq ) {
        p_self->last_time_toggle = curr_time;
        gpio_toggle(p_self->pin_led);
    }

    return 0;
}
