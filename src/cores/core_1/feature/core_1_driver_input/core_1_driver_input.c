#include "core_1_driver_input.h"
#include "../../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
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
    uint8_t impl;
};

union Core1DriverInput_h_t_conv{
    Core1DriverInput_h* const restrict hidden;
    struct Core1DriverInput_t* const restrict clear;
};

#define ADD_ENTRY_TO_LOG(LOG, MODE, DATA, NAME, POS)\
{\
    struct LogEntry_h entry= {\
        .data_mode = MODE,\
        .data_ptr = &DATA,\
        .data_min = 0,\
        .data_max = 100,\
        .log_mode = LOG_SD | LOG_TELEMETRY,\
        .name = NAME,\
    };\
    if (log_add_entry(log, &entry,POS)<0)\
    {\
        return -1;\
    }\
}

#ifdef DEBUG
char __assert_size_core_1_driver_input[(sizeof(Core1DriverInput_h) == sizeof(struct Core1DriverInput_t))? 1:-1];
#endif // DEBUG

int8_t
core_1_driver_input_init(
        Core1DriverInput_h* const restrict self ,
        Log_h* const restrict log )
{
    union Core1DriverInput_h_t_conv conv = {self};
    struct Core1DriverInput_t* const restrict p_self = conv.clear;
    struct CanNode* can_node = NULL;
    
    memset(p_self, 0, sizeof(*p_self));

    ADD_ENTRY_TO_LOG(log, DATA_FLOATED, p_self->driver_input[THROTTLE],
            "driver input throttle",11);

    ADD_ENTRY_TO_LOG(log, DATA_FLOATED, p_self->driver_input[BRAKE],
            "driver input brake",12);

    ADD_ENTRY_TO_LOG(log, DATA_FLOATED, p_self->driver_input[REGEN],
            "driver input regen",13);

    ADD_ENTRY_TO_LOG(log, DATA_FLOATED, p_self->driver_input[STERRING_WHEEL],
            "driver input steering wheel",14);

    ADD_ENTRY_TO_LOG(log, DATA_FLOATED, p_self->driver_input[STERRING_WHEEL],
            "driver input steering wheel",15);

    ADD_ENTRY_TO_LOG(log, DATA_UNSIGNED, p_self->impl,
            "driver input implausibility list",16);


    ACTION_ON_CAN_NODE(CAN_GENERAL, can_node)
    {
      p_self->driver_input_mailbox =
        hardware_get_mailbox_single_mex(can_node,RECV_MAILBOX, CAN_ID_DRIVER,4);
      if(!p_self->driver_input_mailbox)
      {
      return -1;
      }
    }


    return 0;
}

int8_t
core_1_driver_input_update(Core1DriverInput_h* const restrict self)
{
  union Core1DriverInput_h_t_conv conv = {self};
  struct Core1DriverInput_t* const restrict p_self __attribute__((__unused__))= conv.clear;
  return 0;
}
