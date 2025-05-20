#include "core_1_driver_input.h"
#include "../../../../lib/raceup_board/components/can.h"
#include "../../../core_utility/core_utility.h"

#include <stdint.h>
#include <string.h>

enum DRIVER_IMP{
    IMP_THROTTLE_BRAKE=0,
    IMP_THROTTLE_REGEN,
    IMP_THROTTLE_POT,
};

struct Core1DriverInput_t{
    float m_driver_input[__NUM_OF_INPUT_TYPES__];
    uint8_t m_impl;
    DriverInputReader_h m_driver;
};

union Core1DriverInput_h_t_conv{
    Core1DriverInput_h* const restrict hidden;
    struct Core1DriverInput_t* const restrict clear;
};

#define ADD_ENTRY_TO_LOG(LOG, MODE, DATA, NAME)\
{\
    LogEntry_h entry= {\
        .data_mode = MODE,\
        .data_ptr = &DATA,\
        .log_mode = LOG_SD | LOG_TELEMETRY,\
        .name = NAME,\
    };\
    if (log_add_entry(log, &entry)<0)\
    {\
        SET_TRACE(CORE_1);\
        return -1;\
    }\
}

#ifdef DEBUG
char __assert_size_core_1_driver_input[(sizeof(Core1DriverInput_h) == sizeof(struct Core1DriverInput_t))? 1:-1];
char __assert_align_core_1_driver_input[(_Alignof(Core1DriverInput_h) == _Alignof(struct Core1DriverInput_t))? 1:-1];
#endif // DEBUG

int8_t core_1_driver_input_init(
        Core1DriverInput_h* const restrict self ,
        Log_h* const restrict log )
{
    union Core1DriverInput_h_t_conv conv = {self};
    struct Core1DriverInput_t* const restrict p_self = conv.clear;
    
    if (driver_input_reader_init(&p_self->m_driver)<0)
    {
      SET_TRACE(CORE_1);
      return -1;
    }

    memset(p_self, 0, sizeof(*p_self));

    ADD_ENTRY_TO_LOG(log, __float__, p_self->m_driver_input[THROTTLE],
            "driver input throttle");

    ADD_ENTRY_TO_LOG(log, __float__, p_self->m_driver_input[BRAKE],
            "driver input brake/regen");

    ADD_ENTRY_TO_LOG(log, __float__, p_self->m_driver_input[STEERING_ANGLE],
            "driver input steering wheel");

    ADD_ENTRY_TO_LOG(log, __float__, p_self->m_impl,
            "driver input impls list");


    return 0;
}

int8_t core_1_driver_input_update(Core1DriverInput_h* const restrict self)
{
  union Core1DriverInput_h_t_conv conv = {self};
  struct Core1DriverInput_t* const restrict p_self __attribute__((__unused__))= conv.clear;

  p_self->m_driver_input[THROTTLE] = driver_input_reader_get(&p_self->m_driver, DRIVER_HUMAN, THROTTLE);
  p_self->m_driver_input[BRAKE] = driver_input_reader_get(&p_self->m_driver, DRIVER_HUMAN, BRAKE);
  p_self->m_driver_input[STEERING_ANGLE] = driver_input_reader_get(&p_self->m_driver, DRIVER_HUMAN, STEERING_ANGLE);

  return 0;
}
