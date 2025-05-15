#include "imu.h"
#include "../shared_message/shared_message.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "../../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#pragma GCC diagnostic pop
#include "../../../../lib/raceup_board/raceup_board.h"
#include <string.h>

struct Imu_t{
    float m_acc[__NUM_OF_AXIS__];
    float m_omega[__NUM_OF_AXIS__];
    SharedMessageReader_h m_recv_imu_1;
    SharedMessageReader_h m_recv_imu_2;
    SharedMessageReader_h m_recv_imu_3;
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

    memset(p_self, 0, sizeof(*p_self));

    if (shared_message_reader_init(&p_self->m_recv_imu_1, SHARED_MEX_IMU1))
    {
      return -1;
    }

    if (shared_message_reader_init(&p_self->m_recv_imu_2, SHARED_MEX_IMU2))
    {
      return -2;
    }

    if (shared_message_reader_init(&p_self->m_recv_imu_3, SHARED_MEX_IMU3))
    {
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

    if (shared_message_read_unpack_can2(&p_self->m_recv_imu_1, &o2)>0)
    {
      p_self->m_acc[AXES_X] = (float) o2.can_0x060_Imu1.acc_x;
      p_self->m_acc[AXES_Y] = (float) o2.can_0x060_Imu1.acc_y;
    }

    if (shared_message_read_unpack_can2(&p_self->m_recv_imu_2, &o2)>0)
    {
      p_self->m_acc[AXES_Z] = (float) o2.can_0x061_Imu2.acc_z;
      p_self->m_omega[AXES_X] = (float) o2.can_0x061_Imu2.omega_x;
    }

    if (shared_message_read_unpack_can2(&p_self->m_recv_imu_3, &o2)>0)
    {
      unpack_message_can2(&o2, mex.id, mex.full_word, mex.message_size, 0);
      p_self->m_acc[AXES_Y] = (float) o2.can_0x062_Imu3.omega_y;
      p_self->m_omega[AXES_Z] = (float) o2.can_0x062_Imu3.omega_z;
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
        return p_self->m_acc[axes];
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
        return p_self->m_omega[axes];
    }
    return -1;
}
