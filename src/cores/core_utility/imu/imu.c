#include "imu.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "../../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#pragma GCC diagnostic pop
#include "../../../../lib/raceup_board/raceup_board.h"
#include <string.h>

struct Imu_t{
    float acc[__NUM_OF_AXIS__];
    float omega[__NUM_OF_AXIS__];
    struct CanMailbox* mailbox_imu_1;
    struct CanMailbox* mailbox_imu_2;
    struct CanMailbox* mailbox_imu_3;
};

union Imu_h_t_conv{
    Imu_h* const restrict hidden;
    struct Imu_t* const restrict clear;
};

union Imu_h_t_conv_const{
    const Imu_h* const hidden;
    const struct Imu_t* const clear;
};

#ifdef DEBUG
char __assert_size_core_0_imu[(sizeof(Imu_h)) == sizeof(struct Imu_t)? 1:-1];
char __assert_align_core_0_imu[(_Alignof(Imu_h)) == _Alignof(struct Imu_t)? 1:-1];
#endif // DEBUG

//public

int8_t imu_init(Imu_h* const restrict self)
{
    union Imu_h_t_conv conv = {self};
    struct Imu_t* const restrict p_self = conv.clear;
    struct CanNode* can_node = NULL;

    memset(p_self, 0, sizeof(*p_self));

    ACTION_ON_CAN_NODE(CAN_GENERAL, can_node)
    {
      p_self->mailbox_imu_1 =
        hardware_get_mailbox_single_mex(
            can_node,
            RECV_MAILBOX,
            CAN_ID_IMU1,
            message_dlc_can2(CAN_ID_IMU1));
    }

    if (!p_self->mailbox_imu_1)
    {
      return -1;
    }

    ACTION_ON_CAN_NODE(CAN_GENERAL, can_node)
    {
      p_self->mailbox_imu_2 =
        hardware_get_mailbox_single_mex(
            can_node,
            RECV_MAILBOX,
            CAN_ID_IMU2,
            message_dlc_can2(CAN_ID_IMU1));
    }

    if (!p_self->mailbox_imu_2)
    {
      hardware_free_mailbox_can(&p_self->mailbox_imu_1);
      return -2;
    }

    ACTION_ON_CAN_NODE(CAN_GENERAL, can_node)
    {
      p_self->mailbox_imu_1 =
        hardware_get_mailbox_single_mex(
            can_node,
            RECV_MAILBOX,
            CAN_ID_IMU1,
            message_dlc_can2(CAN_ID_IMU1));
    }

    if (!p_self->mailbox_imu_1)
    {
      hardware_free_mailbox_can(&p_self->mailbox_imu_1);
      hardware_free_mailbox_can(&p_self->mailbox_imu_2);
      return -3;
    }

    return 0;
}

int8_t imu_update(Imu_h* const restrict self )
{
    union Imu_h_t_conv conv = {self};
    struct Imu_t* const restrict p_self = conv.clear;
    CanMessage mex = {0};
    can_obj_can2_h_t o2 = {0};

    if (hardware_mailbox_read(p_self->mailbox_imu_1, &mex))
    {
      unpack_message_can2(&o2, mex.id, mex.full_word, mex.message_size, 0);
      p_self->acc[AXES_X] = (float) o2.can_0x060_Imu1.acc_x;
      p_self->acc[AXES_Y] = (float) o2.can_0x060_Imu1.acc_y;
    }

    if (hardware_mailbox_read(p_self->mailbox_imu_2, &mex))
    {
      unpack_message_can2(&o2, mex.id, mex.full_word, mex.message_size, 0);
      p_self->acc[AXES_Z] = (float) o2.can_0x061_Imu2.acc_z;
      p_self->omega[AXES_X] = (float) o2.can_0x061_Imu2.omega_x;
    }

    if (hardware_mailbox_read(p_self->mailbox_imu_3, &mex))
    {
      unpack_message_can2(&o2, mex.id, mex.full_word, mex.message_size, 0);
      p_self->acc[AXES_Y] = (float) o2.can_0x062_Imu3.omega_y;
      p_self->omega[AXES_Z] = (float) o2.can_0x062_Imu3.omega_z;
    }

    return 0;
}

float imu_get_acc(const Imu_h* const restrict self,
        const enum IMU_AXIS axes)
{
    const union Imu_h_t_conv_const conv = {self};
    const struct Imu_t* const restrict p_self = conv.clear;
    if (axes < __NUM_OF_AXIS__)
    {
        return p_self->acc[axes];
    }
    return -1;
}

float imu_get_omega(const Imu_h* const restrict self,
        const enum IMU_AXIS axes)
{
    const union Imu_h_t_conv_const conv = {self};
    const struct Imu_t* const restrict p_self = conv.clear;
    if (axes < __NUM_OF_AXIS__)
    {
        return p_self->omega[axes];
    }
    return -1;
}
