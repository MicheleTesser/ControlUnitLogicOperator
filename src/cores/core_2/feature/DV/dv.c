#include "dv.h"
#include "../../../core_utility/core_utility.h"
#include "../../../../lib/raceup_board/raceup_board.h"
#include "../../../../lib/board_dbc/dbc/out_lib/can3/can3.h"
#include "../../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#include "../../../core_utility/running_status/running_status.h"
#include "../../../core_utility/mission_reader/mission_reader.h"
#include "../../../core_utility/rtd_assi_sound/rtd_assi_sound.h"
#include "res/res.h"
#include "ebs/ebs.h"
#include "speed/speed.h"
#include "../dv_driver_input/dv_driver_input.h"
#include "./steering_wheel_alg/stw_alg.h"
#include <stdint.h>
#include <string.h>

//private

enum AS_STATUS{
  AS_OFF =1,
  AS_READY,
  AS_DRIVING,
  AS_FINISHED,
  AS_EMERGENCY,

  __NUM_OF_AS_STATUS__
};

enum DV_CAR_STATUS{
  DV_CAR_STATUS_OFF=0,
  DV_CAR_STATUS_READY,
  DV_CAR_STATUS_ERROR,

  __NUM_OF_DV_CAR_STATUS__,
};

//INFO: copied from .dbc can2 CarMissionStatus
enum MISSION_STATUS{
  MISSION_NOT_RUNNING=0,
  MISSION_RUNNING,
  MISSION_FINISHED,
};

struct Dv_t{
  enum AS_STATUS status;
  enum DV_CAR_STATUS dv_car_status;
  enum MISSION_STATUS o_dv_mission_status;
  uint8_t sound_start;
  time_var_microseconds driving_last_time_on;
  EmergencyNode_h emergency_node;
  time_var_microseconds emergency_last_time_on;
  time_var_microseconds emergency_sound_last_time_on;
  DvRes_h dv_res;
  DvEbs_h dv_ebs;
  DvSpeed_h dv_speed;
  GpioRead_h gpio_air_precharge_init;
  GpioRead_h gpio_air_precharge_done;
  RtdAssiSound_h o_assi_sound;
  GpioPwm_h gpio_ass_light_blue;
  GpioPwm_h gpio_ass_light_yellow;
  CarMissionReader_h* p_mission_reader;
  const DvDriverInput_h* dv_driver_input;
  struct CanMailbox* p_send_car_dv_car_status_mailbox;
  struct CanMailbox* p_mailbox_recv_mision_status;
  struct CanMailbox* p_mailbox_send_mission_can_2;
};

enum DV_EMERGENCY{
  EMERENGENCY_DV_EMERGENCY =0,

  __NUM_OF_DV_EMERGENCIES__
};

union Dv_h_t_conv{
  struct Dv_h* const hidden;
  struct Dv_t* const clear;
};

#ifdef DEBUG
char __assert_size_dv[(sizeof(Dv_h) == sizeof(struct Dv_t))? 1:-1];
char __assert_align_dv[(_Alignof(Dv_h) == _Alignof(struct Dv_t))? 1:-1];
#endif // DEBUG

//INFO: check dbc of can3 in message DV_system_status

static uint8_t _sdc_closed(const struct Dv_t* const restrict self)
{
  return  gpio_read_state(&self->gpio_air_precharge_init) &&
    gpio_read_state(&self->gpio_air_precharge_done) &&
    EmergencyNode_is_emergency_state(&self->emergency_node);
}

static void _update_dv_status(struct Dv_t* const restrict self, const enum AS_STATUS status)
{
  if (status == AS_EMERGENCY)
  {
    self->dv_car_status = DV_CAR_STATUS_ERROR;
    EmergencyNode_raise(&self->emergency_node, EMERENGENCY_DV_EMERGENCY);
  }
  else
  {
    EmergencyNode_solve(&self->emergency_node, EMERENGENCY_DV_EMERGENCY);
  }
  if (status==AS_OFF)
  {
    self->dv_car_status = DV_CAR_STATUS_OFF;
  }
  else
  {
    self->dv_car_status = DV_CAR_STATUS_READY;
  }
  self->status = status;
}

/*
 * AS Off -> off
 * AS Ready -> yellow continuous
 * AS Driving -> yellow flashing (100 MILLIS)
 * AS Emergency -> blue flashing (100 MILLIS)
 * AS Finished -> blue continuous
 */
