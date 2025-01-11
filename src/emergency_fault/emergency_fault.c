#include "emergency_fault.h"
#include "../../lib/raceup_board/raceup_board.h"
#include "../board_conf/id_conf.h"
#include "../missions/missons.h"
#include "../DV/dv.h"
#include <stdint.h>

//private

static struct{
    uint8_t num_of_emergency[__NUM_OF_EMERGENCY_FAULTS/8 + !!(__NUM_OF_EMERGENCY_FAULTS % 8)];
}EMERGENCYS;

struct ErrorIndexArray {
    uint8_t emergency_buffer;
    uint8_t emergencY_bit;
};

static void 
get_error_index(const enum EMERGENCY_FAULT fault, struct ErrorIndexArray* const restrict o_buffer)
{
    o_buffer->emergency_buffer = fault/8;
    o_buffer->emergencY_bit = fault%8;
}

//public

int8_t one_emergency_raised(const enum EMERGENCY_FAULT id)
{
    if (id == __NUM_OF_EMERGENCY_FAULTS) {
        return  -1;
    }
    struct ErrorIndexArray index;
    get_error_index(id, &index);
    EMERGENCYS.num_of_emergency[index.emergency_buffer] |= index.emergencY_bit;
    gpio_set_low(SCS);
    if (get_current_mission() > MANUALY) {
        dv_set_status(AS_EMERGENCY);
    }

    return 0;
}

int8_t one_emergency_solved(const enum EMERGENCY_FAULT id)
{
    if (id == __NUM_OF_EMERGENCY_FAULTS) {
        return  -1;
    }
    struct ErrorIndexArray index;
    get_error_index(id, &index);
    if (EMERGENCYS.num_of_emergency[index.emergency_buffer] & index.emergencY_bit) {
        EMERGENCYS.num_of_emergency[index.emergency_buffer]^= index.emergencY_bit;
    }

    if (!EMERGENCYS.num_of_emergency[0] && !EMERGENCYS.num_of_emergency[1]) {
        gpio_set_high(SCS);
    }
    return 0;
}

int8_t is_emergency_state(void)
{
    return !!(EMERGENCYS.num_of_emergency[0] || EMERGENCYS.num_of_emergency[1]);
}
