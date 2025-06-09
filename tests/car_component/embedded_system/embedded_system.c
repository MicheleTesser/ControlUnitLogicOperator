#include "embedded_system.h"
#include "../../linux_board/linux_board.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "../src/lib/board_dbc/dbc/out_lib/can3/can3.h"
#include "../src/lib/board_dbc/dbc/out_lib/can2/can2.h"
#pragma GCC diagnostic pop 
#include "../external_gpio.h"

#include <stdint.h>
#include <string.h>
#include <threads.h>
#include <stdio.h>

struct EmbeddedSystem_t{
  enum EMBEDDED_STATUS embedded_status;
  uint8_t running;
  thrd_t thread;
  int8_t throttle;
  GpioRead_h m_gpio_pcu_embedded;
  struct CanNode* can_node_general;
  struct CanNode* can_node_dv;
  struct CanMailbox* p_mailbox_input_vcu;
  struct CanMailbox* p_mailbox_send_mission_status_vcu;
  struct CanMailbox* p_mailbox_send_alive_message;
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
  uint64_t payload_dv_mission = 0;

  while (p_self->running)
  {
    ACTION_ON_FREQUENCY(t_var, get_tick_from_millis(50))
    {
      if (gpio_read_state(&p_self->m_gpio_pcu_embedded))
      {
        o3.can_0x07d_DV_Driver.Throttle = p_self->throttle;

        o3.can_0x07e_DV_EMBEDDED_Status.Embedded_status = p_self->embedded_status;

        pack_message_can3(&o3, CAN_ID_DV_DRIVER, &payload_dv_driver);
        pack_message_can3(&o3, CAN_ID_DV_EMBEDDED_STATUS, &payload_dv_mission);

        hardware_mailbox_send(p_self->p_mailbox_input_vcu, payload_dv_driver);
        hardware_mailbox_send(p_self->p_mailbox_send_mission_status_vcu, payload_dv_mission);
        hardware_mailbox_send(p_self->p_mailbox_send_alive_message, 0);
      }
    }
  }

  return 0;
}


//public

int8_t embedded_system_start(EmbeddedSystem_h* const restrict self)
{
  union EmbeddedSystem_h_t_conv conv = {self};
  struct EmbeddedSystem_t* const p_self = conv.clear;

  memset(p_self, 0, sizeof(*p_self));

  if (hardware_init_read_permission_gpio(&p_self->m_gpio_pcu_embedded, GPIO_PCU_EMBEDDED_SYSTEM)<0)
  {
    return -1;
  }

  p_self->can_node_general = hardware_init_new_external_node(CAN_GENERAL);

  if (!p_self->can_node_general)
  {
    return -2;
  }

  p_self->can_node_dv = hardware_init_new_external_node(CAN_DV);

  if (!p_self->can_node_dv)
  {
    hardware_init_new_external_node_destroy(p_self->can_node_general);
    return -4;
  }

  p_self->p_mailbox_input_vcu = 
    hardware_get_mailbox_single_mex(
        p_self->can_node_dv, 
        SEND_MAILBOX,
        CAN_ID_DV_DRIVER,
        message_dlc_can3(CAN_ID_DV_DRIVER));

  if (!p_self->p_mailbox_input_vcu)
  {
    hardware_init_new_external_node_destroy(p_self->can_node_dv);
    hardware_init_new_external_node_destroy(p_self->can_node_general);
    return -3;
  }


  p_self->p_mailbox_send_mission_status_vcu =
    hardware_get_mailbox_single_mex(
        p_self->can_node_dv,
        SEND_MAILBOX,
        CAN_ID_DV_EMBEDDED_STATUS,
        message_dlc_can3(CAN_ID_DV_EMBEDDED_STATUS));

  if (!p_self->p_mailbox_send_mission_status_vcu)
  {
    hardware_free_mailbox_can(&p_self->p_mailbox_input_vcu);
    hardware_init_new_external_node_destroy(p_self->can_node_general);
    hardware_init_new_external_node_destroy(p_self->can_node_dv);
    return -2;
  }

  p_self->p_mailbox_send_alive_message =
    hardware_get_mailbox_single_mex(
        p_self->can_node_general,
        SEND_MAILBOX,
        CAN_ID_EMBEDDEDALIVECHECK,
        message_dlc_can2(CAN_ID_EMBEDDEDALIVECHECK));

  if (!p_self->p_mailbox_send_alive_message)
  {
    hardware_free_mailbox_can(&p_self->p_mailbox_send_mission_status_vcu);
    hardware_free_mailbox_can(&p_self->p_mailbox_input_vcu);
    hardware_init_new_external_node_destroy(p_self->can_node_general);
    hardware_init_new_external_node_destroy(p_self->can_node_dv);
    return -3;
  }

  p_self->embedded_status = EMBEDDED_STATUS_OFF;
  p_self->running=1;
  thrd_create(&p_self->thread, _start_embedded_system, p_self);
  return 0;
}

int8_t embedded_system_set_dv_input(EmbeddedSystem_h* const restrict self, const int8_t value)
{
  union EmbeddedSystem_h_t_conv conv = {self};
  struct EmbeddedSystem_t* const p_self = conv.clear;

  p_self->throttle = value;

  return 0;
}

int8_t embedded_system_set_mission_status(EmbeddedSystem_h* const restrict self,
    const enum EMBEDDED_STATUS embedded_status)
{
  union EmbeddedSystem_h_t_conv conv = {self};
  struct EmbeddedSystem_t* const p_self = conv.clear;

  p_self->embedded_status = embedded_status;

  return 0;
}

void embedded_system_stop(EmbeddedSystem_h* const restrict self)
{
  union EmbeddedSystem_h_t_conv conv = {self};
  struct EmbeddedSystem_t* const p_self = conv.clear;

  printf("stopping embedded_system\n");
  p_self->running=0;
  thrd_join(p_self->thread, NULL);

  hardware_free_mailbox_can(&p_self->p_mailbox_send_alive_message);
  hardware_free_mailbox_can(&p_self->p_mailbox_send_mission_status_vcu);
  hardware_free_mailbox_can(&p_self->p_mailbox_input_vcu);
  hardware_init_new_external_node_destroy(p_self->can_node_general);
  hardware_init_new_external_node_destroy(p_self->can_node_dv);
}