static int8_t _dv_update_led(struct Dv_t* const restrict self)
{
  GpioPwm_h* const restrict gpio_light_blue = &self->gpio_ass_light_blue;
  GpioPwm_h* const restrict gpio_light_yellow = &self->gpio_ass_light_yellow;

  ACTION_ON_FREQUENCY(self->emergency_sound_last_time_on, 8 SECONDS)
  {
    rtd_assi_sound_stop(&self->o_assi_sound);
  }

  switch (self->status)
  {
    case AS_OFF:
      hardware_write_gpio_pwm(gpio_light_blue,0);
      hardware_write_gpio_pwm(gpio_light_yellow,0);
      rtd_assi_sound_stop(&self->o_assi_sound);
      break;
    case AS_READY:
      hardware_write_gpio_pwm(gpio_light_blue,0);
      hardware_write_gpio_pwm(gpio_light_yellow,100);
      rtd_assi_sound_stop(&self->o_assi_sound);
      res_start_time_go(&self->dv_res);
      break;
    case AS_DRIVING:
      hardware_write_gpio_pwm(gpio_light_blue,0);
      hardware_write_gpio_pwm(gpio_light_yellow, 50);
      rtd_assi_sound_stop(&self->o_assi_sound);
      break;
    case AS_EMERGENCY:
      hardware_write_gpio_pwm(gpio_light_yellow,0);
      hardware_write_gpio_pwm(gpio_light_blue,50);
      if (!self->sound_start) {
        self->sound_start =1;
        rtd_assi_sound_start(&self->o_assi_sound);
        self->emergency_sound_last_time_on = timer_time_now();
      }
      break;
    case AS_FINISHED:
      rtd_assi_sound_stop(&self->o_assi_sound);
      hardware_write_gpio_pwm(gpio_light_blue,0);
      hardware_write_gpio_pwm(gpio_light_yellow,0);
      break;
    case __NUM_OF_AS_STATUS__:
    default:
      return -1;

  }

  return 0;
}

//INFO: Flowchart T 14.9.2
static int8_t _dv_update_status(struct Dv_t* const restrict self)
{
  const float current_speed = dv_speed_get(&self->dv_speed);
  const float driver_brake = dv_driver_input_get_brake(self->dv_driver_input);
  const enum RUNNING_STATUS giei_status = global_running_status_get();

  if (EmergencyNode_is_emergency_state(&self->emergency_node)) {
    self->status = AS_EMERGENCY;
    EmergencyNode_raise(&self->emergency_node, EMERENGENCY_DV_EMERGENCY);
  }

  if (!ebs_on(&self->dv_ebs)) 
  {
    if (car_mission_reader_get_current_mission(self->p_mission_reader) > CAR_MISSIONS_HUMAN &&
        ebs_asb_consistency_check(&self->dv_ebs) == EBS_SUCCESS &&
        giei_status >= TS_READY)
    {
      if (giei_status == RUNNING)
      {
        _update_dv_status(self, AS_DRIVING);
      }
      else if(driver_brake > 50)
      {
        _update_dv_status(self, AS_READY);
      }
      else
      {
        _update_dv_status(self, AS_OFF);
      }
    }
    else
    {
      _update_dv_status(self, AS_OFF);
    }
  }


  else if (self->o_dv_mission_status == MISSION_FINISHED && !current_speed && _sdc_closed(self))
  {
    _update_dv_status(self, AS_FINISHED);
  }
  else
  {
    _update_dv_status(self, AS_EMERGENCY);
  }
  return 0;
}

//public

