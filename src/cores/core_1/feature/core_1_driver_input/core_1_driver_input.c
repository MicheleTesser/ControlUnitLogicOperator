#include "core_1_driver_input.h"
#include "../../../../lib/raceup_board/components/can.h"
#include <stdint.h>
#include <string.h>

enum DRIVER_INPUT{
    THROTTLE=0,
    BRAKE,
    REGEN,
    STERRING_WHEEL,

    __NUM_OF_DRIVER_INPUT__
};

enum DRIVER_IMP{
    IMP_THROTTLE_BRAKE=0,
    IMP_THROTTLE_REGEN,
    IMP_THROTTLE_POT,
};

struct Core1DriverInput_t{
    float driver_input[__NUM_OF_DRIVER_INPUT__];
    struct CanMailbox* driver_input_mailbox;
    struct CanMailbox* regen_stw_mailbox;
    uint8_t impl;
};

union Core1DriverInput_h_t_conv{
    Core1DriverInput_h* const restrict hidden;
    struct Core1DriverInput_t* const restrict clear;
};

#define ADD_ENTRY_TO_LOG(LOG, MODE, DATA, NAME)\
{\
    struct LogEntry_h entry= {\
        .data_mode = MODE,\
        .data_ptr = &DATA,\
        .data_size = sizeof(DATA),\
        .log_mode = LOG_SD | LOG_TELEMETRY,\
        .name = NAME,\
    };\
    if (log_add_entry(log, &entry)<0)\
    {\
        return -1;\
    }\
}

int8_t
core_1_driver_input_init(
        Core1DriverInput_h* const restrict self __attribute__((__nonnull__)),
        Log_h* const restrict log __attribute__((__nonnull__)))
{
    union Core1DriverInput_h_t_conv conv = {self};
    struct Core1DriverInput_t* const restrict p_self = conv.clear;
    
    memset(p_self, 0, sizeof(*p_self));

    ADD_ENTRY_TO_LOG(log, DATA_FLOATED, p_self->driver_input[THROTTLE],
            "driver input throttle");

    ADD_ENTRY_TO_LOG(log, DATA_FLOATED, p_self->driver_input[BRAKE],
            "driver input brake");

    ADD_ENTRY_TO_LOG(log, DATA_FLOATED, p_self->driver_input[REGEN],
            "driver input regen");

    ADD_ENTRY_TO_LOG(log, DATA_FLOATED, p_self->driver_input[STERRING_WHEEL],
            "driver input steering wheel");

    ADD_ENTRY_TO_LOG(log, DATA_FLOATED, p_self->driver_input[STERRING_WHEEL],
            "driver input steering wheel");

    ADD_ENTRY_TO_LOG(log, DATA_UNSIGNED, p_self->impl,
            "driver input implausibility list");


    p_self->driver_input_mailbox = hardware_get_mailbox(CORE_1_DRIVER);
    if(!p_self->driver_input_mailbox)
    {
        return -1;
    }

    p_self->regen_stw_mailbox = hardware_get_mailbox(CORE_1_REGEN);
    if(!p_self->regen_stw_mailbox)
    {
        hardware_free_mailbox_can(&p_self->driver_input_mailbox);
        return -1;
    }


    return 0;
}

int8_t
core_1_driver_input_update(Core1DriverInput_h* const restrict self);
