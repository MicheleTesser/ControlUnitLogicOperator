#include "car_status.h"
#include "../GIEI/giei.h"
#include "../lib/board_dbc/dbc/out_lib/can2/can2.h"
#include "../board_can/board_can.h"
#include "../DV/dv_status/dv_status.h"
#include "../DV/res/res.h"
#include "../board_conf/id_conf.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>

static int8_t send_can_settings_message(const struct GIEI *giei)
{
    can_obj_can2_h_t o;
    memset(&o, 0, sizeof(o));
    CanMessage mex;
    o.can_0x066_CarSettings.pwr_limit = GIEI_get_info(giei, GIEI_INFO_LIMIT_POWER);
    o.can_0x066_CarSettings.speed_lim = GIEI_get_info(giei, GIEI_INFO_MAX_SPEED);
    o.can_0x066_CarSettings.max_neg_trq = GIEI_get_info(giei, GIEI_INFO_MAX_POS_TORQUE);
    o.can_0x066_CarSettings.max_neg_trq = GIEI_get_info(giei, GIEI_INFO_MAX_NEG_TORQUE);
    o.can_0x066_CarSettings.torque_vectoring = GIEI_get_info(giei, GIEI_INFO_TV);
    o.can_0x066_CarSettings.max_regen_current = GIEI_get_info(giei, GIEI_INFO_LIMIT_REGEN);
    o.can_0x066_CarSettings.rear_motor_repartition = GIEI_get_info(giei, GIEI_INFO_REAR_REPARTITION);
    o.can_0x066_CarSettings.front_motor_repartition = GIEI_get_info(giei, GIEI_INFO_FRONT_REPARTITION);
    mex.id = CAN_ID_CARSETTINGS;
    mex.message_size = pack_message_can2(&o, CAN_ID_CARSETTINGS, &mex.full_word);

    return board_can_write(CAN_MODULE_GENERAL, &mex);
}

static int8_t send_can_status_message(const struct GIEI* const restrict giei)
{
    can_obj_can2_h_t o;
    memset(&o, 0, sizeof(o));
    CanMessage mex;
    o.can_0x065_CarStatus.HV = GIEI_get_info(giei, GIEI_INFO_STATUS_HV);
    o.can_0x065_CarStatus.RF = GIEI_get_info(giei, GIEI_INFO_STATUS_RF);
    o.can_0x065_CarStatus.R2D = GIEI_check_running_condition() == RUNNING;
    o.can_0x065_CarStatus.AIR1 = gpio_read_state(AIR_PRECHARGE_INIT);
    o.can_0x065_CarStatus.AIR2 = gpio_read_state(AIR_PRECHARGE_DONE);
    o.can_0x065_CarStatus.precharge = 
        gpio_read_state(AIR_PRECHARGE_INIT) && gpio_read_state(AIR_PRECHARGE_DONE);
    o.can_0x065_CarStatus.speed = GIEI_get_info(giei, GIEI_INFO_CURRENT_SPEED);
    mex.id = CAN_ID_CARSTATUS;
    mex.message_size = pack_message_can2(&o, CAN_ID_CARSTATUS, &mex.full_word);

    return board_can_write(CAN_MODULE_GENERAL, &mex);
}

int8_t car_status_send_status(void)
{
    const struct GIEI* p_giei = GIEI_get();
    return send_can_status_message(p_giei) || send_can_settings_message(p_giei);
}

int8_t car_status_get_info(const enum CarStatusInfo info)
{
    switch (info) {
        case CAR_STATUS_DV_STATUS_READY:
            {
                const struct DvStatus* p_dv_status = dv_status_class_get();
                return dv_status_get(p_dv_status) == AS_READY;
            }
        case CAR_STATUS_DV_STATUS_DRIVING:
            {
                const struct DvStatus* p_dv_status = dv_status_class_get();
                return dv_status_get(p_dv_status) == AS_DRIVING;
            }
        case CAR_STATUS_RES_READY_GO:
            {
                const struct DvRes* p_res = res_class_get();
                return res_check_go(p_res);
            }
            break;
    }
    return -1;
}