int8_t dv_class_init(Dv_h* const restrict self ,
    CarMissionReader_h* const restrict p_mission_reader,
    DvDriverInput_h* const restrict driver )
{
  union Dv_h_t_conv conv = {self};
  struct Dv_t* const p_self = conv.clear;
  struct CanNode* can_node;

  memset(p_self, 0, sizeof(*p_self));

  if(dv_stw_alg_init()<0)
  {
    return -1;
  }

  if(res_class_init(&p_self->dv_res)<0)
  {
    return -2;
  }
  if(ebs_class_init(&p_self->dv_ebs) <0)
  {
    return -3;
  }
  if (dv_speed_init(&p_self->dv_speed)<0) {
    return -4;
  }

  if (hardware_init_gpio_pwm(&p_self->gpio_ass_light_yellow, PWM_GPIO_ASSI_LIGHT_YELLOW)<0)
  {
    return -5;
  }

  if (hardware_init_gpio_pwm(&p_self->gpio_ass_light_blue, PWM_GPIO_ASSI_LIGHT_BLU)<0)
  {
    return -6;
  }

  rtd_assi_sound_init(&p_self->o_assi_sound);

  if (hardware_init_read_permission_gpio(&p_self->gpio_air_precharge_init,
        GPIO_AIR_PRECHARGE_INIT)<0)
  {
    return -5;   
  }

  if (hardware_init_read_permission_gpio(&p_self->gpio_air_precharge_done,
        GPIO_AIR_PRECHARGE_DONE)<0)
  {
    return -6;   
  }

  if (EmergencyNode_init(&p_self->emergency_node)<0)
  {
    return -7;
  }

  ACTION_ON_CAN_NODE(CAN_DV,can_node)
  {
    p_self->p_send_car_dv_car_status_mailbox =
    hardware_get_mailbox_single_mex(
        can_node,
        SEND_MAILBOX,
        CAN_ID_DV_CARSTATUS,
        message_dlc_can3(CAN_ID_DV_CARSTATUS));
  }
  if (!p_self->p_send_car_dv_car_status_mailbox)
  {
    return -8;
  }

  ACTION_ON_CAN_NODE(CAN_DV,can_node)
  {
    p_self->p_mailbox_recv_mision_status=
    hardware_get_mailbox_single_mex(
        can_node,
        RECV_MAILBOX,
        CAN_ID_DV_MISSION,
        message_dlc_can3(CAN_ID_DV_MISSION));
  }

  if (!p_self->p_mailbox_recv_mision_status)
  {
    hardware_free_mailbox_can(&p_self->p_send_car_dv_car_status_mailbox);
    return -9;
  }

  ACTION_ON_CAN_NODE(CAN_GENERAL, can_node)
  {
    p_self->p_mailbox_send_mission_can_2=
    hardware_get_mailbox_single_mex(
        can_node,
        RECV_MAILBOX,
        CAN_ID_CARMISSIONSTATUS,
        message_dlc_can3(CAN_ID_DV_MISSION));
  }

  if (!p_self->p_mailbox_send_mission_can_2)
  {
    hardware_free_mailbox_can(&p_self->p_send_car_dv_car_status_mailbox);
    hardware_free_mailbox_can(&p_self->p_mailbox_recv_mision_status);
    return -10;
  }

  p_self->dv_car_status=DV_CAR_STATUS_OFF;
  p_self->o_dv_mission_status = MISSION_NOT_RUNNING;
  p_self->dv_driver_input = driver;
  p_self->p_mission_reader = p_mission_reader;

  return 0;
}

int8_t dv_update(Dv_h* const restrict self)
{
  union Dv_h_t_conv conv = {self};
  struct Dv_t* const p_self = conv.clear;
  const uint8_t input_stw_alg= 1;
  uint8_t output_stw_alg = 1;
  CanMessage mex = {0};
  uint64_t mission_status_payload = 0;
  can_obj_can3_h_t o3 = {0};
  can_obj_can2_h_t o2 = {0};

  if(dv_speed_update(&p_self->dv_speed)<0)return -1;
  if(ebs_update(&p_self->dv_ebs)<0) return -2;

  if (car_mission_reader_get_current_mission(p_self->p_mission_reader) > CAR_MISSIONS_HUMAN)
  {
    if (hardware_mailbox_read(p_self->p_mailbox_recv_mision_status, &mex))
    {
      unpack_message_can3(&o3, mex.id, mex.full_word, mex.message_size, timer_time_now());
      p_self->o_dv_mission_status = o3.can_0x07e_DV_Mission.Mission_status;
    }
  
    if (p_self->status == AS_DRIVING)
    {
      dv_stw_alg_compute(&input_stw_alg, &output_stw_alg); //TODO: not yet implemented
    }

    if(_dv_update_status(p_self)<0)return -4;
    if(_dv_update_led(p_self)<0) return -5;
    
    o2.can_0x071_CarMissionStatus.Mission = car_mission_reader_get_current_mission(p_self->p_mission_reader);
    o2.can_0x071_CarMissionStatus.MissionStatus = p_self->o_dv_mission_status;

    pack_message_can2(&o2, CAN_ID_CARMISSIONSTATUS, &mission_status_payload);

    hardware_mailbox_send(p_self->p_send_car_dv_car_status_mailbox, mission_status_payload);
    hardware_mailbox_send(p_self->p_send_car_dv_car_status_mailbox, p_self->dv_car_status);
  }

  return 0;
}

#ifdef DEBUG

enum DV_CAR_STATUS debug_dv_get_car_status(Dv_h* const restrict self)
{
  union Dv_h_t_conv conv = {self};
  struct Dv_t* const p_self = conv.clear;
  return p_self->dv_car_status;
}

enum AS_STATUS debug_dv_get_as_status(Dv_h* const restrict self)
{
  union Dv_h_t_conv conv = {self};
  struct Dv_t* const p_self = conv.clear;
  return p_self->status;
}
#else

enum DV_CAR_STATUS debug_dv_get_car_status(Dv_h* const restrict self __attribute__(( __unused__)))
{
  while (1) {}
}

enum AS_STATUS debug_dv_get_as_status(Dv_h* const restrict self __attribute__(( __unused__)))
{
  while (1) {}
}

#endif /* ifdef DEBUG */

