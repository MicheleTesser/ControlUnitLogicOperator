#include "imu.h"
#include "../../../../lib/board_dbc/dbc/out_lib/cansbg/cansbg.h"
#include "../../../../lib/raceup_board/raceup_board.h"
#include <string.h>

//TODO: imu can messages not yet defined

struct Imu_t{
    float acc[__NUM_OF_AXIS__];
    struct CanMailbox* mailbox_imu;
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
      //TODO: not yet defined
      p_self->mailbox_imu =
        hardware_get_mailbox_single_mex(
            can_node,
            RECV_MAILBOX,
            1,
            8);
    }

    if (!p_self->mailbox_imu)
    {
      return -1;
    }

    return 0;
}

int8_t imu_update(Imu_h* const restrict self )
{
    union Imu_h_t_conv conv = {self};
    struct Imu_t* const restrict p_self __attribute__((__unused__)) = conv.clear;

    return 0;
}

float imu_get_acc(const Imu_h* const restrict self,
        const enum IMU_AXIS axes)
{
    const union Imu_h_t_conv_const conv = {self};
    const struct Imu_t* const restrict p_self = conv.clear;
    if (axes != __NUM_OF_AXIS__) {
        return p_self->acc[axes];
    }
    return -1;
}
