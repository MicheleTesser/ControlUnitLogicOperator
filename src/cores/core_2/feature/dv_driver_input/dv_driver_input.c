#include "dv_driver_input.h"
#include "../../../../lib/raceup_board/raceup_board.h"
#include "../../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#include "../../../../lib/board_dbc/dbc/out_lib/can3/can3.h"
#include <stdint.h>
#include <string.h>

struct DvDriverInput_t{
    float brake;
    struct CanMailbox* human_brake_mailbox;
    struct CanMailbox* dv_brake_mailbox;
};

union DvDriverInput_h_t_conv{
    DvDriverInput_h* const restrict hidden;
    struct DvDriverInput_t* const restrict clear;
};

union DvDriverInput_h_t_conv_const{
    const DvDriverInput_h* const restrict hidden;
    const struct DvDriverInput_t* const restrict clear;
};

#ifdef DEBUG
char __assert_size_dv_driver_input[(sizeof(DvDriverInput_h) == sizeof(struct DvDriverInput_t))? 1:-1];
#endif // DEBUG

int8_t
dv_driver_input_init(DvDriverInput_h* const restrict self )
{
    union DvDriverInput_h_t_conv conv = {self};
    struct DvDriverInput_t* const restrict p_self = conv.clear;

    memset(p_self, 0, sizeof(*p_self));

    ACTION_ON_CAN_NODE(CAN_DV,{
        p_self->dv_brake_mailbox =
        hardware_get_mailbox_single_mex(can_node,RECV_MAILBOX, CAN_ID_DV_DRIVER,3);
    })
    if (!p_self->dv_brake_mailbox)
    {
        return -1;
    }

    ACTION_ON_CAN_NODE(CAN_GENERAL,{
        p_self->human_brake_mailbox =
          hardware_get_mailbox_single_mex(can_node, RECV_MAILBOX, CAN_ID_DRIVER, 4);
    })
    if (!p_self->human_brake_mailbox)
    {
        hardware_free_mailbox_can(&p_self->dv_brake_mailbox);
        return -1;
    }

    return 0;
}

int8_t
dv_driver_input_update(DvDriverInput_h* const restrict self )
{
    union DvDriverInput_h_t_conv conv = {self};
    struct DvDriverInput_t* const restrict p_self = conv.clear;
    can_obj_can2_h_t o2;
    can_obj_can3_h_t o3;
    CanMessage mex;
    float new_brake=0;

    if(!hardware_mailbox_read(p_self->dv_brake_mailbox, &mex))
    {
        unpack_message_can3(
                &o3,
                CAN_ID_DV_DRIVING_DYNAMICS_1,
                mex.full_word,
                8,
                timer_time_now());
    }

    if(!hardware_mailbox_read(p_self->human_brake_mailbox, &mex))
    {
        unpack_message_can2(&o2, CAN_ID_DRIVER, mex.full_word, 8, timer_time_now());
    }

    new_brake = o2.can_0x053_Driver.brake;
    if (o3.can_0x1f4_DV_driving_dynamics_1_time_stamp_rx == o2.can_0x053_Driver_time_stamp_rx &&
        o3.can_0x1f4_DV_driving_dynamics_1.Brake_hydr_actual > o2.can_0x053_Driver.brake)
    {
        new_brake = o3.can_0x1f4_DV_driving_dynamics_1.Brake_hydr_actual;
    }

    p_self->brake = new_brake;

    return 0;
}

float
dv_driver_input_get_brake(const DvDriverInput_h* const restrict self )
{
    union DvDriverInput_h_t_conv_const conv = {self};
    const struct DvDriverInput_t* const restrict p_self = conv.clear;

    return p_self->brake;
}
