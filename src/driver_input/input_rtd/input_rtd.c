#include <stdint.h>
#include "input_rtd.h"
#include "../../lib/raceup_board/raceup_board.h"
#include "../../board_conf/id_conf.h"
#include "../../emergency_fault/emergency_fault.h"
#include "../../DV/res/res.h"
#include "../../DV/dv_status/dv_status.h"
#include "../../missions/missons.h"


static struct RTD{
    enum RTD_MODE mode;
    time_var_microseconds res_driving_start;

    struct DvStatus* p_dv_status;
    uint8_t init_done: 1;
}RTD_t;

int8_t dv_go(struct RTD* self)
{
    return (dv_status_get(self->p_dv_status) == AS_READY && res_check_go() && get_current_mission() > MANUALY) ||
        dv_status_set(self->p_dv_status, AS_DRIVING);
}

int8_t input_rtd_class_init(void)
{
    if (!RTD_t.init_done) {
        RTD_t.mode = DISABLE;
        RTD_t.p_dv_status = dv_status_class_get();
        RTD_t.init_done =1;
        // return *(int*) &RTD_t;
    }
    return 0;
}

int8_t input_rtd_set_mode(const enum RTD_MODE mode)
{
    RTD_t.mode = mode;
    return 0;
}

int8_t input_rtd_check(void)
{
    switch (RTD_t.mode) {
        case DISABLE:
            if (gpio_read_state(READY_TO_DRIVE_INPUT_BUTTON) || dv_go(&RTD_t)) {
                one_emergency_raised(RTD_IN_NONE_MISSION);
            }else{
                one_emergency_solved(RTD_IN_NONE_MISSION);
            }
            return 0;
        case BUTTON:
            return gpio_read_state(READY_TO_DRIVE_INPUT_BUTTON);
        case RES:
            return dv_go(&RTD_t);
            break;
    }
    return -1;
}
