#include <stdint.h>
#include "input_rtd.h"
#include "../../lib/raceup_board/raceup_board.h"
#include "../../board_conf/id_conf.h"
#include "../../emergency_fault/emergency_fault.h"
#include "../../DV/dv.h"


static struct{
    enum RTD_MODE mode;
    time_var_microseconds res_driving_start;
}RTD;

int8_t input_rtd_class_init(void)
{
    RTD.mode = BUTTON;
    return 0;
}

int8_t input_rtd_set_mode(const enum RTD_MODE mode)
{
    RTD.mode = mode;
    return 0;
}

int8_t input_rtd_check(void)
{
    switch (RTD.mode) {
        case DISABLE:
            if (gpio_read_state(READY_TO_DRIVE_INPUT_BUTTON) || dv_go()) {
                one_emergency_raised(RTD_IN_NONE_MISSION);
            }else{
                one_emergency_solved(RTD_IN_NONE_MISSION);
            }
            return 0;
        case BUTTON:
            return gpio_read_state(READY_TO_DRIVE_INPUT_BUTTON);
        case RES:
            return dv_go();
            break;
    }
    return -1;
}
