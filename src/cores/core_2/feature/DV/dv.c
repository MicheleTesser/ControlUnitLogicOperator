#include "dv.h"
#include "../../../core_utility/core_utility.h"
#include "../../../../lib/raceup_board/raceup_board.h"
#include "../../../../lib/board_dbc/dbc/out_lib/can3/can3.h"
#include "../../../core_utility/running_status/running_status.h"
#include "../mission/mission.h"
#include "res/res.h"
#include "asb/asb.h"
#include "speed/speed.h"
#include "../dv_driver_input/dv_driver_input.h"
#include "../mission/mission.h"
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

struct Dv_t{
  enum AS_STATUS status;
  enum DV_CAR_STATUS dv_car_status;
  uint8_t sound_start;
  time_var_microseconds driving_last_time_on;
  time_var_microseconds emergency_last_time_on;
  time_var_microseconds emergency_sound_last_time_on;
  DvRes_h dv_res;
  DvAsb_h dv_asb;
  DvSpeed_h dv_speed;
  GpioRead_h gpio_air_precharge_init;
  GpioRead_h gpio_air_precharge_done;
  Gpio_h gpio_emergency_sound;
  Gpio_h gpio_ass_light_blue;
  Gpio_h gpio_ass_light_yellow;
  DvMission_h* dv_mission;
  const DvDriverInput_h* dv_driver_input;
  struct EmergencyNode_h emergency_node;
  struct CanMailbox* send_car_dv_car_status_mailbox;
};

enum DV_EMERGENCY{
  EMERENGENCY_DV_EMERGENCY =0,

  __NUM_OF_DV_EMERGENCIES__
};

union Dv_h_t_conv{
  struct Dv_h* const restrict hidden;
  struct Dv_t* const restrict clear;
};

#ifdef DEBUG
char __assert_size_dv[(sizeof(Dv_h) == sizeof(struct Dv_t))? 1:-1];
#endif // DEBUG

//INFO: check dbc of can3 in message DV_system_status

static uint8_t sdc_closed(const struct Dv_t* const restrict self)
{
  return  gpio_read_state(&self->gpio_air_precharge_init) &&
    gpio_read_state(&self->gpio_air_precharge_done) &&
    EmergencyNode_is_emergency_state(&self->emergency_node);
}

static void update_dv_status(struct Dv_t* const restrict self, const enum AS_STATUS status)
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

static int8_t dv_update_led(struct Dv_t* const restrict self)
{
  Gpio_h* const restrict gpio_light_blue = &self->gpio_ass_light_blue;
  Gpio_h* const restrict gpio_light_yellow = &self->gpio_ass_light_yellow;
  Gpio_h* const restrict gpio_sound = &self->gpio_emergency_sound;

  if ((timer_time_now() - self->emergency_sound_last_time_on) > 8 SECONDS) {
    gpio_set_high(gpio_sound);
    self->emergency_sound_last_time_on = timer_time_now();
  }

  switch (self->status)
  {
    case AS_OFF:
      gpio_set_high(gpio_light_blue);
      gpio_set_high(gpio_light_yellow);
      gpio_set_high(gpio_sound);
      break;
    case AS_READY:
      gpio_set_high(gpio_light_blue);
      gpio_set_low(gpio_light_yellow);
      gpio_set_high(gpio_sound);
      res_start_time_go(&self->dv_res);
      break;
    case AS_DRIVING:
      gpio_set_high(gpio_light_blue);
      gpio_set_high(gpio_sound);
      if ((timer_time_now() - self->driving_last_time_on) > 100 MILLIS ) {
        gpio_toggle(gpio_light_yellow);
        self->driving_last_time_on = timer_time_now();
      }
      break;
    case AS_EMERGENCY:
      gpio_set_high(gpio_light_yellow);
      if ((timer_time_now() - self->emergency_last_time_on) > 100 MILLIS ) {
        gpio_toggle(gpio_light_blue);
        self->emergency_last_time_on = timer_time_now();
      }
      if (!self->sound_start) {
        self->sound_start =1;
        gpio_set_low(gpio_sound);
        self->emergency_sound_last_time_on = timer_time_now();
      }
      break;
    case AS_FINISHED:
      gpio_set_high(gpio_sound);
      gpio_set_high(gpio_light_yellow);
      gpio_set_low(gpio_light_blue);
      break;
    case __NUM_OF_AS_STATUS__:
    default:
      return -1;

  }

  return 0;
}

