#include "emergency_fault.h"
#include "../../lib/raceup_board/raceup_board.h"
#include "../board_conf/id_conf.h"
#include <stdint.h>

static uint32_t num_of_emergency =0;


int8_t one_emergency_raised(const enum EMERGENCY_FAULT id)
{
    num_of_emergency|=  id;
    gpio_set_low(SCS);

    return 0;
}

int8_t one_emergency_solved(const enum EMERGENCY_FAULT id)
{
    if (num_of_emergency & id) {
        num_of_emergency^= id;
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
