#include "./raceup_board/raceup_board.h"
#include <sys/cdefs.h>

int8_t hardware_init_gpio(const BoardComponentId id __attribute_maybe_unused__)
{
    return 0;
}

int8_t gpio_set_pin_mode(const BoardComponentId id __attribute_maybe_unused__,
        uint8_t mode __attribute_maybe_unused__)
{
    return 0;
}
int8_t gpio_toggle(const BoardComponentId id __attribute_maybe_unused__)
{
    return 0;
}
int8_t gpio_read_state(const BoardComponentId id __attribute_maybe_unused__)
{
    return 0;
}
int8_t gpio_set_high(const BoardComponentId id __attribute_maybe_unused__)
{
    return 0;
}
int8_t gpio_set_low(const BoardComponentId id __attribute_maybe_unused__)
{
    return 0;
}
