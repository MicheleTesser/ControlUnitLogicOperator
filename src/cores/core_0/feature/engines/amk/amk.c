#include "./amk.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "../../../../../lib/board_dbc/dbc/out_lib/can1/can1.h"
#include "../../../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#pragma GCC diagnostic pop 
#include "../../../../../lib/raceup_board/raceup_board.h"
#include "../../../../core_utility/core_utility.h"
#include "../../math_saturated/saturated.h"
#include "../../../../core_1/feature/log/external_log_variables/external_log_variables.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//INFO: doc/amk_datasheet.pdf page 61

//private

#define MAX_MOTOR_TORQUE            21.0f
#define SPEED_LIMIT                 18000           //INFO: Typical value: 15000

enum AMK_EMERGENCY {
  FAILED_RTD_AMK =0,

  __NUM_OF_AMK_EMERGENCY__
};

struct AMK_Actual_Values_1{
  struct{
    uint8_t b_reserve; //INFO: Reserved
    uint8_t AMK_bSystemReady: 1; //INFO: System ready (SBM)
    uint8_t AMK_bError :1; //INFO: Error
    uint8_t AMK_bWarn :1; //INFO: Warning
    uint8_t AMK_bQuitDcOn :1; //INFO: HV activation acknowledgment
    uint8_t AMK_bDcOn :1; //INFO: HV activation level
    uint8_t AMK_bQuitInverterOn:1; //INFO: Controller enable acknowledgment
    uint8_t AMK_bInverterOn:1; //INFO: Controller enable level
    uint8_t AMK_bDerating :1; //INFO: Derating (torque limitation active)
  }AMK_status;
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

typedef struct Inverter{
  CarSpeedMut_h m_car_speed;
  uint8_t rf_status:1;
  uint8_t started_precharge:1;
  uint8_t hvCounter[__NUM_OF_ENGINES__];
  struct{
    struct AMK_Actual_Values_1 amk_values_1;
    struct AMK_Actual_Values_2 amk_values_2;
    int16_t AMK_TargetVelocity; // INFO: unit: rpm Speed setpoint
    int16_t AMK_TorqueLimitPositive; //INFO: unit: 0.1% MN Positive torque limit (subject to nominal torque)
    int16_t AMK_TorqueLimitNegative; //INFO: unit: 0.1% MN Negative torque limit (subject to nominal torque)
  }engines[__NUM_OF_ENGINES__];
  time_var_microseconds u_last_send_info; 
  GpioRead_h gpio_precharge_init; 
  GpioRead_h gpio_precharge_done; 
  GpioRead_h gpio_rtd_button; 
  enum RUNNING_STATUS engine_status; 
  const DriverInput_h* driver_input; 
  struct CanNode* can_inverter; 
  struct{
    struct CanMailbox* p_recv_mailbox_inverter_1; 
    struct CanMailbox* p_recv_mailbox_inverter_2; 
  }m_engines_mailbox[__NUM_OF_ENGINES__];
  struct CanMailbox* mailbox_pcu_rf_signal_send;
  struct CanMailbox* mailbox_pcu_rf_signal_read;

  EmergencyNode_h __attribute__((aligned(4))) amk_emergency; 
}AMKInverter_t;

union AMKConv{
  AmkInverter_h* hidden;
  AMKInverter_t* clear;
};

#define AMK_H_T_CONV(h_ptr, t_ptr_name)\
  union AMKConv __a_conv##t_ptr_name__ = {h_ptr};\
  AMKInverter_t* t_ptr_name = __a_conv##t_ptr_name__.clear;

#ifdef DEBUG
// char (*aa)[sizeof(AMKInverter_t)]=1;
const uint8_t __debug_amk_size__[(sizeof(AmkInverter_h) == sizeof(AMKInverter_t))? 1 : -1];
const uint8_t __debug_amk_align__[(_Alignof(AmkInverter_h) == _Alignof(AMKInverter_t))? 1 : -1];
#endif /* ifdef DEBUG */

#define POPULATE_MEX_ENGINE(amk_stop,engine)\
  engine.AMK_bReserved = 0;\
engine.AMK_bDcOn = amk_stop->AMK_Control_fields.AMK_bDcOn;\
engine.AMK_bInverterOn = amk_stop->AMK_Control_fields.AMK_bInverterOn;\
engine.AMK_bEnable = amk_stop->AMK_Control_fields.AMK_bEnable;\
engine.AMK_bErrorReset = amk_stop->AMK_Control_fields.AMK_bErrorReset;\
engine.AMK_bReservedEnd = 0;\
engine.NegTorq = amk_stop->AMK_TorqueLimitNegative;\
engine.PosTorq = amk_stop->AMK_TorqueLimitPositive;\
engine.TargetVel = amk_stop->AMK_TargetVelocity;\

static int8_t _send_message_amk(const AMKInverter_t* const restrict self,
    const enum ENGINES engine, 
    const struct AMK_Setpoints* const restrict setpoint)
{
  can_obj_can1_h_t om={0};
  CanMessage mex={0};

  switch (engine)
  {
    case FRONT_LEFT:
      POPULATE_MEX_ENGINE(setpoint, om.can_0x189_VCUInvFL);
      mex.id = CAN_ID_VCUINVFL;
      break;
    case FRONT_RIGHT:
      POPULATE_MEX_ENGINE(setpoint, om.can_0x188_VCUInvFR);
      mex.id = CAN_ID_VCUINVFR;
      break;
    case REAR_LEFT:
      POPULATE_MEX_ENGINE(setpoint, om.can_0x185_VCUInvRL);
      mex.id = CAN_ID_VCUINVRL;
      break;
    case REAR_RIGHT:
      POPULATE_MEX_ENGINE(setpoint, om.can_0x184_VCUInvRR);
      mex.id = CAN_ID_VCUINVRR;
      break;
    default:
      SET_TRACE(CORE_0);
      return -1;
  }

  mex.message_size = (uint8_t) pack_message_can1(&om, mex.id, &mex.full_word);
  return hardware_write_can(self->can_inverter, &mex);
}

static uint8_t _amk_inverter_on(const AMKInverter_t* const restrict self)
{
  uint8_t res = 0;
  FOR_EACH_ENGINE(engine){
    res |= self->engines[engine].amk_values_1.AMK_status.AMK_bSystemReady;
  }
  return res;
}

static uint8_t _precharge_ended(const AMKInverter_t* const restrict self)
{
  uint8_t res =0;
  FOR_EACH_ENGINE(engine)
  {
    //HACK: in emulation environment checking that all the engine have done the precharge phase 
    //caused a weird behaviour where at some point, after the precharge is completed, for one engine 
    //the the hv was cut off and so also the precharge. This only happen in release mode 
    //and it's most certainly caused by an optimization problem in the emulation of the inverter
    //or in the module of the amk. To make things work for now i'm checking if at least one engine
    //has completed its precharge phase.
    res |= self->engines[engine].amk_values_1.AMK_status.AMK_bQuitDcOn;
  }
  return 
    res &&
    self->started_precharge &&
    !gpio_read_state(&self->gpio_precharge_init) &&
    gpio_read_state(&self->gpio_precharge_done);
}

static uint8_t _precharge_started(const AMKInverter_t* const restrict self)
{
  uint8_t res =0;
  FOR_EACH_ENGINE(engine)
  {
    //HACK: in emulation environment checking that all the engine have done the precharge phase 
    //caused a weird behaviour where at some point, after the precharge is completed, for one engine 
    //the the hv was cut off and so also the precharge. This only happen in release mode 
    //and it's most certainly caused by an optimization problem in the emulation of the inverter
    //or in the module of the amk. To make things work for now i'm checking if at least one engine
    //has completed its precharge phase.
    res |= self->engines[engine].amk_values_1.AMK_status.AMK_bQuitDcOn;
  }
  return res &&
    !gpio_read_state(&self->gpio_precharge_init) && !gpio_read_state(&self->gpio_precharge_done);
}

static uint8_t _hv_on_no_ack(const AMKInverter_t* const restrict self)__attribute__((__unused__));
static uint8_t _hv_on_no_ack(const AMKInverter_t* const restrict self)
{
  uint8_t res =0;
  FOR_EACH_ENGINE(engine)
  {
    //HACK: in emulation environment checking that all the engine have done the precharge phase 
    //caused a weird behaviour where at some point, after the precharge is completed, for one engine 
    //the the hv was cut off and so also the precharge. This only happen in release mode 
    //and it's most certainly caused by an optimization problem in the emulation of the inverter
    //or in the module of the amk. To make things work for now i'm checking if at least one engine
    //has completed its precharge phase.
    res |= self->engines[engine].amk_values_1.AMK_status.AMK_bDcOn;
  }
  return res;
}

static uint8_t _amk_inverter_hv_status(AMKInverter_t* const restrict self)
{
  const uint8_t HV_TRAP = 50;
  uint8_t res = 0;

  FOR_EACH_ENGINE(engine)
  {
    const uint8_t AMK_bQuitDcOn = 
      self->engines[engine].amk_values_1.AMK_status.AMK_bQuitDcOn;
    if (!(AMK_bQuitDcOn) && (self->hvCounter[engine] < HV_TRAP))
    {
      self->hvCounter[engine]++;
    }
    else if (AMK_bQuitDcOn || (self->hvCounter[engine] >= HV_TRAP))
    {
      res |= AMK_bQuitDcOn;
      self->hvCounter[engine] = 0;
    }
  }


  return res;
}

static uint8_t _amk_inverter_ready_to_go(AMKInverter_t* const restrict self)
{
  const uint8_t HV_TRAP = 50;
  uint8_t res = 0;

  FOR_EACH_ENGINE(engine)
  {
    const uint8_t AMK_bQuitInverterOn = 
      self->engines[engine].amk_values_1.AMK_status.AMK_bQuitInverterOn;
    if (!(AMK_bQuitInverterOn) && (self->hvCounter[engine] < HV_TRAP))
    {
      self->hvCounter[engine]++;
    }
    else if (AMK_bQuitInverterOn || (self->hvCounter[engine] >= HV_TRAP))
    {
      res |= AMK_bQuitInverterOn;
      self->hvCounter[engine] = 0;
    }
  }


  return res;
}

static void _amk_update_rtd_procedure(AMKInverter_t* const restrict self)
{
  struct AMK_Setpoints setpoint = {0};
  can_obj_can2_h_t o2={0};
  uint64_t data=0;

  if(self->engine_status == SYSTEM_OFF)
  {
    EmergencyNode_solve(&self->amk_emergency, FAILED_RTD_AMK);
  }

  switch (self->engine_status)
  {
    case SYSTEM_OFF:
      self->started_precharge = 0;
      if (_amk_inverter_on(self))
      {
        setpoint.AMK_Control_fields.AMK_bDcOn = 1;
      }

      if (!EmergencyNode_is_emergency_state(&self->amk_emergency) &&
          _amk_inverter_on(self) &&
          _hv_on_no_ack(self) &&
          _precharge_started(self) &&
          !giei_driver_input_rtd_request(self->driver_input))
      {
        self->engine_status = SYSTEM_PRECAHRGE;
      }
      else
      {
        if(giei_driver_input_rtd_request(self->driver_input)||
            EmergencyNode_is_emergency_state(&self->amk_emergency))
        {
          SET_TRACE(CORE_0);
          EmergencyNode_raise(&self->amk_emergency, FAILED_RTD_AMK);
          memset(&setpoint, 0, sizeof(setpoint));
          self->engine_status = SYSTEM_OFF;
        }
      }
      break;
    case SYSTEM_PRECAHRGE:
      self->started_precharge = 1;
      setpoint.AMK_Control_fields.AMK_bDcOn = 1;
      if (!_amk_inverter_on(self) ||
          giei_driver_input_rtd_request(self->driver_input))
      {
        SET_TRACE(CORE_0);
        EmergencyNode_raise(&self->amk_emergency, FAILED_RTD_AMK);
        memset(&setpoint, 0, sizeof(setpoint));
        self->engine_status = SYSTEM_OFF;
      }
      else if (_precharge_ended(self))
      {
        self->engine_status = TS_READY;
      }
      break;
    case TS_READY:
      setpoint.AMK_Control_fields.AMK_bDcOn = 1;
      setpoint.AMK_Control_fields.AMK_bInverterOn = 1;

      if (!_amk_inverter_hv_status(self) || !_precharge_ended(self) || !_amk_inverter_on(self))
      {
        SET_TRACE(CORE_0);
        EmergencyNode_raise(&self->amk_emergency, FAILED_RTD_AMK);
        memset(&setpoint, 0, sizeof(setpoint));
        self->engine_status = SYSTEM_OFF;
      }
      else if (giei_driver_input_rtd_request(self->driver_input) &&
          giei_driver_input_get(self->driver_input, BRAKE) > 20)
      {
        o2.can_0x130_Pcu.mode = 1;
        o2.can_0x130_Pcu.rf = giei_driver_input_rtd_request(self->driver_input) & 1;

        //INFO: RF message to PCU and wait until rf is active before going
        //into RUNNING status
        if (self->rf_status)
        {
          setpoint.AMK_Control_fields.AMK_bDcOn = 1;
          setpoint.AMK_Control_fields.AMK_bInverterOn = 1;
          setpoint.AMK_Control_fields.AMK_bEnable = 1;
          if (_amk_inverter_ready_to_go(self))
          {
            self->engine_status = RUNNING;
          }
        }
      }
      break;
    case RUNNING:
      if (!_amk_inverter_ready_to_go(self) ||
          !_amk_inverter_hv_status(self) ||
          !_precharge_ended(self)||
          !_amk_inverter_on(self))
      {
        SET_TRACE(CORE_0);
        EmergencyNode_raise(&self->amk_emergency, FAILED_RTD_AMK);
        memset(&setpoint, 0, sizeof(setpoint));
        self->engine_status = SYSTEM_OFF;
      }
      else if (giei_driver_input_rtd_request(self->driver_input)<=0)
      {
        setpoint.AMK_Control_fields.AMK_bDcOn = 0;
        setpoint.AMK_Control_fields.AMK_bInverterOn = 0;
        setpoint.AMK_Control_fields.AMK_bEnable = 0;
        o2.can_0x130_Pcu.mode = 1;
        o2.can_0x130_Pcu.rf = 0;
        self->engine_status= SYSTEM_OFF;
      }
      else
      {
        return;
      }

      break;
  }

  ACTION_ON_FREQUENCY(self->u_last_send_info, get_tick_from_millis(10))
  {
    pack_message_can2(&o2, CAN_ID_PCU, &data);
    hardware_mailbox_send(self->mailbox_pcu_rf_signal_send, data);
    FOR_EACH_ENGINE(engine)
    {
      _send_message_amk(self, engine, &setpoint);
    }
  }

}

static float _max_torque(const AMKInverter_t* const restrict self)
{
  float torque_max_sum = 0;
  FOR_EACH_ENGINE(engine)
  {
    const float actual_velocity = 
      self->engines[engine].amk_values_1.AMK_ActualVelocity;
    torque_max_sum += MAX_MOTOR_TORQUE - 0.000857f*(actual_velocity - 13000.0f);
  }
  return  torque_max_sum/(float)__NUM_OF_ENGINES__;
}

static int8_t _share_var_engine(const AMKInverter_t* const restrict self, const enum ENGINES engine)
{
  uint8_t basic_id_start = AMK_STATUS_FL;
  uint8_t cursor =0;

  switch (engine)
  {
    case FRONT_LEFT:
      basic_id_start = AMK_STATUS_FL;
      break;
    case FRONT_RIGHT:
      basic_id_start = AMK_STATUS_FR;
      break;
    case REAR_LEFT:
      basic_id_start = AMK_STATUS_RL;
      break;
    case REAR_RIGHT:
      basic_id_start = AMK_STATUS_RR;
      break;
    default:
      SET_TRACE(CORE_0);
      return -1;
  }

#define SHARE_LOG_VAR(engine, var)\
  if(external_log_variables_store_pointer(&self->engines[engine].var,\
        basic_id_start + cursor)<0)\
  {\
    SET_TRACE(CORE_0);\
    return -9 + cursor;\
  }\
  ++cursor;

  //INFO: ORDER MATTER. For order look at shared_memory.h in AMK_SHARED_VARS
  SHARE_LOG_VAR(engine, amk_values_1.AMK_status);
  SHARE_LOG_VAR(engine, amk_values_1.AMK_ActualVelocity);
  SHARE_LOG_VAR(engine, amk_values_1.AMK_TorqueCurrent);
  SHARE_LOG_VAR(engine, amk_values_1.AMK_MagnetizingCurrent);
  SHARE_LOG_VAR(engine, amk_values_1.AMK_TorqueCurrent);
  SHARE_LOG_VAR(engine, amk_values_2.AMK_TempMotor);
  SHARE_LOG_VAR(engine, amk_values_2.AMK_TempInverter);
  SHARE_LOG_VAR(engine, amk_values_2.AMK_TempIGBT);
  SHARE_LOG_VAR(engine, amk_values_2.AMK_ErrorInfo);
  SHARE_LOG_VAR(engine, AMK_TorqueLimitPositive);
  SHARE_LOG_VAR(engine, AMK_TorqueLimitNegative);

  return 0;
}

//public

enum RUNNING_STATUS amk_rtd_procedure(const AMKInverter_t* const restrict self)
{
  return self->engine_status;
}

#define UPDATE_INFO_1(engine, mex)\
{\
  struct AMK_Actual_Values_1* val = &self->engines[engine].amk_values_1;\
  val->AMK_ActualVelocity = mex.ActualVelocity;\
  val->AMK_MagnetizingCurrent = mex.MagCurr;\
  val->AMK_TorqueCurrent = mex.Voltage;\
  val->AMK_status.AMK_bSystemReady= mex.SystemReady;\
  val->AMK_status.AMK_bDcOn = mex.HVOn;\
  val->AMK_status.AMK_bQuitDcOn = mex.HVOnAck;\
  val->AMK_status.AMK_bInverterOn = mex.InverterOn;\
  val->AMK_status.AMK_bQuitInverterOn = mex.InverterOnAck;\
  val->AMK_status.AMK_bDerating= mex.Derating;\
  val->AMK_status.AMK_bWarn = mex.Warning;\
  val->AMK_status.AMK_bError = mex.Error;\
}

#define UPDATE_INFO_2(engine, mex)\
{\
  struct AMK_Actual_Values_2* val = &self->engines[engine].amk_values_2;\
  val->AMK_ErrorInfo = mex.ErrorInfo;\
  val->AMK_TempIGBT = mex.TempIGBT;\
  val->AMK_TempInverter = mex.TempInv;\
  val->AMK_TempMotor = mex.TempMotor;\
}

int8_t amk_update(AMKInverter_t* const restrict self)
{
  int8_t err=0;
  CanMessage mex = {0};
  can_obj_can1_h_t o1 ={0};
  can_obj_can2_h_t o2 ={0};

  _amk_update_rtd_procedure(self);


#define READ_MAILBOX_INV(ENGINE, MEX_1, MEX_2)\
  if (hardware_mailbox_read(self->m_engines_mailbox[ENGINE].p_recv_mailbox_inverter_1, &mex))\
  {\
    if(unpack_message_can1(&o1, mex.id, mex.full_word, 8, timer_time_now())<0)\
    {\
      SET_TRACE(CORE_0);\
      EmergencyNode_raise(&self->amk_emergency, 1);\
    }\
    UPDATE_INFO_1(ENGINE, o1.MEX_1);\
  }\
  if (hardware_mailbox_read(self->m_engines_mailbox[ENGINE].p_recv_mailbox_inverter_2, &mex))\
  {\
    if(unpack_message_can1(&o1, mex.id, mex.full_word, 8, timer_time_now())<0)\
    {\
      SET_TRACE(CORE_0);\
      EmergencyNode_raise(&self->amk_emergency, 1);\
    }\
    UPDATE_INFO_2(ENGINE, o1.MEX_2);\
  }

  READ_MAILBOX_INV(FRONT_LEFT, can_0x288_InverterFL1, can_0x28a_InverterFL2);
  READ_MAILBOX_INV(FRONT_RIGHT, can_0x287_InverterFR1, can_0x289_InverterFR2);
  READ_MAILBOX_INV(REAR_LEFT, can_0x284_InverterRL1, can_0x286_InverterRL2);
  READ_MAILBOX_INV(REAR_RIGHT, can_0x283_InverterRR1, can_0x285_InverterRR2);

  const float engines_rpm[] = 
  {
    self->engines[FRONT_LEFT].AMK_TargetVelocity,
    self->engines[FRONT_RIGHT].AMK_TargetVelocity,
    self->engines[REAR_LEFT].AMK_TargetVelocity,
    self->engines[REAR_RIGHT].AMK_TargetVelocity,
  };
  if(car_speed_mut_update(&self->m_car_speed, engines_rpm)<0)
  {
    err--;
  }

  memset(&mex, 0, sizeof(mex));
  if (hardware_mailbox_read(self->mailbox_pcu_rf_signal_read, &mex))
  {
    unpack_message_can2(&o2, mex.id, mex.full_word, mex.message_size, timer_time_now());
    self->rf_status = o2.can_0x134_PcuRfAck.rf_signalAck;
  }
  return err;
}

float amk_get_info(const AMKInverter_t* const restrict self,
    const enum ENGINES engine, const enum ENGINE_INFO info)
{
  if (engine == __NUM_OF_ENGINES__) {
    return -1;
  }
  switch (info)
  {
    case ENGINE_VOLTAGE:
      return self->engines[engine].amk_values_1.AMK_TorqueCurrent;
    case ENGINE_RPM:
      return self->engines[engine].amk_values_1.AMK_ActualVelocity;
    case TARGET_VELOCITY:
      return self->engines[engine].AMK_TargetVelocity;
    default:
      SET_TRACE(CORE_0);
      return -1;
  }
}

float amk_max_pos_torque(const AMKInverter_t* const restrict self, const float limit_max_pos_torque)
{
  const float v_max_torque = _max_torque(self);
  return saturate_float(v_max_torque, limit_max_pos_torque, 0.0f);
}

float amk_max_neg_torque(const AMKInverter_t* const restrict self, const float limit_max_neg_torque)
{
  const float v_max_torque = _max_torque(self);
  return  -saturate_float(v_max_torque, 0.0f, limit_max_neg_torque);

}

int8_t amk_send_torque(const AMKInverter_t* const restrict self,
    const enum ENGINES engine, const float pos_torque, const float neg_torque)
{
  struct AMK_Setpoints setpoint  ={
    .AMK_Control_fields ={0},
    .AMK_TargetVelocity = 0,
    .AMK_TorqueLimitPositive = 0,
    .AMK_TorqueLimitNegative = 0,
  };
  (void)pos_torque;
  if (amk_rtd_procedure(self) == RUNNING)
  {
    setpoint.AMK_TargetVelocity = SPEED_LIMIT;
    setpoint.AMK_Control_fields.AMK_bDcOn=1;
    setpoint.AMK_Control_fields.AMK_bEnable=1;
    setpoint.AMK_Control_fields.AMK_bInverterOn =1;
    setpoint.AMK_TorqueLimitPositive = 21 * 100;
    setpoint.AMK_TorqueLimitNegative = (int16_t) neg_torque;
  }

  return _send_message_amk(self, engine, &setpoint);
}

void amk_destroy(AMKInverter_t* const restrict self __attribute__((__unused__)))
{
  return;
}

int8_t amk_module_init(AmkInverter_h* const restrict self,
    const DriverInput_h* const p_driver_input,
    struct EngineType* const restrict general_inverter)
{
  AMK_H_T_CONV(self, p_self);
  struct CanNode* can_node = NULL;

