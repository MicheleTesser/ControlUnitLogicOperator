#include "asb.h"
#include "../../linux_board/linux_board.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "../../../src/lib/board_dbc/dbc/out_lib/can2/can2.h"
#pragma GCC diagnostic pop 
#include "../embedded_system/embedded_system.h"

#include <stdint.h>
#include <string.h>
#include <threads.h>
#include <stdio.h>

enum TANKS{
  TANK_RIGHT=0,
  TANK_LEFT,

  __NUM_OF_TANKS__
};

struct Asb_t{
  thrd_t thread;
  uint8_t running:1;
  uint8_t integrity_check_status:1;
  uint8_t mission_status:2;
  uint8_t active:1;
  struct{
    uint8_t pressure:5;
    uint8_t sanity:1;
  }Tank[__NUM_OF_TANKS__];
  struct CanMailbox* p_mailbox_send_asb_status;
  struct CanMailbox* p_mailbox_send_integrity_check_resp;
  struct CanMailbox* p_mailbox_recv_mission_status;
  struct CanMailbox* p_mailbox_recv_integrity_check_req;
};

union asb_h_t_conv{
  Asb_h* const hidden;
  struct Asb_t* const clear;
};

union asb_h_t_conv_const{
  const Asb_h* const hidden;
  const struct Asb_t* const clear;
};

#ifdef DEBUG
char __assert_size_asb[(sizeof(Asb_h)==sizeof(struct Asb_t))?+1:-1];
char __assert_align_asb[(_Alignof(Asb_h)==_Alignof(struct Asb_t))?+1:-1];
#endif /* ifdef DEBUG */

int _start_asb(void* arg)
{
  struct Asb_t* const p_self = arg;
  CanMessage mex = {0};
  time_var_microseconds t_var =0;
  can_obj_can2_h_t o2 = {0};
  uint64_t payload_tank_ebs_mex = 0;
  uint64_t payload_consistency_check_resp = 0;

  while (p_self->running)
  {
    ACTION_ON_FREQUENCY(t_var, 10 MILLIS)
    {
      if (hardware_mailbox_read(p_self->p_mailbox_recv_mission_status, &mex))
      {
        unpack_message_can2(&o2, mex.id, mex.full_word, mex.message_size, 0);
        p_self->mission_status = o2.can_0x071_CarMissionStatus.MissionStatus;
        p_self->active = o2.can_0x071_CarMissionStatus.Mission > 1;
      }

      if (p_self->active &&
          p_self->mission_status == MISSION_NOT_RUNNING &&
          hardware_mailbox_read(p_self->p_mailbox_recv_integrity_check_req, &mex) &&
          !mex.full_word)
      {
        o2.can_0x068_CheckASB.Mode = 1;
        o2.can_0x068_CheckASB.response_status = 0;
        pack_message_can2(&o2, CAN_ID_CHECKASB, &payload_consistency_check_resp);
        hardware_mailbox_send(p_self->p_mailbox_send_integrity_check_resp, payload_consistency_check_resp);
      }

      if (p_self->active)
      {
        o2.can_0x03c_TanksEBS.press_left_tank = p_self->Tank[TANK_LEFT].pressure;
        o2.can_0x03c_TanksEBS.sanity_left_sensor = p_self->Tank[TANK_LEFT].sanity;

        o2.can_0x03c_TanksEBS.press_right_tank = p_self->Tank[TANK_RIGHT].pressure;
        o2.can_0x03c_TanksEBS.sanity_right_sensor = p_self->Tank[TANK_RIGHT].sanity;

        o2.can_0x03c_TanksEBS.system_check = p_self->integrity_check_status;

        pack_message_can2(&o2, CAN_ID_TANKSEBS, &payload_tank_ebs_mex);
        hardware_mailbox_send(p_self->p_mailbox_send_asb_status, payload_tank_ebs_mex);
      }
    }
  }

  return 0;
}


//public

