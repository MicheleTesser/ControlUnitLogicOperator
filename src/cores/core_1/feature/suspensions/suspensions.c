#include "suspensions.h"
#include "../../../../lib/raceup_board/raceup_board.h"
#include "../../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#include <stdint.h>
#include <string.h>

enum SUSPS_MAILBOX{
    M_FRONT=0,
    M_REAR,

    __NUM_OF_SUSPS_MAILBOX__
};

struct Suspensions_t{
    float susps_value[__NUM_OF_SUSPS__];
    struct CanMailbox* mailbox[__NUM_OF_SUSPS_MAILBOX__]; 
};

union Suspensions_h_t_conv {
    Suspensions_h* const restrict hidden;
    struct Suspensions_t* const restrict clear;
};

#define UPDATE_LOG(LOG, DATA, NAME, POS)\
{\
    LogEntry_h entry ={\
        .data_ptr = &DATA,\
        .log_mode = LOG_SD | LOG_TELEMETRY,\
        .data_mode = __float__,\
    };\
    if (log_add_entry(log, &entry, POS)<0)\
    {\
        return -1;\
    }\
}

#ifdef DEBUG
char __assert_size_alive_suspensions[(sizeof(Suspensions_h) == sizeof(struct Suspensions_t))? 1:-1];
char __assert_align_alive_suspensions[(_Alignof(Suspensions_h) == _Alignof(struct Suspensions_t))? 1:-1];
#endif // DEBUG

int8_t
suspensions_init(
        Suspensions_h* const restrict self ,
        Log_h* const restrict log )
{
    union Suspensions_h_t_conv conv = {self};
    struct Suspensions_t* const restrict p_self = conv.clear;
    struct CanNode* can_node = NULL;

    memset(p_self, 0, sizeof(*p_self));

    UPDATE_LOG(log, p_self->susps_value[SUSP_FRONT_LEFT] , "susps front left",1);
    UPDATE_LOG(log, p_self->susps_value[SUSP_FRONT_RIGHT] , "susps front right",2);
    UPDATE_LOG(log, p_self->susps_value[SUSP_REAR_LEFT] , "susps rear left",3);
    UPDATE_LOG(log, p_self->susps_value[SUSP_REAR_RIGHT] , "susps rear right",4);

    ACTION_ON_CAN_NODE(CAN_GENERAL,can_node)
    {
      p_self->mailbox[M_FRONT] =
        hardware_get_mailbox_single_mex(
            can_node,
            RECV_MAILBOX,
            CAN_ID_SUSPFRONT,
            message_dlc_can2(CAN_ID_SUSPFRONT));
      if (!p_self->mailbox[M_FRONT])
      {
        return -2;
      }

      p_self->mailbox[M_REAR] =
        hardware_get_mailbox_single_mex(
            can_node,
            RECV_MAILBOX,
            CAN_ID_SUSPREAR,
            message_dlc_can2(CAN_ID_SUSPREAR));

      if (!p_self->mailbox[M_REAR])
      {
        return -3;
      }
    }


    return 0;
}

int8_t
suspensions_update(Suspensions_h* const restrict self)
{
    union Suspensions_h_t_conv conv = {self};
    struct Suspensions_t* const restrict p_self = conv.clear;

    CanMessage mex;
    can_obj_can2_h_t o;

    if(hardware_mailbox_read(p_self->mailbox[M_FRONT], &mex)>0){
        unpack_message_can2(&o, CAN_ID_SUSPFRONT, mex.full_word, 8, timer_time_now());
        p_self->susps_value[SUSP_FRONT_LEFT] = o.can_0x104_SuspFront.susp_fl;
        p_self->susps_value[SUSP_FRONT_RIGHT] = o.can_0x104_SuspFront.susp_fr;
    }

    if(hardware_mailbox_read(p_self->mailbox[M_REAR],&mex)>0){
        unpack_message_can2(&o, CAN_ID_SUSPREAR, mex.full_word, 8, timer_time_now());
        p_self->susps_value[SUSP_REAR_LEFT] = o.can_0x102_SuspRear.susp_rl;
        p_self->susps_value[SUSP_REAR_RIGHT] = o.can_0x102_SuspRear.susp_rr;
    }

    return 0;
}