  memset(p_self, 0, sizeof(*p_self));
  p_self->engine_status =SYSTEM_OFF;
  p_self->driver_input = p_driver_input;

  if (EmergencyNode_init(&p_self->amk_emergency))
  {
    SET_TRACE(CORE_0);
    return -1;
  }
  p_self->can_inverter = hardware_init_can_get_ref_node(CAN_INVERTER);
  if (!p_self->can_inverter) {
    SET_TRACE(CORE_0);
    return -2;
  }

  if (car_speed_mut_init(&p_self->m_car_speed)<0)
  {
    SET_TRACE(CORE_0);
    return -3;
  }

  if (hardware_init_read_permission_gpio(&p_self->gpio_precharge_init, GPIO_AIR_PRECHARGE_INIT)<0)
 
  {
    SET_TRACE(CORE_0);
    return -4;
  }

  if (hardware_init_read_permission_gpio(&p_self->gpio_precharge_done, GPIO_AIR_PRECHARGE_DONE)<0)
  {
    SET_TRACE(CORE_0);
    return -5;
  }

  if (hardware_init_read_permission_gpio(&p_self->gpio_rtd_button, GPIO_RTD_BUTTON)<0)
  {
    SET_TRACE(CORE_0);
    return -6;
  }


#define INVERTER_MAILBOX(engine, MAILBOX, CAN_ID)\
  p_self->m_engines_mailbox[engine].MAILBOX =\
    hardware_get_mailbox_single_mex(\
        p_self->can_inverter,\
        RECV_MAILBOX,\
        CAN_ID,\
        message_dlc_can1(CAN_ID));\




