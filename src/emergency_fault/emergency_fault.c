#include "emergency_fault.h"
#include "../../lib/raceup_board/raceup_board.h"
#include "../board_conf/id_conf.h"
#include "../missions/missons.h"
#include "../DV/dv.h"
#include <stdint.h>

static struct{
    uint32_t num_of_emergency[2];
}EMERGENCYS;


int8_t one_emergency_raised(const enum EMERGENCY_FAULT id)
{
    switch (id) {

        case FAILED_RTD_SEQ:
        case ENGINE_FAULT:
        case DV_EMERGENCY_STATE:
        case RTD_IN_NONE_MISSION:
            EMERGENCYS.num_of_emergency[0] |=  id;
            break;
        default:
            return -1;
    }
    gpio_set_low(SCS);
    if (get_current_mission() > MANUALY) {
        dv_set_status(AS_EMERGENCY);
    }

    return 0;
}

int8_t one_emergency_solved(const enum EMERGENCY_FAULT id)
{
    switch (id) {

        case FAILED_RTD_SEQ:
        case ENGINE_FAULT:
        case DV_EMERGENCY_STATE:
        case RTD_IN_NONE_MISSION:
            if (EMERGENCYS.num_of_emergency[0] & id) {
                EMERGENCYS.num_of_emergency[0]^= id;
            }
            break;
        default:
            return -1;
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
