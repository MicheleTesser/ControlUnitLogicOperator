#include "./amk.h"
#include "../../../../../lib/board_dbc/dbc/out_lib/can1/can1.h"
#include "../../../../../lib/raceup_board/raceup_board.h"
#include "../../../../core_utility/emergency_module/emergency_module.h"
#include "../../math_saturated/saturated.h"
#include <stdint.h>
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
    uint8_t bSystemReady: 1; //INFO: System ready (SBM)
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
  time_var_microseconds enter_precharge_phase;
  struct GpioRead_h gpio_precharge_init;
  struct GpioRead_h gpio_precharge_done;
  struct GpioRead_h gpio_rtd_button;
  struct EmergencyNode_h amk_emergency;
  uint8_t hvCounter[__NUM_OF_ENGINES__];
  const struct DriverInput_h* driver_input;
  const struct CanNode* can_inverter;
  const struct CanMailbox *engine_mailbox[__NUM_OF_ENGINES__ * 2];
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
engine.NegTorq = amk_stop->AMK_TorqueLimitNegative;\
engine.PosTorq = amk_stop->AMK_TorqueLimitPositive;\
engine.TargetVel = amk_stop->AMK_TargetVelocity;\
engine.AMK_bInverterOn = setpoint->AMK_Control_fields.AMK_bInverterOn;\
engine.AMK_bDcOn = setpoint->AMK_Control_fields.AMK_bDcOn;\
engine.AMK_bEnable = setpoint->AMK_Control_fields.AMK_bEnable;\
engine.AMK_bErrorReset = setpoint->AMK_Control_fields.AMK_bErrorReset;\
engine.AMK_bReservedEnd = 0;\

static int8_t send_message_amk(const AMKInverter_t* const restrict self,
    const enum ENGINES engine, 
    const struct AMK_Setpoints* const restrict setpoint)
{
  int8_t err=0;
  can_obj_can1_h_t om;
  CanMessage mex;
  switch (engine) {
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
      goto invalid_mex_type;
  }

  mex.message_size = pack_message_can1(&om, mex.id, &mex.full_word);
  return hardware_write_can(self->can_inverter, &mex);

invalid_mex_type:
  err--;

  return err;
}

static uint8_t amk_disable_inverter(const AMKInverter_t* const restrict self)
{
  FOR_EACH_ENGINE({
      struct AMK_Setpoints setpoint;
      memset(&setpoint, 0, sizeof(setpoint));
      send_message_amk(self, index_engine, &setpoint);
      })
  return 0;
}


static uint8_t amk_inverter_on(const AMKInverter_t* const restrict self)
{
  uint8_t res = 0xFF;
  FOR_EACH_ENGINE({
      res &= self->engines[index_engine].amk_values_1.AMK_status.bSystemReady;
      })
  return res;
}

static inline uint8_t precharge_ended(const AMKInverter_t* const restrict self)
{
  return
    gpio_read_state(&self->gpio_precharge_init) && gpio_read_state(&self->gpio_precharge_done);
}

static void amk_shut_down_power(AMKInverter_t* const restrict self)
{
  amk_disable_inverter(self);
  self->engine_status= SYSTEM_OFF;
}

static uint8_t amk_inverter_precharge_started(AMKInverter_t* const restrict self)
{
  FOR_EACH_ENGINE({
      if (self->engines[index_engine].amk_values_1.AMK_status.AMK_bDcOn) {
      return 1;
      }
      })
  return 0;
}

static uint8_t amk_inverter_hv_status(AMKInverter_t* const restrict self)
{
  const uint8_t HV_TRAP = 50;
  uint8_t res = 0;

  FOR_EACH_ENGINE({
      const uint8_t AMK_bQuitDcOn = 
      self->engines[index_engine].amk_values_1.AMK_status.AMK_bQuitDcOn;
      if (!(AMK_bQuitDcOn) && (self->hvCounter[index_engine] < HV_TRAP))
      {
      self->hvCounter[index_engine]++;
      }
      else if (AMK_bQuitDcOn || (self->hvCounter[index_engine] >= HV_TRAP))
      {
      res |= AMK_bQuitDcOn;
      self->hvCounter[index_engine] = 0;
      }
      })


  return res;
}

static uint8_t amk_activate_control(const AMKInverter_t* const restrict self)
{
  FOR_EACH_ENGINE({
      struct AMK_Setpoints setpoint;
      memset(&setpoint, 0, sizeof(setpoint));
      setpoint.AMK_Control_fields.AMK_bDcOn =1;
      setpoint.AMK_Control_fields.AMK_bEnable=1;
      setpoint.AMK_Control_fields.AMK_bInverterOn=1;
      send_message_amk(self, index_engine, &setpoint);
      })
  return 0;
}