  ACTION_ON_CAN_NODE(CAN_GENERAL,can_node)
  {
    p_self->mailbox_pcu_rf_signal_send =
      hardware_get_mailbox_single_mex(can_node,
          SEND_MAILBOX,
          CAN_ID_PCU,
          message_dlc_can2(CAN_ID_PCU));
  }

  if (!p_self->mailbox_pcu_rf_signal_send)
  {
    SET_TRACE(CORE_0);
    return -9;
  }

  ACTION_ON_CAN_NODE(CAN_GENERAL,can_node)
  {
    p_self->mailbox_pcu_rf_signal_read =
      hardware_get_mailbox_single_mex(
          can_node,
          RECV_MAILBOX,
          CAN_ID_PCURFACK,
          message_dlc_can2(CAN_ID_PCURFACK));
  }


  if (!p_self->mailbox_pcu_rf_signal_read)
  {
    hardware_free_mailbox_can(&p_self->mailbox_pcu_rf_signal_read);
    SET_TRACE(CORE_0);
    return -9;
  }

  INVERTER_MAILBOX(FRONT_LEFT, p_recv_mailbox_inverter_1, CAN_ID_INVERTERFL1)
  INVERTER_MAILBOX(FRONT_LEFT, p_recv_mailbox_inverter_2, CAN_ID_INVERTERFL2)