//INFO: Flowchart T 14.9.2
static int8_t dv_update_status(struct Dv_t* const restrict self)
{
  const float current_speed = dv_speed_get(&self->dv_speed);
  const float driver_brake = dv_driver_input_get_brake(self->dv_driver_input);
  const enum RUNNING_STATUS giei_status = global_running_status_get();

  if (EmergencyNode_is_emergency_state(&self->emergency_node)) {
    self->status = AS_EMERGENCY;
    EmergencyNode_raise(&self->emergency_node, EMERENGENCY_DV_EMERGENCY);
  }

  if (!ebs_on(&self->dv_asb.dv_ebs)) 
  {
    if (dv_mission_get_current(self->dv_mission) > MANUALY &&
        asb_consistency_check(&self->dv_asb)
        && giei_status >= TS_READY)
    {
      if (giei_status == RUNNING)
      {
        update_dv_status(self, AS_DRIVING);
      }
      else if(driver_brake > 50)
      {
        update_dv_status(self, AS_READY);
      }
      else
      {
        update_dv_status(self, AS_OFF);
      }
    }
    else
    {
      update_dv_status(self, AS_OFF);
    }
  }


  else if (dv_mission_get_status(self->dv_mission) == MISSION_FINISHED &&
      !current_speed && sdc_closed(self))
  {
    update_dv_status(self, AS_FINISHED);
  }
  else
  {
    update_dv_status(self, AS_EMERGENCY);
  }
  return 0;
}

  static int8_t
dv_send_dv_car_status(const struct Dv_t* const restrict self)
{
  const uint64_t data=self->dv_car_status;
  return hardware_mailbox_send(self->send_car_dv_car_status_mailbox, data);
}

//public

  int8_t
dv_class_init(Dv_h* const restrict self ,
    DvMission_h* const restrict mission ,
    DvDriverInput_h* const restrict driver )
{
  union Dv_h_t_conv conv = {self};
  struct Dv_t* const p_self = conv.clear;
  memset(p_self, 0, sizeof(*p_self));

  if(dv_stw_alg_init()<0)
  {
    return -1;
  }

  if(res_class_init(&p_self->dv_res)<0)
  {
    return -2;
  }
  if(asb_class_init(&p_self->dv_asb) <0)
  {
    return -3;
  }
  if (dv_speed_init(&p_self->dv_speed)<0) {
    return -4;
  }

  if (hardware_init_gpio(&p_self->gpio_ass_light_yellow, GPIO_ASSI_LIGHT_YELLOW)<0)
  {
    return -5;
  }

  if (hardware_init_gpio(&p_self->gpio_ass_light_blue, GPIO_ASSI_LIGHT_BLU)<0)
  {
    return -6;
  }

  if (hardware_init_gpio(&p_self->gpio_emergency_sound, GPIO_AS_EMERGENCY_SOUND)<0)
  {
    return -6;
  }

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

  ACTION_ON_CAN_NODE(CAN_DV,{
    p_self->send_car_dv_car_status_mailbox =
    hardware_get_mailbox_single_mex(can_node, SEND_MAILBOX, CAN_ID_DV_CARSTATUS, 1);
  })

  p_self->dv_car_status=DV_CAR_STATUS_OFF;
  p_self->dv_mission = mission;
  p_self->dv_driver_input = driver;

  return 0;
}

int8_t dv_update(Dv_h* const restrict self )
{
  union Dv_h_t_conv conv = {self};
  struct Dv_t* const p_self = conv.clear;
  const uint8_t input_stw_alg= 1;
  uint8_t output_stw_alg = 1;

  if(dv_speed_update(&p_self->dv_speed)<0)return -1;
  if(asb_update(&p_self->dv_asb)<0) return -2;

  if(dv_update_status(p_self)<0)return -3;
  if(dv_update_led(p_self)<0) return -4;
  if (p_self->status == AS_DRIVING && dv_mission_get_current(p_self->dv_mission) > MANUALY)
  {
    dv_stw_alg_compute(&input_stw_alg, &output_stw_alg); //TODO: not yet implemented
  }
  if (dv_send_dv_car_status(p_self)<0)
  {
    return -5;
  }
  return 0;
}