int8_t asb_start(Asb_h* const restrict self)
{
  union asb_h_t_conv conv = {self};
  struct Asb_t* const p_self = conv.clear;
  struct CanNode* can_node = NULL;

  memset(p_self, 0, sizeof(*p_self));

  ACTION_ON_CAN_NODE_EXTERNAL(CAN_GENERAL, can_node)
  {
    p_self->p_mailbox_send_asb_status = 
      hardware_get_mailbox_single_mex(
          can_node,
          SEND_MAILBOX,
          CAN_ID_TANKSEBS,
          message_dlc_can2(CAN_ID_TANKSEBS)
          );
  }

  if (!p_self->p_mailbox_send_asb_status)
  {
    return -1;
  }


  ACTION_ON_CAN_NODE_EXTERNAL(CAN_GENERAL, can_node)
  {
    p_self->p_mailbox_recv_mission_status= 
      hardware_get_mailbox_single_mex(
          can_node,
          RECV_MAILBOX,
          CAN_ID_CARMISSIONSTATUS,
          message_dlc_can2(CAN_ID_CARMISSIONSTATUS)
          );
  }

  if (!p_self->p_mailbox_recv_mission_status)
  {
    hardware_free_mailbox_can(&p_self->p_mailbox_send_asb_status);
    return -2;
  }


  ACTION_ON_CAN_NODE_EXTERNAL(CAN_GENERAL, can_node)
  {
    p_self->p_mailbox_recv_integrity_check_req =
      hardware_get_mailbox_single_mex(
          can_node,
          RECV_MAILBOX,
          CAN_ID_CHECKASB,
          message_dlc_can2(CAN_ID_CHECKASB)
          );
  }

  if (!p_self->p_mailbox_recv_integrity_check_req)
  {
    hardware_free_mailbox_can(&p_self->p_mailbox_send_asb_status);
    hardware_free_mailbox_can(&p_self->p_mailbox_recv_mission_status);
    return -3;
  }

  ACTION_ON_CAN_NODE_EXTERNAL(CAN_GENERAL, can_node)
  {
    p_self->p_mailbox_send_integrity_check_resp =
      hardware_get_mailbox_single_mex(
          can_node,
          SEND_MAILBOX,
          CAN_ID_CHECKASB,
          message_dlc_can2(CAN_ID_CHECKASB)
          );
  }

  if (!p_self->p_mailbox_send_integrity_check_resp)
  {
    hardware_free_mailbox_can(&p_self->p_mailbox_send_asb_status);
    hardware_free_mailbox_can(&p_self->p_mailbox_recv_integrity_check_req);
    hardware_free_mailbox_can(&p_self->p_mailbox_recv_mission_status);
    return -3;
  }

  p_self->running=1;
  thrd_create(&p_self->thread, _start_asb, p_self);
  return 0;
}

int8_t
asb_set_parameter(Asb_h* const restrict self,
    const enum ASB_CONFIG param_type, const uint8_t value)
{
  union asb_h_t_conv conv = {self};
  struct Asb_t* const p_self = conv.clear;

  switch (param_type)
  {
    case TANK_LEFT_PRESSURE:
      p_self->Tank[TANK_LEFT].pressure = value;
      break;
    case TANK_LEFT_SANITY:
      p_self->Tank[TANK_LEFT].sanity = value;
      break;
    case TANK_RIGHT_PRESSURE:
      p_self->Tank[TANK_RIGHT].pressure = value;
      break;
    case TANK_RIGHT_SANITY:
      p_self->Tank[TANK_RIGHT].sanity = value;
      break;
    case INTEGRITY_CHECK_STATUS:
      p_self->integrity_check_status = value;
      break;
    default:
      return -1;;
    }

  return 0;
}

int8_t asb_stop(Asb_h* const restrict self)
{
  union asb_h_t_conv conv = {self};
  struct Asb_t* const p_self = conv.clear;

  printf("stopping asb\n");
  p_self->running=0;
  thrd_join(p_self->thread, NULL);

  return 0;
}

