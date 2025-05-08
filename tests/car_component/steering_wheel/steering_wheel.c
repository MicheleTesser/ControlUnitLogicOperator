#include "steering_wheel.h"
#include "../../linux_board/linux_board.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "../../../src/lib/board_dbc/dbc/out_lib/can2/can2.h"
#pragma GCC diagnostic pop 

#include <stdint.h>
#include <string.h>
#include <threads.h>
#include <stdio.h>

struct SteeringWheel_t{
  uint8_t u8_running;
  struct{
    uint8_t power;
    uint8_t regen;
    uint8_t torque_rep;
  }Maps;
  thrd_t o_thread;
  enum CAR_MISSIONS o_current_mission;
  struct CanMailbox* p_mailbox_send_mission;
  struct CanMailbox* p_mailbox_send_maps;
  struct CanNode* temp_can_node;
};

union SteeringWheel_h_t_conv{
  SteeringWheel_h* const hidden;
  struct SteeringWheel_t* const clear;
};

union SteeringWheel_h_t_conv_const{
  const SteeringWheel_h* const hidden;
  const struct SteeringWheel_t* const clear;
};

#ifdef DEBUG
char __assert_size_SteeringWheel[(sizeof(SteeringWheel_h)==sizeof(struct SteeringWheel_t))?+1:-1];
char __assert_align_SteeringWheel[(_Alignof(SteeringWheel_h)==_Alignof(struct SteeringWheel_t))?+1:-1];
#endif /* ifdef DEBUG */

int _start_SteeringWheel(void* arg)
{
  struct SteeringWheel_t* const p_self = arg;
  time_var_microseconds t_var =0;
  can_obj_can2_h_t o2 = {0};
  uint64_t mission_mex_payload = 0;
  uint64_t map_mex_payload = 0;

  while (p_self->u8_running)
  {
    ACTION_ON_FREQUENCY(t_var, get_tick_from_millis(50))
    {
      o2.can_0x047_CarMission.Mission = p_self->o_current_mission;

      o2.can_0x064_Map.power = p_self->Maps.power;
      o2.can_0x064_Map.regen = p_self->Maps.regen;
      o2.can_0x064_Map.torque_rep = p_self->Maps.torque_rep;

      pack_message_can2(&o2, CAN_ID_CARMISSION, &mission_mex_payload);
      pack_message_can2(&o2, CAN_ID_MAP, &map_mex_payload);

      hardware_mailbox_send(p_self->p_mailbox_send_mission, mission_mex_payload);
      hardware_mailbox_send(p_self->p_mailbox_send_maps, map_mex_payload);
    }
  }

  return 0;
}


//public

int8_t steering_wheel_start(SteeringWheel_h* const restrict self)
{
  union SteeringWheel_h_t_conv conv = {self};
  struct SteeringWheel_t* const p_self = conv.clear;

  memset(p_self, 0, sizeof(*p_self));

  p_self->temp_can_node = hardware_init_new_external_node(CAN_GENERAL);

  p_self->p_mailbox_send_mission = 
    hardware_get_mailbox_single_mex(
        p_self->temp_can_node,
        SEND_MAILBOX,
        CAN_ID_CARMISSION,
        message_dlc_can2(CAN_ID_CARMISSION));

  if (!p_self->p_mailbox_send_mission)
  {
    hardware_init_new_external_node_destroy(p_self->temp_can_node);
    return -1;
  }

  p_self->p_mailbox_send_maps =
    hardware_get_mailbox_single_mex(
        p_self->temp_can_node,
        SEND_MAILBOX,
        CAN_ID_MAP,
        message_dlc_can2(CAN_ID_MAP));


  if (!p_self->p_mailbox_send_maps)
  {
    hardware_free_mailbox_can(&p_self->p_mailbox_send_mission);
    hardware_init_new_external_node_destroy(p_self->temp_can_node);
    return -2;
  }

  p_self->u8_running = 1;
  thrd_create(&p_self->o_thread, _start_SteeringWheel, p_self);
  return 0;
}

int8_t steering_wheel_select_mission(SteeringWheel_h* const restrict self,
    const enum CAR_MISSIONS mission)
{
  union SteeringWheel_h_t_conv conv = {self};
  struct SteeringWheel_t* const p_self = conv.clear;

  if (mission >= __NUM_OF_CAR_MISSIONS__)
  {
    return -1;
  }

  p_self->o_current_mission = mission;

  return 0;
}

int8_t steering_wheel_select_map(SteeringWheel_h* const restrict self,
    const enum MAPS_TYPE map_type, const uint8_t value)
{
  union SteeringWheel_h_t_conv conv = {self};
  struct SteeringWheel_t* const p_self = conv.clear;

  switch (map_type)
  {
    case MAPS_TYPE_POWER:
      p_self->Maps.power = value;
      break;
    case MAPS_TYPE_REGEN:
      p_self->Maps.regen = value;
      break;
    case MAPS_TYPE_TV_REPARTITION:
      p_self->Maps.torque_rep = value;
      break;
    case __NUM_OF_INPUT_MAPS_TYPE__:
    default:
      return -1;
  }

  return 0;
}

int8_t steering_wheel_stop(SteeringWheel_h* const restrict self)
{
  union SteeringWheel_h_t_conv conv = {self};
  struct SteeringWheel_t* const p_self = conv.clear;

  printf("stopping SteeringWheel\n");
  p_self->u8_running=0;
  thrd_join(p_self->o_thread, NULL);
  hardware_free_mailbox_can(&p_self->p_mailbox_send_maps);
  hardware_free_mailbox_can(&p_self->p_mailbox_send_mission);
  hardware_init_new_external_node_destroy(p_self->temp_can_node);


  return 0;
}

