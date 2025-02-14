#include "amk_inverter.h"
#include "../../linux_board/linux_board.h"
#include "../src/cores/core_0/feature/engines/engines.h"
#include "../src/lib/board_dbc/dbc/out_lib/can1/can1.h"
#include "../external_gpio.h"

#include <linux/can.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <threads.h>
#include <unistd.h>
#include <fcntl.h>

struct AMK_Actual_Values_1{
  struct{
    uint8_t b_reserve; //INFO: Reserved
    uint8_t AMK_bSystemReady: 1; //INFO: System ready (SBM)
    uint8_t AMK_bError :1; //INFO: Error
    uint8_t AMK_bWarn :1; //INFO: Warning
    uint8_t AMK_bQuitDcOn :1; //INFO: HV activation acknowledgment
    uint8_t AMK_bDcOn :1; //INFO: HV activation level
    uint8_t AMK_bQuitInverterOn:1; //INFO: Controller enable acknowledgment
    uint8_t AMK_bEnable:1;
    uint8_t AMK_bInverterOn:1; //INFO: Controller enable level
    uint8_t AMK_bDerating :1; //INFO: Derating (torque limitation active)
  }AMK_STATUS;
  //INFO:  AMK_ActualVelocity : Unit = rpm; Actual speed value
  int16_t AMK_ActualVelocity; 
  //INFO: AMK_TorqueCurrent: Raw data for calculating 'actual torque current' Iq See 'Units' on page 65.               
  int16_t AMK_TorqueCurrent; 
  //INFO: Raw data for calculating 'actual magnetizing current' Id See 'Units' on page 1.
  int16_t AMK_MagnetizingCurrent;
};

struct AMK_Actual_Values_2{
  int16_t AMK_TempMotor; //INFO: Unit= 0.1 °C. Motor temperature
  int16_t AMK_TempInverter; //INFO: Unit= 0.1 °C. Cold plate temperature
  uint16_t AMK_ErrorInfo; //INFO: Diagnostic number
  int16_t AMK_TempIGBT; //INFO: 0.1 °C. IGBT temperature
};

struct AMK_Setpoints{
  //INFO: AMK_Control: 
  //Control word See the table below: Content of the 'AMK_Control' control word
  struct {
    uint8_t AMK_bReserve_start;
    uint8_t AMK_bInverterOn:1;
    uint8_t AMK_bDcOn:1;
    uint8_t AMK_bEnable:1;
    uint8_t AMK_bErrorReset:1;
    uint8_t AMK_bReserve_end :4;
  }AMK_Control_fields;
  int16_t AMK_TargetVelocity; //INFO: unit: rpm Speed setpoint
  int16_t AMK_TorqueLimitPositive; //INFO: unit: 0.1% MN Positive torque limit (subject to nominal torque)
  int16_t AMK_TorqueLimitNegative; //INFO: unit: 0.1% MN Negative torque limit (subject to nominal torque)
};

struct EmulationAmkInverter_t{
  struct amk_engines{
    struct AMK_Actual_Values_1 amk_data_1;
    struct AMK_Actual_Values_2 amk_data_2;
  }engines[4];
  struct CanNode* CanNodeInverter;
  thrd_t t;
  thrd_t update;
  uint8_t running:1;
  Gpio_h air_1;
  Gpio_h air_2;
  GpioRead_h rf_signal;
};

union AmkInverter_h_t_conv{
  EmulationAmkInverter_h* const restrict hidden;
  struct EmulationAmkInverter_t* const restrict clear;
};

#ifdef DEBUG
const char __assert_size_amk_inverter[(sizeof(EmulationAmkInverter_h)==sizeof(struct EmulationAmkInverter_t))?1:-1]; 
#endif /* ifdef DEBUG */

#define UPDATE_DATA(self, mex, rf_signal)\
{\
  self.AMK_STATUS.AMK_bDcOn = mex.AMK_bDcOn;\
  self.AMK_STATUS.AMK_bInverterOn = mex.AMK_bInverterOn && self.AMK_STATUS.AMK_bQuitDcOn;\
  self.AMK_STATUS.AMK_bQuitInverterOn = self.AMK_STATUS.AMK_bInverterOn;\
  self.AMK_STATUS.AMK_bEnable = mex.AMK_bEnable && self.AMK_STATUS.AMK_bQuitInverterOn && rf_signal;\
  if (mex.AMK_bErrorReset)\
  {\
    self.AMK_STATUS.AMK_bError = 0;\
    self.AMK_STATUS.AMK_bWarn = 0;\
    self.AMK_STATUS.AMK_bDerating = 0;\
  }\
}

