#include "atc.h"
#include "../../linux_board/linux_board.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "../src/lib/board_dbc/dbc/out_lib/can2/can2.h"
#pragma GCC diagnostic pop 
#include <stdint.h>
#include <stdio.h>
#include <threads.h>

struct Atc_t{
  float throttle;
  float brake;
  float steering_angle;
  struct CanMailbox* send_vcu_mailbox;
  thrd_t thread;
  uint8_t run:1;
};

union Atc_h_t_conv{
  Atc_h* const hidden;
  struct Atc_t* const clear;
};

#ifdef DEBUG
char __assert_size_atc[(sizeof(Atc_h)==sizeof(struct Atc_t))?1:-1];
char __assert_align_atc[(sizeof(Atc_h)==sizeof(struct Atc_t))?1:-1];
#endif /* ifdef DEBUG */

static int
_atc_start(void* arg)
{
  struct Atc_t* const restrict self = arg;
  time_var_microseconds last_sent = 0;

  while (self->run)
  {
    ACTION_ON_FREQUENCY(last_sent, 50 MILLIS)
    {
      can_obj_can2_h_t o2 = {0};
      uint64_t data=0;
      o2.can_0x053_Driver.brake = self->brake;
      o2.can_0x053_Driver.throttle = self->throttle;
      o2.can_0x053_Driver.steering = self->steering_angle;
      o2.can_0x053_Driver.no_implausibility = 1;
      pack_message_can2(&o2, CAN_ID_DRIVER, &data);
      hardware_mailbox_send(self->send_vcu_mailbox, data);
    }
  }
  return 0;
}

//public

int8_t
atc_start(Atc_h* const restrict self)
{
  union Atc_h_t_conv conv = {self};
  struct Atc_t* const restrict p_self = conv.clear;

  struct CanNode* node = hardware_init_new_external_node(CAN_GENERAL);

  if (!node)
  {
    return -1;
  }

  p_self->send_vcu_mailbox =
    hardware_get_mailbox_single_mex(
        node,
        SEND_MAILBOX,
        CAN_ID_DRIVER,
        message_dlc_can2(CAN_ID_DRIVER));

  if (!p_self->send_vcu_mailbox)
  {
    return -2;
  }
  hardware_init_new_external_node_destroy(node);

  p_self->run=1;
  thrd_create(&p_self->thread, _atc_start, p_self);
  return 0;
}

int8_t
atc_pedals_steering_wheel(Atc_h* const restrict self, const enum InputAtc input, const float value)
{
  union Atc_h_t_conv conv = {self};
  struct Atc_t* const restrict p_self = conv.clear;

  switch (input)
  {
    case ATC_THROTTLE:
      p_self->throttle = value;
      break;
    case ATC_BRAKE:
      p_self->brake = value;
      break;
    case ATC_STEERING_ANGLE:
      p_self->steering_angle = value;
      break;
    default:
      return -1;
  }

  return 0;
}

int8_t
atc_stop(Atc_h* const restrict self)
{
  union Atc_h_t_conv conv = {self};
  struct Atc_t* const restrict p_self = conv.clear;

  printf("stopping atc\n");
  p_self->run=0;
  thrd_join(p_self->thread, NULL);
  hardware_free_mailbox_can(&p_self->send_vcu_mailbox);
  return 0;
}
