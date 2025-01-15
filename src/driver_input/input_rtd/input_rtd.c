#include <stdint.h>
#include "input_rtd.h"
#include "../../lib/raceup_board/raceup_board.h"
#include "../../board_conf/id_conf.h"
#include "../../emergency_fault/emergency_fault.h"
#include "../../missions/missons.h"
#include "../../car_status/car_status.h"


static struct RTD{
    enum RTD_MODE mode;
    time_var_microseconds res_driving_start;

    struct DvStatus* p_dv_status;
    uint8_t init_done: 1;
}RTD_t;

int8_t dv_go(void)
{
   
    const uint8_t ready_to_go = (
            car_status_get_info(CAR_STATUS_DV_STATUS_READY) && 
            car_status_get_info(CAR_STATUS_RES_READY_GO));
    const uint8_t already_driving = 
        car_status_get_info(CAR_STATUS_DV_STATUS_DRIVING);

    return ready_to_go || already_driving;
}

int8_t input_rtd_class_init(void)
{
    if (!RTD_t.init_done) {
        RTD_t.mode = DISABLE;
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
            if (gpio_read_state(READY_TO_DRIVE_INPUT_BUTTON) || dv_go()) {
                EMERGENCY_FAULT_MUT_ACTION({
                    one_emergency_raised(emergency_mut_ptr, RTD_IN_NONE_MISSION);
                })
            }else{
                EMERGENCY_FAULT_MUT_ACTION({
                    one_emergency_solved(emergency_mut_ptr, RTD_IN_NONE_MISSION);
                })
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
