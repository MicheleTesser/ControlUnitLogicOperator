#include <stdint.h>
#include <string.h>
#include "../../log/log.h"
#include "../../../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#include "../../../../../lib/raceup_board/raceup_board.h"
#include "bms.h"

enum VOLTS{
    MAX=0,
    MIN,
    AVG,

    __NUM_OF_VOLTS__
};

struct Bms_t{
    const struct CanMailbox* mailbox;
    uint16_t volts[__NUM_OF_VOLTS__];
    uint8_t soc;
};

union Hv_h_t_conv{
    Bms_h* const restrict hidden;
    struct Bms_t* const restrict clear;
};

int8_t
bms_init(Bms_h* const restrict self ,
        const uint16_t bms_id, const char* const restrict bms_name,
        Log_h* const restrict log __attribute__((__unused__)))
{
    union Hv_h_t_conv conv = {self};
    struct Bms_t* const restrict p_self = conv.clear;

    memset(p_self, 0, sizeof(*p_self));

    p_self->mailbox = hardware_get_mailbox(bms_id);
    if (!p_self->mailbox) {
        return -1;
    }

    {
        LogEntry_h entry = {
            .data_max = 4200,
            .data_min = 1900,
            .data_mode = DATA_UNSIGNED,
            .data_ptr = &p_self->volts[MAX],
            .log_mode = LOG_TELEMETRY| LOG_SD,
            .name = "Bms Max Volt",
        };
        if(log_add_entry(log, &entry)<0)
        {
            return -1;
        }
    }

    {
        LogEntry_h entry = {
            .data_max = 4200,
            .data_min = 1900,
            .data_mode = DATA_UNSIGNED,
            .data_ptr = &p_self->volts[MIN],
            .log_mode = LOG_TELEMETRY| LOG_SD,
            .name = "Bms bms Min Volt",
        };
        if(log_add_entry(log, &entry)<0)
        {
            return -2;
        }
    }


    {
        LogEntry_h entry = {
            .data_max = 100,
            .data_min = 0,
            .data_mode = DATA_UNSIGNED,
            .data_ptr = &p_self->soc,
            .log_mode = LOG_TELEMETRY| LOG_SD,
            .name = "Bms sv soc",
        };
        if(log_add_entry(log, &entry)<0)
        {
            return -3;
        }
    }

    return 0;
}

int8_t
bms_update(Bms_h* const restrict self )
{
    union Hv_h_t_conv conv = {self};
    struct Bms_t* const restrict p_self = conv.clear;
    can_obj_can2_h_t o;
    uint64_t data=0;

    if (!hardware_mailbox_read(p_self->mailbox, &data))
    {
        unpack_message_can2(&o, CAN_ID_BMSHV1, data, 7, timer_time_now());
        p_self->volts[MAX] = o.can_0x057_BmsHv1.max_volt;
        p_self->volts[MIN] = o.can_0x057_BmsHv1.min_volt;
        p_self->volts[AVG] = o.can_0x057_BmsHv1.avg_volt;
        p_self->soc = o.can_0x057_BmsHv1.soc;
    }

    return 0;
}
