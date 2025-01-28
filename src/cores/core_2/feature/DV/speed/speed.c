#include "speed.h"
#include "imu/imu.h"
#include "../../../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#include <stdint.h>
#include <string.h>


struct DvSpeed_t{
    DvImu_h imu;
};

union DvSpeed_h_t_conv{
    DvSpeed_h* const restrict hidden;
    struct DvSpeed_t* const restrict clear;
};

int8_t
dv_speed_init(DvSpeed_h* const restrict self )
{
    int8_t err=0;
    union DvSpeed_h_t_conv conv = {self};
    struct DvSpeed_t* const restrict p_self = conv.clear;

    memset(p_self, 0, sizeof(*p_self));
    struct CanMailbox* dv_imu_mailbox = NULL;
    // ACTION_ON_CAN_NODE(CAN_GENERAL,{
    //     dv_imu_mailbox = hardware_get_mailbox(can_node,,); //TODO: not yet defined
    // });
    if (!dv_imu_mailbox) {
        goto mailbox_fail;
    }
    if(dv_imu_init(&p_self->imu, dv_imu_mailbox) <0){
        goto imu_fail;
    }

    return 0;

mailbox_fail:
    err--;
imu_fail:
    err--;
    hardware_free_mailbox_can(&dv_imu_mailbox);

    return err;
}

int8_t
dv_speed_update(DvSpeed_h* const restrict self )
{
    union DvSpeed_h_t_conv conv = {self};
    struct DvSpeed_t* const restrict p_self = conv.clear;
    return dv_imu_update(&p_self->imu);
}

float
dv_speed_get(DvSpeed_h* const restrict self __attribute__((__unused__)))
{
  return 0;
}
