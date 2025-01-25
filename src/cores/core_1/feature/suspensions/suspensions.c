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

#define UPDATE_LOG(LOG, DATA, NAME)\
{\
    struct LogEntry_h entry ={\
        .data_ptr = &DATA,\
        .data_size = sizeof(DATA),\
        .log_mode = LOG_SD | LOG_TELEMETRY,\
        .data_mode = DATA_FLOATED,\
    };\
    if (log_add_entry(log, &entry)<0)\
    {\
        return -1;\
    }\
}

int8_t
suspensions_init(
        Suspensions_h* const restrict self __attribute__((__nonnull__)),
        Log_h* const restrict log __attribute__((__nonnull__)))
{
    union Suspensions_h_t_conv conv = {self};
    struct Suspensions_t* const restrict p_self = conv.clear;

    memset(p_self, 0, sizeof(*p_self));

    UPDATE_LOG(log, p_self->susps_value[SUSP_FRONT_LEFT] , "susps front left");
    UPDATE_LOG(log, p_self->susps_value[SUSP_FRONT_RIGHT] , "susps front right");
    UPDATE_LOG(log, p_self->susps_value[SUSP_REAR_LEFT] , "susps rear left");
    UPDATE_LOG(log, p_self->susps_value[SUSP_REAR_RIGHT] , "susps rear right");

    p_self->mailbox[M_FRONT] = hardware_get_mailbox(CORE_1_SUSP_FRONT);
    if (!p_self->mailbox[M_FRONT])
    {
        return -2;
    }

    p_self->mailbox[M_REAR] = hardware_get_mailbox(CORE_1_SUSP_REAR);
    if (!p_self->mailbox[M_REAR])
    {
        hardware_free_mailbox_can(&p_self->mailbox[M_FRONT]);
        return -3;
    }


    return 0;
}

int8_t
suspensions_update(Suspensions_h* const restrict self)
{
    union Suspensions_h_t_conv conv = {self};
    struct Suspensions_t* const restrict p_self = conv.clear;

    uint64_t mex;
    can_obj_can2_h_t o;

    if(hardware_mailbox_read(p_self->mailbox[M_FRONT], &mex)>0){
        unpack_message_can2(&o, CAN_ID_SUSPFRONT, mex, 8, timer_time_now());
        p_self->susps_value[SUSP_FRONT_LEFT] = o.can_0x104_SuspFront.susp_fl;
        p_self->susps_value[SUSP_FRONT_RIGHT] = o.can_0x104_SuspFront.susp_fr;
    }

    if(hardware_mailbox_read(p_self->mailbox[M_REAR],&mex)>0){
        unpack_message_can2(&o, CAN_ID_SUSPREAR, mex, 8, timer_time_now());
        p_self->susps_value[SUSP_REAR_LEFT] = o.can_0x102_SuspRear.susp_rl;
        p_self->susps_value[SUSP_REAR_RIGHT] = o.can_0x102_SuspRear.susp_rr;
    }

    return 0;
}
