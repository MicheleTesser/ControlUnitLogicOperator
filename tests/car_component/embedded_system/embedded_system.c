#include "embedded_system.h"
#include "../../linux_board/linux_board.h"
#include "../src/lib/board_dbc/dbc/out_lib/can3/can3.h"

#include <stdint.h>
#include <string.h>
#include <threads.h>
#include <stdio.h>

struct EmbeddedSystem_t{
  uint8_t running;
  thrd_t thread;
  struct{
    uint8_t throttle;
    uint8_t brake;
    uint8_t steering_wheel;
  }Input;
  struct CanMailbox* p_mailbox_input_vcu;
};

union EmbeddedSystem_h_t_conv{
  EmbeddedSystem_h* const hidden;
  struct EmbeddedSystem_t* const clear;
};

union EmbeddedSystem_h_t_conv_const{
  const EmbeddedSystem_h* const hidden;
  const struct EmbeddedSystem_t* const clear;
};

#ifdef DEBUG
char __assert_size_embedded_system[(sizeof(EmbeddedSystem_h)==sizeof(struct EmbeddedSystem_t))?+1:-1];
char __assert_align_embedded_system[(_Alignof(EmbeddedSystem_h)==_Alignof(struct EmbeddedSystem_t))?+1:-1];
#endif /* ifdef DEBUG */

int _start_embedded_system(void* arg)
{
  struct EmbeddedSystem_t* const p_self = arg;

  time_var_microseconds t_var =0;
  can_obj_can3_h_t o3={0};
  uint64_t payload_dv_driver = 0;

  while (p_self->running)
  {
    ACTION_ON_FREQUENCY(t_var, 50 MILLIS)
    {
      o3.can_0x07d_DV_Driver.Brake = p_self->Input.brake;
      o3.can_0x07d_DV_Driver.Throttle = p_self->Input.throttle;
      o3.can_0x07d_DV_Driver.Steering_angle = p_self->Input.steering_wheel;

      pack_message_can3(&o3, CAN_ID_DV_DRIVER, &payload_dv_driver);
      hardware_mailbox_send(p_self->p_mailbox_input_vcu, payload_dv_driver);
    }
  }

  return 0;
}


//public

int8_t embedded_system_start(EmbeddedSystem_h* const restrict self)
{
  union EmbeddedSystem_h_t_conv conv = {self};
  struct EmbeddedSystem_t* const p_self = conv.clear;
  struct CanNode* can_node = NULL;

  memset(p_self, 0, sizeof(*p_self));

  ACTION_ON_CAN_NODE_EXTERNAL(CAN_DV, can_node)
  {
    p_self->p_mailbox_input_vcu = 
      hardware_get_mailbox_single_mex(
          can_node, 
          SEND_MAILBOX,
          CAN_ID_DV_DRIVER,
          message_dlc_can3(CAN_ID_DV_DRIVER));
  }

  if (!p_self->p_mailbox_input_vcu)
  {
    return -1;
  }

  p_self->running=1;
  thrd_create(&p_self->thread, _start_embedded_system, p_self);
  return 0;
}

int8_t embedded_system_set_dv_input(EmbeddedSystem_h* const restrict self,
    const enum EMBEDDED_DV_INPUT dv_input_type, const uint8_t value)
{
  union EmbeddedSystem_h_t_conv conv = {self};
  struct EmbeddedSystem_t* const p_self = conv.clear;

  switch (dv_input_type)
  {
    case DV_INPUT_THROTTLE:
      p_self->Input.throttle = value;
      break;
    case DV_INPUT_BRAKE:
      p_self->Input.brake = value;
      break;
    case DV_INPUT_STEERING_ANGLE:
      p_self->Input.steering_wheel = value;
      break;
    case __NUM_OF_EMBEDDED_IMPUT__:
    default:
      return -1;
  }

  return 0;
}

int8_t embedded_system_stop(EmbeddedSystem_h* const restrict self)
{
  union EmbeddedSystem_h_t_conv conv = {self};
  struct EmbeddedSystem_t* const p_self = conv.clear;

  printf("stopping embedded_system\n");
  p_self->running=0;
  thrd_join(p_self->thread, NULL);

  return 0;
}

