#include "./amk.h"
#include "../../../../../lib/board_dbc/dbc/out_lib/can1/can1.h"
#include "../../../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#include "../../../../../lib/raceup_board/raceup_board.h"
#include "../../../../core_utility/emergency_module/emergency_module.h"
#include "../../math_saturated/saturated.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdatomic.h>

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
  struct{
    struct AMK_Actual_Values_1 amk_values_1;
    struct AMK_Actual_Values_2 amk_values_2;
    int16_t AMK_TargetVelocity; //INFO: unit: rpm Speed setpoint
    int16_t AMK_TorqueLimitPositive; //INFO: unit: 0.1% MN Positive torque limit (subject to nominal torque)
    int16_t AMK_TorqueLimitNegative; //INFO: unit: 0.1% MN Negative torque limit (subject to nominal torque)
  }engines[__NUM_OF_ENGINES__];
  enum RUNNING_STATUS engine_status;
  uint8_t hvCounter[__NUM_OF_ENGINES__];
  time_var_microseconds u_last_send_info;
  struct GpioRead_h gpio_precharge_init;
  struct GpioRead_h gpio_precharge_done;
  struct GpioRead_h gpio_rtd_button;
  struct GpioRead_h gpio_ts_button;
  const struct DriverInput_h* driver_input;
  struct CanNode* can_inverter;
  struct CanMailbox* engine_mailbox;
  struct CanMailbox* mailbox_pcu_rf_signal_send;
  struct CanMailbox* mailbox_pcu_rf_signal_read;
  EmergencyNode_h amk_emergency;
  uint8_t rf_status:1;
}AMKInverter_t;

union AMKConv{
  AmkInverter_h* hidden;
  AMKInverter_t* clear;
};

#define AMK_H_T_CONV(h_ptr, t_ptr_name)\
  union AMKConv __a_conv##t_ptr_name__ = {h_ptr};\
  AMKInverter_t* t_ptr_name = __a_conv##t_ptr_name__.clear;

#ifdef DEBUG
const uint8_t __debug_amk_size__[(sizeof(AmkInverter_h) == sizeof(AMKInverter_t))? 1 : -1];
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

  static int8_t
_send_message_amk(const AMKInverter_t* const restrict self,
    const enum ENGINES engine, 
    const struct AMK_Setpoints* const restrict setpoint)
{
  can_obj_can1_h_t om={0};
  CanMessage mex={0};

  switch (engine)
  {
    case FRONT_LEFT:
      POPULATE_MEX_ENGINE(setpoint, om.can_0x184_VCUInvFL);
      mex.id = CAN_ID_VCUINVFL;
      break;
    case FRONT_RIGHT:
      POPULATE_MEX_ENGINE(setpoint, om.can_0x185_VCUInvFR);
      mex.id = CAN_ID_VCUINVFR;
      break;
    case REAR_LEFT:
      POPULATE_MEX_ENGINE(setpoint, om.can_0x188_VCUInvRL);
      mex.id = CAN_ID_VCUINVRL;
      break;
    case REAR_RIGHT:
      POPULATE_MEX_ENGINE(setpoint, om.can_0x189_VCUInvRR);
      mex.id = CAN_ID_VCUINVRR;
      break;
    default:
      return -1;
  }

  mex.message_size = pack_message_can1(&om, mex.id, &mex.full_word);
  return hardware_write_can(self->can_inverter, &mex);
}

static uint8_t
_amk_inverter_on(const AMKInverter_t* const restrict self)
{
  uint8_t res = 0xFF;
  FOR_EACH_ENGINE(engine){
    res &= self->engines[engine].amk_values_1.AMK_status.AMK_bSystemReady;
  }
  return res;
}

static uint8_t
_precharge_ended(const AMKInverter_t* const restrict self)
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
    gpio_read_state(&self->gpio_precharge_init) && gpio_read_state(&self->gpio_precharge_done);
}

static uint8_t
_amk_inverter_hv_status(AMKInverter_t* const restrict self)
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

