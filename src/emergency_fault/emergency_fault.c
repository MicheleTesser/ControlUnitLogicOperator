#include "emergency_fault.h"
#include "../../lib/raceup_board/raceup_board.h"
#include "../board_conf/id_conf.h"
#include <stdint.h>

static uint8_t num_of_emergency =0;

int8_t one_emergency_raised(void)
{
    num_of_emergency++;
    gpio_set_low(SCS);

    return 0;
}
int8_t one_emergency_solved(void)
{
    if (num_of_emergency > 0) {
        num_of_emergency--;
    }
    if (!num_of_emergency) {
        gpio_set_high(SCS);
    }
    return 0;
}

int8_t is_emergency_state(void)
{
    return !!num_of_emergency;
}