  INVERTER_MAILBOX(FRONT_RIGHT, p_recv_mailbox_inverter_1, CAN_ID_INVERTERFR1)
  INVERTER_MAILBOX(FRONT_RIGHT, p_recv_mailbox_inverter_2, CAN_ID_INVERTERFR2)

  INVERTER_MAILBOX(REAR_LEFT, p_recv_mailbox_inverter_1, CAN_ID_INVERTERRL1)
  INVERTER_MAILBOX(REAR_LEFT, p_recv_mailbox_inverter_2, CAN_ID_INVERTERRL2)

  INVERTER_MAILBOX(REAR_RIGHT, p_recv_mailbox_inverter_1, CAN_ID_INVERTERRR1)
  INVERTER_MAILBOX(REAR_RIGHT, p_recv_mailbox_inverter_2, CAN_ID_INVERTERRR2)

  int8_t err=0;
  FOR_EACH_ENGINE(engine)
  {
    if((err =_share_var_engine(p_self, engine))<0)
    {
      SET_TRACE(CORE_0);
      return -9 + err;
    }
  }

  general_inverter->update_f=amk_update;
  general_inverter->rtd_f=amk_rtd_procedure;
  general_inverter->get_info_f=amk_get_info;
  general_inverter->max_pos_torque_f=amk_max_pos_torque;
  general_inverter->max_neg_torque_f=amk_max_neg_torque;
  general_inverter->send_torque_f=amk_send_torque;
  general_inverter->destroy_f=amk_destroy;
  general_inverter->data = p_self;

  return 0;
}