static int 
car_amk_inverter_update(void* args)
{
  union AmkInverter_h_t_conv conv = {args};
  struct EmulationAmkInverter_t* const restrict p_self = conv.clear;

  while (p_self->running)
  {
    CanMessage mex = {0};
    can_obj_can1_h_t o1 ={0};
    const uint8_t rf_signal = gpio_read_state(&p_self->rf_signal);

    hardware_read_can(p_self->CanNodeInverter,&mex);
    unpack_message_can1(&o1, mex.id,mex.full_word, mex.message_size, 0);
    switch (mex.id)
    {
      case CAN_ID_VCUINVFL:
        UPDATE_DATA(p_self->engines[FRONT_LEFT].amk_data_1, o1.can_0x184_VCUInvFL,rf_signal);
        break;
      case CAN_ID_VCUINVFR:
        UPDATE_DATA(p_self->engines[FRONT_RIGHT].amk_data_1, o1.can_0x185_VCUInvFR,rf_signal);
        break;
      case CAN_ID_VCUINVRL:
        UPDATE_DATA(p_self->engines[REAR_LEFT].amk_data_1, o1.can_0x188_VCUInvRL,rf_signal);
        break;
      case CAN_ID_VCUINVRR:
        UPDATE_DATA(p_self->engines[REAR_RIGHT].amk_data_1, o1.can_0x189_VCUInvRR,rf_signal);
        break;
    }
  }
  return 0;
}

//public