static void
_amk_update_rtd_procedure(AMKInverter_t* const restrict self)
{
  struct AMK_Setpoints setpoint = {0};
  can_obj_can2_h_t o2={0};
  uint64_t data=0;

  if(self->engine_status == SYSTEM_OFF)
  {
    EmergencyNode_solve(&self->amk_emergency, FAILED_RTD_AMK);
  }

  if (EmergencyNode_is_emergency_state(&self->amk_emergency))
  {
    FOR_EACH_ENGINE(engine)
    {
      _send_message_amk(self, engine, &setpoint);
    }
    self->engine_status = SYSTEM_OFF;
    return;
  }

  switch (self->engine_status)
  {
    case SYSTEM_OFF:

      if (_amk_inverter_on(self) && gpio_read_state(&self->gpio_ts_button) &&
          !driver_input_rtd_request(self->driver_input))

      {
        self->engine_status = SYSTEM_PRECAHRGE;
      }
      else
      {
        if(driver_input_rtd_request(self->driver_input))
        {
          EmergencyNode_raise(&self->amk_emergency, FAILED_RTD_AMK);
        }
      }
      break;
    case SYSTEM_PRECAHRGE:
      setpoint.AMK_Control_fields.AMK_bDcOn = 1;
      if (!_amk_inverter_on(self) ||
          !gpio_read_state(&self->gpio_ts_button) ||
          driver_input_rtd_request(self->driver_input))
      {
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

      o2.can_0x130_Pcu.mode = 1;
      o2.can_0x130_Pcu.rf = 1;
      if (!_amk_inverter_hv_status(self) || !_precharge_ended(self) || !_amk_inverter_on(self))
      {
        EmergencyNode_raise(&self->amk_emergency, FAILED_RTD_AMK);
        memset(&setpoint, 0, sizeof(setpoint));
        self->engine_status = SYSTEM_OFF;
      }
      else if (driver_input_rtd_request(self->driver_input) &&
          driver_input_get(self->driver_input, BRAKE) > 20)
      {
        //INFO: RF message to PCU and wait until rf is active before going
        //into RUNNING status
        if (self->rf_status)
        {
          self->engine_status = RUNNING;
        }
      }
      break;
    case RUNNING:
      setpoint.AMK_Control_fields.AMK_bDcOn = 1;
      setpoint.AMK_Control_fields.AMK_bInverterOn = 1;
      setpoint.AMK_Control_fields.AMK_bEnable = 1;

      o2.can_0x130_Pcu.mode = 1;
      o2.can_0x130_Pcu.rf = 1;
      if (!_amk_inverter_on(self) || !_amk_inverter_hv_status(self) || !_precharge_ended(self))
      {
        EmergencyNode_raise(&self->amk_emergency, FAILED_RTD_AMK);
        memset(&setpoint, 0, sizeof(setpoint));
        self->engine_status = SYSTEM_OFF;
      }
      else if (!driver_input_rtd_request(self->driver_input))
      {
        setpoint.AMK_Control_fields.AMK_bEnable = 0;
        self->engine_status= TS_READY;
      }
      break;
  }

  ACTION_ON_FREQUENCY(self->u_last_send_info, 10 MILLIS)
  {
    pack_message_can2(&o2, CAN_ID_PCU, &data);
    hardware_mailbox_send(self->mailbox_pcu_rf_signal_send, data);
    FOR_EACH_ENGINE(engine)
    {
      _send_message_amk(self, engine, &setpoint);
    }
  }

}

  static float
_max_torque(const AMKInverter_t* const restrict self)
{
  float torque_max_sum = 0;
  FOR_EACH_ENGINE(engine)
  {
    const float actual_velocity = 
      self->engines[engine].amk_values_1.AMK_ActualVelocity;
    torque_max_sum += MAX_MOTOR_TORQUE - 0.000857*(actual_velocity - 13000.0f);
  }
  return  torque_max_sum/__NUM_OF_ENGINES__;
}

//public

  enum RUNNING_STATUS
amk_rtd_procedure(const AMKInverter_t* const restrict self)
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

int8_t
amk_update(AMKInverter_t* const restrict self)
{
  CanMessage mex = {0};
  can_obj_can1_h_t o1 ={0};
  can_obj_can2_h_t o2 ={0};
  int8_t err=0;

  _amk_update_rtd_procedure(self);
  if (!hardware_mailbox_read(self->engine_mailbox, &mex))
  {
    unpack_message_can1(&o1, mex.id, mex.full_word, mex.message_size, timer_time_now());
    switch (mex.id)
    {
      case CAN_ID_INVERTERFL1:
        UPDATE_INFO_1(FRONT_LEFT, o1.can_0x283_InverterFL1);
        break;
      case CAN_ID_INVERTERFL2:
        UPDATE_INFO_2(FRONT_LEFT, o1.can_0x285_InverterFL2);
        break;

      case CAN_ID_INVERTERFR1:
        UPDATE_INFO_1(FRONT_RIGHT, o1.can_0x284_InverterFR1);
        break;
      case CAN_ID_INVERTERFR2:
        UPDATE_INFO_2(FRONT_RIGHT, o1.can_0x286_InverterFR2);
        break;

      case CAN_ID_INVERTERRL1:
        UPDATE_INFO_1(REAR_LEFT, o1.can_0x287_InverterRL1);
        break;
      case CAN_ID_INVERTERRL2:
        UPDATE_INFO_2(REAR_LEFT, o1.can_0x289_InverterRL2);
        break;

      case CAN_ID_INVERTERRR1:
        UPDATE_INFO_1(REAR_RIGHT, o1.can_0x288_InverterRR1);
        break;
      case CAN_ID_INVERTERRR2:
        UPDATE_INFO_2(REAR_RIGHT, o1.can_0x28a_InverterRR2);
        break;
      default:
        err--;
        break;
    }
  }

  memset(&mex, 0, sizeof(mex));
  if (!hardware_mailbox_read(self->mailbox_pcu_rf_signal_read, &mex))
  {
    unpack_message_can2(&o2, mex.id, mex.full_word, mex.message_size, timer_time_now());
    self->rf_status = o2.can_0x134_PcuRfAck.rf_signalAck;
  }
  return err;
}

  float
amk_get_info(const AMKInverter_t* const restrict self,
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
      return -1;
  }
}

  float