static enum RUNNING_STATUS amk_rtd_procedure(AMKInverter_t* const restrict self)
{
  if(self->engine_status == SYSTEM_OFF){
    EmergencyNode_raise(&self->amk_emergency, FAILED_RTD_AMK);
  }

  if (EmergencyNode_is_emergency_state(&self->amk_emergency)) {
    amk_disable_inverter(self);
    self->engine_status = SYSTEM_OFF;
    return self->engine_status;
  }

  switch (self->engine_status)
  {
    case SYSTEM_OFF:
      if (amk_inverter_on(self) && 
          amk_inverter_precharge_started(self) &&
          !driver_input_rtd_request(self->driver_input))
      {
        self->engine_status = SYSTEM_PRECAHRGE;
      }
      else
      {
        amk_shut_down_power(self);
        if(driver_input_rtd_request(self->driver_input))
        {
          EmergencyNode_raise(&self->amk_emergency, FAILED_RTD_AMK);
        }
      }
      break;
    case SYSTEM_PRECAHRGE:
      if (!amk_inverter_on(self) ||
          !amk_inverter_precharge_started(self) ||
          driver_input_rtd_request(self->driver_input))
      {
        EmergencyNode_raise(&self->amk_emergency, FAILED_RTD_AMK);
        amk_shut_down_power(self);
        self->engine_status = SYSTEM_OFF;
      }
      else if (precharge_ended(self))
      {
        amk_activate_control(self);
        self->engine_status = TS_READY;
      }
      break;
    case TS_READY:
      if (!amk_inverter_hv_status(self) || !precharge_ended(self) || !amk_inverter_on(self))
      {
        amk_shut_down_power(self);
        EmergencyNode_raise(&self->amk_emergency, FAILED_RTD_AMK);
        self->engine_status = SYSTEM_OFF;
        break;
      }
      if (driver_input_rtd_request(self->driver_input))
      {
        self->engine_status = RUNNING;
      }
      break;
    case RUNNING:
      if (!amk_inverter_on(self) || !amk_inverter_hv_status(self) || !precharge_ended(self))
      {
        amk_shut_down_power(self);
        EmergencyNode_raise(&self->amk_emergency, FAILED_RTD_AMK);
        self->engine_status = SYSTEM_OFF;
      }
      else if (driver_input_rtd_request(self->driver_input))
      {
        amk_disable_inverter(self);
        self->engine_status= TS_READY;
      }
      break;
  }
  return self->engine_status;
}

static int8_t amk_update(AMKInverter_t* const restrict self __attribute__((__unused__)))
{
  return 0;
}

static float amk_get_info(const AMKInverter_t* const restrict self,
    const enum ENGINES engine, const enum ENGINE_INFO info)
{
  if (engine == __NUM_OF_ENGINES__) {
    return -1;
  }
  switch (info) {
    case ENGINE_VOLTAGE:
      return self->engines[engine].amk_values_1.AMK_TorqueCurrent;
    case ENGINE_RPM:
      return self->engines[engine].amk_values_1.AMK_ActualVelocity;
    case TARGET_VELOCITY:
      return self->engines[engine].AMK_TargetVelocity;
      break;
    default:
      return -1;
  }
}

static float max_torque(const AMKInverter_t* const restrict self)
{
  float torque_max_sum = 0;
  FOR_EACH_ENGINE({
      const float actual_velocity = 
      self->engines[index_engine].amk_values_1.AMK_ActualVelocity;
      torque_max_sum += MAX_MOTOR_TORQUE - 0.000857*(actual_velocity - 13000.0f);
      })
  return  torque_max_sum/4;
}

  static float
amk_max_pos_torque(const AMKInverter_t* const restrict self, const float limit_max_pos_torque)
{
  const float v_max_torque = max_torque(self);
  return saturate_float(v_max_torque, limit_max_pos_torque, 0.0f);
}

  static float
amk_max_neg_torque(const AMKInverter_t* const restrict self, const float limit_max_neg_torque)
{
  const float v_max_torque = max_torque(self);
  return  -saturate_float(v_max_torque, 0.0f, limit_max_neg_torque);

}

static int8_t amk_send_torque(const AMKInverter_t* const restrict self,
    const enum ENGINES engine, const float pos_torque, const float neg_torque)
{
  struct AMK_Setpoints setpoint  ={
    .AMK_Control_fields ={0},
    .AMK_TargetVelocity = SPEED_LIMIT,
    .AMK_TorqueLimitPositive = pos_torque,
    .AMK_TorqueLimitNegative = neg_torque,
  };
  return send_message_amk(self, engine, &setpoint);

}

static void amk_destroy(AMKInverter_t* const restrict self __attribute__((__unused__)))
{
  return;
}



//public
int8_t amk_module_init(AmkInverter_h* const restrict self,
    const struct DriverInput_h* const p_driver_input,
    struct EngineType* const restrict general_inverter)
{
  AMK_H_T_CONV(self, p_self);
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