#define POPULATE_MEX_ENGINE(self, mex, engine)\
{\
  struct AMK_Actual_Values_1* s_w = &self->engines[engine].amk_data_1;\
  \
  mex.SystemReady = s_w->AMK_STATUS.AMK_bSystemReady;\
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
send_data_engine(EmulationAmkInverter_h* self, const uint16_t can_id) 
{
  union AmkInverter_h_t_conv conv = {self};
  struct EmulationAmkInverter_t* const restrict p_self = conv.clear;
  CanMessage mex = {0};
  can_obj_can1_h_t o = {0};
  uint64_t data=0;

  switch (can_id) {
    case CAN_ID_INVERTERFL1:
      POPULATE_MEX_ENGINE(p_self, o.can_0x283_InverterFL1, FRONT_LEFT);
      break;
    case CAN_ID_INVERTERFR1:
      POPULATE_MEX_ENGINE(p_self, o.can_0x284_InverterFR1, FRONT_RIGHT);
      break;
    case CAN_ID_INVERTERRR1:
      POPULATE_MEX_ENGINE(p_self, o.can_0x288_InverterRR1, REAR_RIGHT);
      break;
    case CAN_ID_INVERTERRL1:
      POPULATE_MEX_ENGINE(p_self, o.can_0x287_InverterRL1, REAR_LEFT);
      break;
    default:
      return;
  }
  mex.id= can_id;
  mex.message_size = pack_message_can1(&o, can_id,&data);
  memcpy(&mex.full_word, &data, mex.message_size);
  hardware_write_can(p_self->CanNodeInverter, &mex);
}

static int inverter_start(void* args __attribute_maybe_unused__)
{
  EmulationAmkInverter_h* self= args;
  union AmkInverter_h_t_conv conv = {self};
  struct EmulationAmkInverter_t* const restrict p_self = conv.clear; 
  time_var_microseconds start_precharge =0;
  time_var_microseconds t = 0;

  while (p_self->running)
  {
    uint8_t precharge_ready=1;

    FOR_EACH_ENGINE(engine)
    {
      if (!p_self->engines[engine].amk_data_1.AMK_STATUS.AMK_bDcOn)
      {
        precharge_ready=0;
        break;
      }
    }



    if (precharge_ready)
    {
      gpio_set_low(&p_self->air_1);
      if (!start_precharge)
      {
        start_precharge = timer_time_now();
      }

      if ((timer_time_now() - start_precharge) > 5 SECONDS)
      {
        printf("precharge ended\n");
        start_precharge = timer_time_now();
        FOR_EACH_ENGINE(engine)
        {
          struct AMK_Actual_Values_1* p_val = &p_self->engines[engine].amk_data_1;
          p_val->AMK_STATUS.AMK_bQuitDcOn=1;
          gpio_set_low(&p_self->air_2);
        }
      }
    }else
    {
      start_precharge=0;
      gpio_set_high(&p_self->air_1);
      gpio_set_high(&p_self->air_2);
    }

    if (!gpio_read_state(&p_self->rf_signal))
    {
      FOR_EACH_ENGINE(engine){
          p_self->engines[engine].amk_data_1.AMK_STATUS.AMK_bEnable =0;
          p_self->engines[engine].amk_data_1.AMK_MagnetizingCurrent =0;
          p_self->engines[engine].amk_data_1.AMK_TorqueCurrent = 0;
          p_self->engines[engine].amk_data_1.AMK_ActualVelocity =0;
      }
    }

    if ((timer_time_now() - t) > 50 MILLIS)
    {
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

int8_t
car_amk_inverter_class_init(EmulationAmkInverter_h* self)
{
  union AmkInverter_h_t_conv conv = {self};
  struct EmulationAmkInverter_t* const restrict p_self = conv.clear;
  memset(self, 0, sizeof(*self));

  p_self->CanNodeInverter = hardware_init_can_get_ref_node_new(CAN_INVERTER);
  if(!p_self->CanNodeInverter)
  {
    return -1;
  }

  if(hardware_init_gpio(&p_self->air_1, GPIO_AIR_PRECHARGE_INIT)<0)
  {
    return -2;
  }

  if(hardware_init_gpio(&p_self->air_2, GPIO_AIR_PRECHARGE_DONE)<0)
  {
    return -3;
  }

  if(hardware_init_read_permission_gpio(&p_self->rf_signal, GPIO_INVERTER_RF_SIGNAL)<0)
  {
    return -4;
  }

  p_self->running=1;
  thrd_create(&p_self->t, inverter_start, self);
  thrd_create(&p_self->update, car_amk_inverter_update, self);
  return 0;
}

void 
car_amk_inverter_reset(EmulationAmkInverter_h* self)
{
  union AmkInverter_h_t_conv conv = {self};
  struct EmulationAmkInverter_t* const restrict p_self = conv.clear;
  FOR_EACH_ENGINE(engine){
    memset(&p_self->engines[engine].amk_data_1, 0,
        sizeof(p_self->engines[engine].amk_data_1));
    memset(&p_self->engines[engine].amk_data_2, 0,
        sizeof(p_self->engines[engine].amk_data_2));
  }
}

int8_t
car_amk_inverter_set_attribute(EmulationAmkInverter_h* self,
    const enum INVERTER_ATTRIBUTE attribute, const uint8_t engine,
    const int64_t value)
{
  union AmkInverter_h_t_conv conv = {self};
  struct EmulationAmkInverter_t* const restrict p_self = conv.clear;
  struct amk_engines* p_engine = &p_self->engines[engine];

  switch (attribute)
  {
    case ERROR:
      p_engine->amk_data_1.AMK_STATUS.AMK_bError = value;
      break;
    case WARNING:
      p_engine->amk_data_1.AMK_STATUS.AMK_bWarn = value;
      break;
    case DERATRING:
      p_engine->amk_data_1.AMK_STATUS.AMK_bDerating = value;
      break;
    case SYSTEM_READY:
      p_engine->amk_data_1.AMK_STATUS.AMK_bSystemReady = value;
      break;
  }

  return 0;
}

int8_t
car_amk_inverter_set_engine_value(EmulationAmkInverter_h* self,
    const enum ENGINE_STATUS status, const uint8_t engine, 
    const float value)
{
  union AmkInverter_h_t_conv conv = {self};
  struct EmulationAmkInverter_t* const restrict p_self = conv.clear;
  struct amk_engines* p_engine = &p_self->engines[engine];

  if (!gpio_read_state(&p_self->rf_signal))
  {
    return -1;
  }

  FOR_EACH_ENGINE(engine)
  {
    if (!p_self->engines[engine].amk_data_1.AMK_STATUS.AMK_bEnable)
    {
      return -2;
    }
  }

  switch (status)
  {
    case ACTUAL_VELOCITY:
      p_engine->amk_data_1.AMK_ActualVelocity = value;
      break;
    case TORQUE_CURRENT:
      p_engine->amk_data_1.AMK_TorqueCurrent = value;
      break;
    case MAGNETIZING_CURRENT:
      p_engine->amk_data_1.AMK_MagnetizingCurrent = value;
      break;
    case TEMP_MOTOR:
      p_engine->amk_data_2.AMK_TempMotor= value;
      break;
    case TEMP_INVERTER:
      p_engine->amk_data_2.AMK_TempInverter = value;
      break;
    case TEMP_IGBT:
      p_engine->amk_data_2.AMK_TempIGBT = value;
      break;
    case ERROR_INFO:
      p_engine->amk_data_2.AMK_ErrorInfo = value;
      break;
    default:
      return -1;
  }

  return 0;
}


void car_amk_inverter_stop(EmulationAmkInverter_h* self)
{
  union AmkInverter_h_t_conv conv = {self};
  struct EmulationAmkInverter_t* const restrict p_self = conv.clear;
  
  p_self->running=0;
  hardware_init_can_get_ref_node_destroy(p_self->CanNodeInverter);

  return;
}

void
car_amk_inverter_emergency_shutdown(EmulationAmkInverter_h* self)
{
  union AmkInverter_h_t_conv conv = {self};
  struct EmulationAmkInverter_t* const restrict p_self = conv.clear;
  gpio_set_high(&p_self->air_1);
  gpio_set_high(&p_self->air_2);
}