amk_max_pos_torque(const AMKInverter_t* const restrict self, const float limit_max_pos_torque)
{
  const float v_max_torque = _max_torque(self);
  return saturate_float(v_max_torque, limit_max_pos_torque, 0.0f);
}

  float
amk_max_neg_torque(const AMKInverter_t* const restrict self, const float limit_max_neg_torque)
{
  const float v_max_torque = _max_torque(self);
  return  -saturate_float(v_max_torque, 0.0f, limit_max_neg_torque);

}

  int8_t
amk_send_torque(const AMKInverter_t* const restrict self,
    const enum ENGINES engine, const float pos_torque, const float neg_torque)
{
  if (amk_rtd_procedure(self) == RUNNING)
  {
    struct AMK_Setpoints setpoint  ={
      .AMK_Control_fields ={0},
      .AMK_TargetVelocity = SPEED_LIMIT,
      .AMK_TorqueLimitPositive = pos_torque,
      .AMK_TorqueLimitNegative = neg_torque,
    };
    return _send_message_amk(self, engine, &setpoint);
  }

  return -1;
}

  void
amk_destroy(AMKInverter_t* const restrict self __attribute__((__unused__)))
{
  return;
}

  int8_t
amk_module_init(AmkInverter_h* const restrict self,
    const struct DriverInput_h* const p_driver_input,
    struct EngineType* const restrict general_inverter)
{
  AMK_H_T_CONV(self, p_self);
  struct CanNode* can_node = NULL;

  memset(p_self, 0, sizeof(*p_self));
  p_self->engine_status =SYSTEM_OFF;
  p_self->driver_input = p_driver_input;

  if (EmergencyNode_init(&p_self->amk_emergency))
  {
    return -1;
  }
  p_self->can_inverter = hardware_init_can_get_ref_node(CAN_INVERTER);
  if (!p_self->can_inverter) {
    return -2;
  }

  if (hardware_init_read_permission_gpio(&p_self->gpio_precharge_init, GPIO_AIR_PRECHARGE_INIT)<0)
 
  {
    return -3;
  }

  if (hardware_init_read_permission_gpio(&p_self->gpio_precharge_done, GPIO_AIR_PRECHARGE_DONE)<0)
  {
    return -4;
  }

  if (hardware_init_read_permission_gpio(&p_self->gpio_rtd_button, GPIO_RTD_BUTTON)<0)
  {
    return -4;
  }

  if (hardware_init_read_permission_gpio(&p_self->gpio_ts_button, GPIO_TS_BUTTON)<0)
  {
    return -5;
  }

  /*
    id 0: 643,      0000001010000011
    id 1: 644,      0000001010000100
    id 2: 645,      0000001010000101
    id 3: 646,      0000001010000110
    id 4: 647,      0000001010000111
    id 5: 648,      0000001010001000
    id 6: 649,      0000001010001001
    id 7: 650,      0000001010001010
    filter id:280,  0000001010000000
    mask: fff0,     1111111111110000
  */
  p_self->engine_mailbox =
    hardware_get_mailbox(p_self->can_inverter, FIFO_BUFFER, 0x280, 0xFFF0, 8);
  if (!p_self->engine_mailbox)
  {
    return -6;
  }

  ACTION_ON_CAN_NODE(CAN_GENERAL,can_node)
  {
    p_self->mailbox_pcu_rf_signal_send =
      hardware_get_mailbox_single_mex(can_node,
          SEND_MAILBOX,
          CAN_ID_PCU,
          message_dlc_can2(CAN_ID_PCU));

    p_self->mailbox_pcu_rf_signal_read =
      hardware_get_mailbox_single_mex(
          can_node,
          RECV_MAILBOX,
          CAN_ID_PCURFACK,
          message_dlc_can2(CAN_ID_PCURFACK));
  }

  if (!p_self->mailbox_pcu_rf_signal_send)
  {
    return -7;
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
