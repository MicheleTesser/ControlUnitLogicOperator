#include "amk_inverter.h"
#include "../../linux_board/linux_board.h"
#include "../src/cores/core_0/feature/engines/engines.h"
#include "../src/lib/board_dbc/dbc/out_lib/can1/can1.h"

#include <stdint.h>
#include <stdio.h>
#include <threads.h>
#include <unistd.h>
#include <fcntl.h>


#define POPULATE_MEX_ENGINE(self, mex, engine)\
{\
    struct AMK_Actual_Values_1* s_w = &self->engines[engine].amk_data_1;\
\
    mex.SystemReady = s_w->AMK_STATUS.bSystemReady;\
    mex.HVOn = s_w->AMK_STATUS.AMK_bDcOn;\
    mex.HVOnAck = s_w->AMK_STATUS.AMK_bQuitDcOn;\
    mex.Derating = s_w->AMK_STATUS.AMK_bDerating;\
    mex.InverterOn = s_w->AMK_STATUS.AMK_bInverterOn;\
    mex.InverterOnAck = s_w->AMK_STATUS.AMK_bQuitInverterOn;\
\
    mex.Error = s_w->AMK_STATUS.AMK_bError;\
    mex.Warning = s_w->AMK_STATUS.AMK_bWarn;\
\
    mex.Voltage = s_w->AMK_TorqueCurrent;\
    mex.MagCurr = s_w->AMK_MagnetizingCurrent;\
    mex.ActualVelocity = s_w->AMK_ActualVelocity;\
}

static void
send_data_engine(struct AmkInverter* self, const uint16_t can_id) 
{
    CanMessage mex = {0};
    can_obj_can1_h_t o = {0};
    switch (can_id) {
        case CAN_ID_INVERTERFL1:
            POPULATE_MEX_ENGINE(self, o.can_0x283_InverterFL1, FRONT_LEFT);
            break;
        case CAN_ID_INVERTERFR1:
            POPULATE_MEX_ENGINE(self, o.can_0x284_InverterFR1, FRONT_RIGHT);
            break;
        case CAN_ID_INVERTERRR1:
            POPULATE_MEX_ENGINE(self, o.can_0x288_InverterRR1, REAR_LEFT);
            break;
        case CAN_ID_INVERTERRL1:
            POPULATE_MEX_ENGINE(self, o.can_0x287_InverterRL1, REAR_RIGHT);
            break;
    }
    mex.id = can_id;
    mex.message_size = pack_message_can1(&o, can_id, &mex.full_word);
    // hardware_write_can(CAN_MODULE_INVERTER, &mex);
}

static int inverter_start(void* args __attribute_maybe_unused__)
{
  struct AmkInverter* self= args;
  time_var_microseconds t = 0;
  while (1) {
    if ((timer_time_now() - t) > 100 MILLIS) {
      send_data_engine(self, CAN_ID_INVERTERFR1);
      send_data_engine(self, CAN_ID_INVERTERFL1);
      send_data_engine(self, CAN_ID_INVERTERRL1);
      send_data_engine(self, CAN_ID_INVERTERRR1);
      t = timer_time_now();
    }
  }
  return 0;
}

//public

void car_amk_inverter_class_init(struct AmkInverter* self)
{
    thrd_t t;
    thrd_create(&t, inverter_start, self);
}

int8_t car_amk_inverter_set_attribute(struct AmkInverter* self,
        const enum INVERTER_ATTRIBUTE attribute, const uint8_t engine,
        const int64_t value)
{
    struct amk_engines* p_engine = &self->engines[engine];
    switch (attribute) {
        case ERROR:
            p_engine->amk_data_1.AMK_STATUS.AMK_bError = value;
            break;
        case WARNING:
            p_engine->amk_data_1.AMK_STATUS.AMK_bWarn = value;
            break;
        case DERATRING:
            p_engine->amk_data_1.AMK_STATUS.AMK_bDerating = value;
            break;
        case HV:
            p_engine->amk_data_1.AMK_STATUS.AMK_bDcOn = value;
            break;
        case HV_ACK:
            p_engine->amk_data_1.AMK_STATUS.AMK_bQuitDcOn = value;
            break;
        case INVERTER_ON:
            p_engine->amk_data_1.AMK_STATUS.AMK_bInverterOn = value;
            break;
        case INVERTER_ON_ACK:
            p_engine->amk_data_1.AMK_STATUS.AMK_bQuitInverterOn = value;
            break;
        case SYSTEM_READY:
            p_engine->amk_data_1.AMK_STATUS.bSystemReady = value;
            break;
    }

    return 0;
}
