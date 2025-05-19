#include "dv.h"
#include "res/res.h"
#include "ebs/ebs.h"
#include "steering_wheel_alg/stw_alg.h"
#include "../../../../lib/raceup_board/raceup_board.h"
#include "../../../core_utility/core_utility.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "../../../../lib/board_dbc/dbc/out_lib/can3/can3.h"
#include "../../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#pragma GCC diagnostic pop 

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
  enum AS_STATUS m_status;
  enum DV_CAR_STATUS m_dv_car_status;
  enum MISSION_STATUS m_dv_mission_status;
  uint8_t m_sound_start;
  time_var_microseconds m_driving_last_time_on; 
  EmergencyNode_h m_emergency_node;
  time_var_microseconds m_emergency_last_time_on; 
  time_var_microseconds m_emergency_sound_last_time_on; 
  time_var_microseconds m_embeed_last_alive; 
  DvRes_h m_dv_res; 
  DvEbs_h m_dv_ebs;
  AsNode_h m_as_node; 
  GpioRead_h m_gpio_air_precharge_init; 
  GpioRead_h m_gpio_air_precharge_done; 
  RtdAssiSound_h m_assi_sound;
  GpioPwm_h m_gpio_ass_light_blue; 
  GpioPwm_h m_gpio_ass_light_yellow; 
  Imu_h m_imu; 
  CarMissionReader_h* p_mission_reader; 
  SharedMessageReader_h m_recv_mission_status; 
  SharedMessageReader_h m_recv_embedded_alive;
  struct CanMailbox* p_send_car_m_dv_car_status_mailbox; 
  struct CanMailbox* p_mailbox_send_mission_can_2; 
};

enum DV_EMERGENCY{
  EMERENGENCY_DV_EMERGENCY =0,
  EMERGENCY_DV_EMBEDDED_OFF,

  __NUM_OF_DV_EMERGENCIES__
};

union Dv_h_t_conv{
  Dv_h* const hidden;
  struct Dv_t* const clear;
};

#ifdef DEBUG
char __assert_size_dv[(sizeof(Dv_h) == sizeof(struct Dv_t))? 1:-1];
char __assert_align_dv[(_Alignof(Dv_h) == _Alignof(struct Dv_t))? 1:-1];
#endif // DEBUG

//INFO: check dbc of can3 in message DV_system_status

static inline uint8_t _sdc_closed(const struct Dv_t* const restrict self)
{
  return  gpio_read_state(&self->m_gpio_air_precharge_init) &&
    gpio_read_state(&self->m_gpio_air_precharge_done) &&
    as_node_get_status(&self->m_as_node);
}

