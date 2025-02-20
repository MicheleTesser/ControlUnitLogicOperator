#include "steering_wheel.h"
#include "../../linux_board/linux_board.h"
#include "../../../src/lib/board_dbc/dbc/out_lib/can2/can2.h"

#include <stdint.h>
#include <string.h>
#include <threads.h>
#include <stdio.h>

struct SteeringWheel_t{
  uint8_t u8_running;
  thrd_t o_thread;
  enum CAR_MISSIONS o_current_mission;
  struct CanMailbox* p_mailbox_send_mission;
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

  while (p_self->u8_running)
  {
    ACTION_ON_FREQUENCY(t_var, 50 MILLIS)
    {
      o2.can_0x067_CarMission.Mission = p_self->o_current_mission;
      pack_message_can2(&o2, CAN_ID_CARMISSION, &mission_mex_payload);
      hardware_mailbox_send(p_self->p_mailbox_send_mission, mission_mex_payload);
    }
  }

  return 0;
}


//public

int8_t steering_wheel_start(struct SteeringWheel_h* const restrict self)
{
  union SteeringWheel_h_t_conv conv = {self};
  struct SteeringWheel_t* const p_self = conv.clear;
  struct CanNode* temp_can_node = NULL;

  memset(p_self, 0, sizeof(*p_self));

  ACTION_ON_CAN_NODE_EXTERNAL(CAN_GENERAL, temp_can_node)
  {
      p_self->p_mailbox_send_mission = 
        hardware_get_mailbox_single_mex(
            temp_can_node,
            SEND_MAILBOX,
            CAN_ID_CARMISSION,
            message_dlc_can2(CAN_ID_CARMISSION));
  }
  if (!p_self->p_mailbox_send_mission)
  {
    return -1;
  }

  p_self->u8_running = 1;
  thrd_create(&p_self->o_thread, _start_SteeringWheel, p_self);
  return 0;
}

int8_t steering_wheel_select_mission(struct SteeringWheel_h* const restrict self,
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

int8_t steering_wheel_stop(struct SteeringWheel_h* const restrict self)
{
  union SteeringWheel_h_t_conv conv = {self};
  struct SteeringWheel_t* const p_self = conv.clear;

  printf("stopping SteeringWheel\n");
  p_self->u8_running=0;
  thrd_join(p_self->o_thread, NULL);

  return 0;
}

