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
  }o_engines[4];
  Gpio_h o_precharge_init;
  Gpio_h o_precharge_done;
  GpioRead_h o_rf;
  thrd_t o_thread;
  struct CanNode* p_can_node_inverter;
  struct CanMailbox* p_recv_mailbox_vcu;
  uint8_t o_running:1;
  time_var_microseconds o_precharge_started_at;
  time_var_microseconds o_last_sent_at;
};

union AmkInverter_h_t_conv{
  EmulationAmkInverter_h* const hidden;
  struct EmulationAmkInverter_t* const clear;
};

union AmkInverter_h_t_conv_const{
  const EmulationAmkInverter_h* const hidden;
  const struct EmulationAmkInverter_t* const clear;
};

#ifdef DEBUG
const char __assert_size_amk_inverter[(sizeof(EmulationAmkInverter_h)==sizeof(struct EmulationAmkInverter_t))?1:-1]; 
#endif /* ifdef DEBUG */

#define POPULATE_MEX_ENGINE(self, mex, engine)\
{\
  struct AMK_Actual_Values_1* s_w = &self->o_engines[engine].amk_data_1;\
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

static void _send_data_engine(struct EmulationAmkInverter_t* const restrict p_self, const uint16_t can_id) 
{
  CanMessage mex = {0};
  can_obj_can1_h_t o = {0};

  switch (can_id)
  {
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
  mex.message_size = pack_message_can1(&o, can_id,&mex.full_word);
  hardware_write_can(p_self->p_can_node_inverter, &mex);
}

static int _inverter_compute_internal_status(struct EmulationAmkInverter_t* const restrict p_self)
{
  uint8_t precharge_ready=1;

  FOR_EACH_ENGINE(engine)
  {
    if (!p_self->o_engines[engine].amk_data_1.AMK_STATUS.AMK_bDcOn)
    {
      precharge_ready=0;
      break;
    }
  }
  if (precharge_ready)
  {
    gpio_set_low(&p_self->o_precharge_init);
    if (!p_self->o_precharge_started_at)
    {
      p_self->o_precharge_started_at = timer_time_now();
    }

    ACTION_ON_FREQUENCY(p_self->o_precharge_started_at, 5 SECONDS)
    {
      p_self->o_precharge_started_at = timer_time_now();
      FOR_EACH_ENGINE(engine)
      {
        struct AMK_Actual_Values_1* p_val = &p_self->o_engines[engine].amk_data_1;
        p_val->AMK_STATUS.AMK_bQuitDcOn=1;
        gpio_set_low(&p_self->o_precharge_done);
      }
    }
  }
  else
  {
    p_self->o_precharge_started_at =0;
    gpio_set_high(&p_self->o_precharge_init);
    gpio_set_high(&p_self->o_precharge_done);
  }


  return 0;
}

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
static void _amk_inverter_update_data(struct EmulationAmkInverter_t* const restrict p_self)
{
  CanMessage mex = {0};
  can_obj_can1_h_t o1 ={0};
  const uint8_t rf_signal = gpio_read_state(&p_self->o_rf);

  if (!hardware_mailbox_read(p_self->p_recv_mailbox_vcu, &mex))
  {
    unpack_message_can1(&o1, mex.id,mex.full_word, mex.message_size, 0);
    switch (mex.id)
    {
      case CAN_ID_VCUINVFL:
        UPDATE_DATA(p_self->o_engines[FRONT_LEFT].amk_data_1, o1.can_0x184_VCUInvFL,rf_signal);
        break;
      case CAN_ID_VCUINVFR:
        UPDATE_DATA(p_self->o_engines[FRONT_RIGHT].amk_data_1, o1.can_0x185_VCUInvFR,rf_signal);
        break;
      case CAN_ID_VCUINVRL:
        UPDATE_DATA(p_self->o_engines[REAR_LEFT].amk_data_1, o1.can_0x188_VCUInvRL,rf_signal);
        break;
      case CAN_ID_VCUINVRR:
        UPDATE_DATA(p_self->o_engines[REAR_RIGHT].amk_data_1, o1.can_0x189_VCUInvRR,rf_signal);
        break;
    }
  }

  if (!rf_signal)
  {
    FOR_EACH_ENGINE(engine)
    {
      p_self->o_engines[engine].amk_data_1.AMK_STATUS.AMK_bEnable =0;
      p_self->o_engines[engine].amk_data_1.AMK_MagnetizingCurrent =0;
      p_self->o_engines[engine].amk_data_1.AMK_TorqueCurrent = 0;
      p_self->o_engines[engine].amk_data_1.AMK_ActualVelocity =0;
    }
  }
}

static int _car_amk_inverter_update(void* args)
{
  union AmkInverter_h_t_conv conv = {args};
  struct EmulationAmkInverter_t* const restrict p_self = conv.clear;

  while (p_self->o_running)
  {
    _amk_inverter_update_data(p_self);
    _inverter_compute_internal_status(p_self);
    ACTION_ON_FREQUENCY(p_self->o_last_sent_at, 50 MILLIS)
    {
      _send_data_engine(p_self, CAN_ID_INVERTERFR1);
      _send_data_engine(p_self, CAN_ID_INVERTERFL1);
      _send_data_engine(p_self, CAN_ID_INVERTERRL1);
      _send_data_engine(p_self, CAN_ID_INVERTERRR1);
    }
  }
  return 0;
}



//public

int8_t car_amk_inverter_start(EmulationAmkInverter_h* self)
{
  union AmkInverter_h_t_conv conv = {self};
  struct EmulationAmkInverter_t* const restrict p_self = conv.clear;
  memset(self, 0, sizeof(*self));

  p_self->p_can_node_inverter = hardware_init_new_external_node(CAN_INVERTER);
  if(!p_self->p_can_node_inverter)
  {
    return -1;
  }

  // FL: 388,       0000000110000100
  // FR: 389,       0000000110000101
  // RL: 392,       0000000110001000
  // RR: 393,       0000000110001001
  //
  // filter id:384, 0000000110000000
  // mask: 65520,   1111111111110000
  const uint16_t filter_id = CAN_ID_VCUINVFL & CAN_ID_VCUINVFR & CAN_ID_VCUINVRL & CAN_ID_VCUINVRR;
  const uint16_t filter_mask = (~0) ^ 15;

  printf("requesting mailbox amk inv\n");
  p_self->p_recv_mailbox_vcu =
    hardware_get_mailbox(p_self->p_can_node_inverter, FIFO_BUFFER, filter_id, filter_mask , 8);

  if (!p_self->p_recv_mailbox_vcu) {
    return -2;
  }

  if(hardware_init_gpio(&p_self->o_precharge_init , GPIO_AIR_PRECHARGE_INIT)<0)
  {
    return -3;
  }

  if(hardware_init_gpio(&p_self->o_precharge_done, GPIO_AIR_PRECHARGE_DONE)<0)
  {
    return -4;
  }

  if(hardware_init_read_permission_gpio(&p_self->o_rf, GPIO_INVERTER_RF_SIGNAL)<0)
  {
    return -5;
  }

  p_self->o_running=1;
  thrd_create(&p_self->o_thread, _car_amk_inverter_update, self);
  return 0;
}

  void 
car_amk_inverter_reset(EmulationAmkInverter_h* self)
{
  union AmkInverter_h_t_conv conv = {self};
  struct EmulationAmkInverter_t* const restrict p_self = conv.clear;
  FOR_EACH_ENGINE(engine){
    memset(&p_self->o_engines[engine].amk_data_1, 0,
        sizeof(p_self->o_engines[engine].amk_data_1));
    memset(&p_self->o_engines[engine].amk_data_2, 0,
        sizeof(p_self->o_engines[engine].amk_data_2));
  }
}

int8_t car_amk_inverter_set_attribute(EmulationAmkInverter_h* self,
    const enum INVERTER_ATTRIBUTE attribute, const uint8_t engine,
    const int64_t value)
{
  union AmkInverter_h_t_conv conv = {self};
  struct EmulationAmkInverter_t* const restrict p_self = conv.clear;
  struct amk_engines* p_engine = &p_self->o_engines[engine];

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

int8_t car_amk_inverter_set_engine_value(EmulationAmkInverter_h* self,
    const enum ENGINE_STATUS status, const uint8_t engine, 
    const float value)
{
  union AmkInverter_h_t_conv conv = {self};
  struct EmulationAmkInverter_t* const restrict p_self = conv.clear;
  struct amk_engines* p_engine = &p_self->o_engines[engine];

  if (!gpio_read_state(&p_self->o_rf))
  {
    return -1;
  }

  FOR_EACH_ENGINE(engine)
  {
    if (!p_self->o_engines[engine].amk_data_1.AMK_STATUS.AMK_bEnable)
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

enum PRECHARGE_STATUS car_amk_inverter_precharge_status(const EmulationAmkInverter_h* const restrict self)
{
  const union AmkInverter_h_t_conv_const conv = {self};
  const struct EmulationAmkInverter_t* const restrict p_self = conv.clear;
  return 
    (gpio_read_state(&p_self->o_precharge_init.gpio_read_permission) << 0) |
    (gpio_read_state(&p_self->o_precharge_done.gpio_read_permission) << 1);
}

void car_amk_inverter_stop(EmulationAmkInverter_h* self)
{
  union AmkInverter_h_t_conv conv = {self};
  struct EmulationAmkInverter_t* const restrict p_self = conv.clear;

  printf("stopping amk inverter\n");
  p_self->o_running=0;
  hardware_init_new_external_node_destroy(p_self->p_can_node_inverter);
  hardware_free_mailbox_can(&p_self->p_recv_mailbox_vcu);

  return;
}

void car_amk_inverter_emergency_shutdown(EmulationAmkInverter_h* self)
{
  union AmkInverter_h_t_conv conv = {self};
  struct EmulationAmkInverter_t* const restrict p_self = conv.clear;
  gpio_set_high(&p_self->o_precharge_init);
  gpio_set_high(&p_self->o_precharge_done);
}