static void _dv_set_status(struct Dv_t* const restrict self, const enum AS_STATUS m_status)
{
  if (m_status == AS_EMERGENCY)
  {
    self->m_dv_car_status = DV_CAR_STATUS_ERROR;
    EmergencyNode_raise(&self->m_emergency_node, EMERENGENCY_DV_EMERGENCY);
  }
  else
  {
    EmergencyNode_solve(&self->m_emergency_node, EMERENGENCY_DV_EMERGENCY);
  }
  if (m_status==AS_OFF || m_status == AS_FINISHED)
  {
    self->m_dv_car_status = DV_CAR_STATUS_OFF;
  }
  else
  {
    self->m_dv_car_status = DV_CAR_STATUS_READY;
  }
  self->m_status = m_status;
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
  GpioPwm_h* const restrict gpio_light_blue = &self->m_gpio_ass_light_blue;
  GpioPwm_h* const restrict gpio_light_yellow = &self->m_gpio_ass_light_yellow;

  ACTION_ON_FREQUENCY(self->m_emergency_sound_last_time_on, get_tick_from_millis(8000))
  {
    rtd_assi_sound_stop(&self->m_assi_sound);
  }

  switch (self->m_status)
  {
    case AS_OFF:
      hardware_write_gpio_pwm(gpio_light_blue,0);
      hardware_write_gpio_pwm(gpio_light_yellow,0);
      rtd_assi_sound_stop(&self->m_assi_sound);
      break;
    case AS_READY:
      hardware_write_gpio_pwm(gpio_light_blue,0);
      hardware_write_gpio_pwm(gpio_light_yellow,100);
      rtd_assi_sound_stop(&self->m_assi_sound);
      res_start_time_go(&self->m_dv_res);
      break;
    case AS_DRIVING:
      hardware_write_gpio_pwm(gpio_light_blue,0);
      hardware_write_gpio_pwm(gpio_light_yellow, 50);
      rtd_assi_sound_stop(&self->m_assi_sound);
      break;
    case AS_EMERGENCY:
      hardware_write_gpio_pwm(gpio_light_yellow,0);
      hardware_write_gpio_pwm(gpio_light_blue,50);
      if (!self->m_sound_start)
      {
        self->m_sound_start =1;
        rtd_assi_sound_start(&self->m_assi_sound);
        self->m_emergency_sound_last_time_on = timer_time_now();
      }
      break;
    case AS_FINISHED:
      rtd_assi_sound_stop(&self->m_assi_sound);
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
  const float current_speed = car_speed_get();
  const enum RUNNING_STATUS giei_status = global_running_status_get();

  if (EmergencyNode_is_emergency_state(&self->m_emergency_node))
  {
    return 0;
  }

  if (as_node_get_status(&self->m_as_node) && ebs_asb_consistency_check(&self->m_dv_ebs))
  {
    switch (giei_status)
    {
      case SYSTEM_OFF:
      case SYSTEM_PRECAHRGE:
      case TS_READY:
        _dv_set_status(self, AS_READY);
        break;
      case RUNNING:
        _dv_set_status(self, AS_DRIVING);
        break;
    }
  }
  else if (self->m_dv_mission_status == MISSION_FINISHED && !current_speed && !_sdc_closed(self))
  {
    _dv_set_status(self, AS_FINISHED);
  }
  else
  {
    _dv_set_status(self, AS_EMERGENCY);
  }
  return 0;
}

//public

int8_t dv_class_init(Dv_h* const restrict self ,
    CarMissionReader_h* const restrict p_mission_reader)
{
  union Dv_h_t_conv conv = {self};
  struct Dv_t* const p_self = conv.clear;
  struct CanNode* can_node;

  memset(p_self, 0, sizeof(*p_self));

  if(dv_stw_alg_init()<0)
  {
    return -1;
  }

  if(res_class_init(&p_self->m_dv_res)<0)
  {
    return -2;
  }
  if(ebs_class_init(&p_self->m_dv_ebs) <0)
  {
    return -3;
  }
  if (imu_init(&p_self->m_imu)<0) {
    return -4;
  }

  if (hardware_init_gpio_pwm(&p_self->m_gpio_ass_light_yellow, PWM_GPIO_ASSI_LIGHT_YELLOW)<0)
  {
    return -5;
  }

  if (hardware_init_gpio_pwm(&p_self->m_gpio_ass_light_blue, PWM_GPIO_ASSI_LIGHT_BLU)<0)
  {
    return -6;
  }

  rtd_assi_sound_init(&p_self->m_assi_sound);

  if (hardware_init_read_permission_gpio(&p_self->m_gpio_air_precharge_init,
        GPIO_AIR_PRECHARGE_INIT)<0)
  {
    return -5;   
  }

  if (hardware_init_read_permission_gpio(&p_self->m_gpio_air_precharge_done,
        GPIO_AIR_PRECHARGE_DONE)<0)
  {
    return -6;   
  }

  if (EmergencyNode_init(&p_self->m_emergency_node)<0)
  {
    return -7;
  }

  if (as_node_init(&p_self->m_as_node,p_mission_reader)<0)
  {
    return -9;
  }

  ACTION_ON_CAN_NODE(CAN_DV,can_node)
  {
    p_self->p_send_car_m_dv_car_status_mailbox =
    hardware_get_mailbox_single_mex(
        can_node,
        SEND_MAILBOX,
        CAN_ID_DV_CARSTATUS,
        message_dlc_can3(CAN_ID_DV_CARSTATUS));
  }
  if (!p_self->p_send_car_m_dv_car_status_mailbox)
  {
    return -10;
  }
  
  if (shared_message_reader_init(&p_self->m_recv_mission_status, SHARED_MEX_DV_MISSION))
  {
    hardware_free_mailbox_can(&p_self->p_send_car_m_dv_car_status_mailbox);
    return -11;
  }

  ACTION_ON_CAN_NODE(CAN_GENERAL, can_node)
  {
    p_self->p_mailbox_send_mission_can_2=
    hardware_get_mailbox_single_mex(
        can_node,
        RECV_MAILBOX,
        CAN_ID_CARMISSIONSTATUS,
        message_dlc_can3(CAN_ID_CARMISSIONSTATUS));
  }

  if (!p_self->p_mailbox_send_mission_can_2)
  {
    hardware_free_mailbox_can(&p_self->p_send_car_m_dv_car_status_mailbox);
    return -12;
  }

  if (shared_message_reader_init(&p_self->m_recv_embedded_alive, SHARED_MEX_EMBEDDEDALIVECHECK))
  {
    hardware_free_mailbox_can(&p_self->p_send_car_m_dv_car_status_mailbox);
    hardware_free_mailbox_can(&p_self->p_mailbox_send_mission_can_2);
    return -13;
  }


  p_self->m_dv_car_status=DV_CAR_STATUS_OFF;
  p_self->m_dv_mission_status = MISSION_NOT_RUNNING;
  p_self->p_mission_reader = p_mission_reader;

  EmergencyNode_raise(&p_self->m_emergency_node, 1); //INFO: debug per tronici remove

  return 0;
}

int8_t dv_update(Dv_h* const restrict self)
{
  union Dv_h_t_conv conv = {self};
  struct Dv_t* const p_self = conv.clear;
  const uint8_t input_stw_alg= 1;
  uint8_t output_stw_alg = 1;
  uint64_t mex = 0;
  uint64_t mission_status_payload = 0;
  can_obj_can3_h_t o3 = {0};
  can_obj_can2_h_t o2 = {0};

  switch (car_mission_reader_get_current_mission(p_self->p_mission_reader))
  {
    case CAR_MISSIONS_NONE:
      p_self->m_dv_mission_status = MISSION_NOT_RUNNING;
      _dv_set_status(p_self, AS_OFF);
      break;
    case CAR_MISSIONS_HUMAN:
      _dv_set_status(p_self, AS_OFF);
      break;
    case CAR_MISSIONS_DV_SKIDPAD:
    case CAR_MISSIONS_DV_AUTOCROSS:
    case CAR_MISSIONS_DV_TRACKDRIVE:
    case CAR_MISSIONS_DV_EBS_TEST:
    case CAR_MISSIONS_DV_INSPECTION:

      if (imu_update(&p_self->m_imu)<0) return -1;
      if (ebs_update(&p_self->m_dv_ebs)<0) return -2;
      if (as_node_update(&p_self->m_as_node)) return -3;

      if (shared_message_read(&p_self->m_recv_embedded_alive, &mex))
      {
        p_self->m_embeed_last_alive = timer_time_now(); 
        EmergencyNode_solve(&p_self->m_emergency_node, EMERGENCY_DV_EMBEDDED_OFF);
      }

      if ((timer_time_now() - p_self->m_embeed_last_alive) > get_tick_from_millis(500))
      {
        p_self->m_dv_mission_status = MISSION_NOT_RUNNING;
        p_self->m_status = AS_OFF;
        EmergencyNode_raise(&p_self->m_emergency_node, EMERGENCY_DV_EMBEDDED_OFF);
      }

      if (shared_message_read_unpack_can3(&p_self->m_recv_mission_status, &o3)>0)
      {
        p_self->m_dv_mission_status = o3.can_0x07e_DV_Mission.Mission_status;
      }


      if (p_self->m_status == AS_DRIVING)
      {
        //TODO: add check continuous monitoring
        dv_stw_alg_compute(&input_stw_alg, &output_stw_alg); //TODO: not yet implemented
      }


      o2.can_0x071_CarMissionStatus.Mission = car_mission_reader_get_current_mission(p_self->p_mission_reader);
      o2.can_0x071_CarMissionStatus.MissionStatus = p_self->m_dv_mission_status;
      o2.can_0x071_CarMissionStatus.AsStatus = p_self->m_status;

      pack_message_can2(&o2, CAN_ID_CARMISSIONSTATUS, &mission_status_payload);
      hardware_mailbox_send(p_self->p_send_car_m_dv_car_status_mailbox, mission_status_payload);
      break;
    case __NUM_OF_CAR_MISSIONS__:
      p_self->m_dv_mission_status = MISSION_NOT_RUNNING;
      p_self->m_status = AS_OFF;
      break;
      if(_dv_update_status(p_self)<0)return -4;
  }

  if(_dv_update_led(p_self)<0) return -5;

  return 0;
}
