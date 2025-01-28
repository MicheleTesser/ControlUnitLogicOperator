#include "colling.h"
#include "../../../../lib/raceup_board/components/can.h"
#include "../../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#include <stdint.h>
#include <string.h>

#define MAX_MAILBOX_FOR_DEVICE 4

struct CoolingDevice_t{
    uint8_t enable:1;
    uint8_t speed:7;
};

typedef struct Cooling_t{
    struct CanMailbox* send_mailbox;
    struct CoolingDevice_t devices[__NUM_OF_COOLING_DEVICES__];
}Cooling_t;

union Cooling_h_t_conv{
    Cooling_h* hidden;
    struct Cooling_t* clear;
};

#ifdef DEBUG
char __assert_size_cooling[(sizeof(Cooling_h) == sizeof(struct Cooling_t))? 1:-1];
#endif // DEBUG

static int8_t update_status(struct Cooling_t* const restrict self)
{
    can_obj_can2_h_t o;
    CanMessage mex;

    mex.id = CAN_ID_PCU;
    o.can_0x130_Pcu.fan_enable = self->devices[FANS_RADIATOR].enable;
    o.can_0x130_Pcu.fan_speed = self->devices[FANS_RADIATOR].speed;

    o.can_0x130_Pcu.pump_enable = self->devices[PUMPS].enable;
    o.can_0x130_Pcu.pump_speed = self->devices[PUMPS].speed;
    
    mex.message_size = pack_message_can2(&o, mex.id, &mex.full_word);

    return hardware_mailbox_send(self->send_mailbox, mex.full_word);
}

//public

int8_t cooling_init(Cooling_h* const restrict self ,
        Log_h* const restrict log )
{
    union Cooling_h_t_conv conv = {self};
    struct Cooling_t* const p_self = conv.clear;
    memset(p_self, 0, sizeof(*p_self));

    ACTION_ON_CAN_NODE(CAN_GENERAL,{
        p_self->send_mailbox = hardware_get_mailbox_send(can_node, CAN_ID_PCU,2);
    })
    if (!p_self->send_mailbox)
    {
        return -1;
    }

    {
        struct LogEntry_h entry ={
            .data_mode = DATA_UNSIGNED,
            .data_ptr = &p_self->devices[FANS_RADIATOR],
            .log_mode = LOG_SD | LOG_TELEMETRY,
            .data_min = 0,
            .data_max = 100,
            .name = "temp fan speed/enable",
        };
        if (log_add_entry(log, &entry)<0)
        {
            return -2;
        }
    }

    {
        struct LogEntry_h entry ={
            .data_mode = DATA_UNSIGNED,
            .data_ptr = &p_self->devices[PUMPS],
            .log_mode = LOG_SD | LOG_TELEMETRY,
            .data_min = 0,
            .data_max = 100,
            .name = "temp pump speed/enable",
        };
        if (log_add_entry(log, &entry)<0)
        {
            return -3;
        }
    }

    return 0;
}

int8_t cooling_switch_device(Cooling_h* const restrict self, const enum COOLING_DEVICES dev_id)
{
#ifdef DEBUG
    if (dev_id==__NUM_OF_COOLING_DEVICES__)
    {
        return -1;
    }
#endif /* ifdef DEBUG */
    union Cooling_h_t_conv conv = {self};
    struct Cooling_t* const p_self = conv.clear;

    p_self->devices[dev_id].enable ^= 1;
    return update_status(p_self);
}

int8_t cooling_set_speed_device(Cooling_h* const restrict self ,
        const enum COOLING_DEVICES dev_id, const float speed)
{
#ifdef DEBUG
    if (dev_id==__NUM_OF_COOLING_DEVICES__)
    {
        return -1;
    }
#endif /* ifdef DEBUG */
    union Cooling_h_t_conv conv = {self};
    struct Cooling_t* const p_self = conv.clear;

    p_self->devices[dev_id].speed = speed;
    
    return update_status(p_self);
}
