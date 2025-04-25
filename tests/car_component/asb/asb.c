#include "asb.h"
#include "../../linux_board/linux_board.h"
#include <sys/cdefs.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "../../../src/lib/board_dbc/dbc/out_lib/can2/can2.h"
#include "../../../src/cores/core_utility/mission_reader/mission_reader.h"
#pragma GCC diagnostic pop 

#include <stdint.h>
#include <string.h>
#include <threads.h>
#include <stdio.h>

enum TANKS{
  TANK_RIGHT=0,
  TANK_LEFT,

  __NUM_OF_TANKS__
};

//INFO: phase details at: https://github.com/raceup-electric/EBS/tree/main
typedef enum{
  EbsPhase_0=0,
  EbsPhase_1,
  EbsPhase_2,
  EbsPhase_3,
  EbsPhase_4,
  EbsPhase_5,
}EbsPhaes_t;

struct Asb_t{
  thrd_t thread;
  enum CAR_MISSIONS m_mission;
  EbsPhaes_t m_phase;
  float m_current_speed;
  uint8_t m_running:1;
  uint8_t m_integrity_check_status:1;
  uint8_t m_rtd:1;
  uint8_t m_brakes_engaged:1;
  struct{
    uint8_t pressure:5;
    uint8_t sanity:1;
  }Tank[__NUM_OF_TANKS__];
  struct CanMailbox* p_mailbox_recv_dv_mission_status;
  struct CanMailbox* p_mailbox_recv_asb_check_req;
  struct CanMailbox* p_mailbox_recv_car_status;
  struct CanMailbox* p_mailbox_recv_res;
  struct CanMailbox* p_mailbox_recv_brake_request;
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

static void _tank_pressure_validation(const struct Asb_t* const restrict self __attribute_maybe_unused__)
{
  printf("_tank_pressure_validation not yet implemented\n");
}

static void _send_system_status(const struct Asb_t* const restrict self __attribute_maybe_unused__)
{
  printf("_send_system_status not yet implemented\n");
}

static void _asb_check(struct Asb_t* const restrict self __attribute_maybe_unused__)
{
  printf("_asb_check not yet implemented\n");
}

static void _send_result_validation(struct Asb_t* const restrict self __attribute_maybe_unused__)
{
  printf("_send_result_validation not yet implemented\n");
}

static int _start_asb(void* arg)
{
  struct Asb_t* const p_self = arg;
  CanMessage mex = {0};
  can_obj_can2_h_t o2 = {0};

  while (p_self->m_running)
  {
    if (hardware_mailbox_read(p_self->p_mailbox_recv_dv_mission_status, &mex))
    {
      unpack_message_can2(&o2, mex.id, mex.full_word, mex.message_size, 0);
      p_self->m_mission = o2.can_0x071_CarMissionStatus.Mission;
    }

    if (hardware_mailbox_read(p_self->p_mailbox_recv_car_status, &mex))
    {
      unpack_message_can2(&o2, mex.id, mex.full_word, mex.message_size, 0);
      p_self->m_current_speed = o2.can_0x065_CarStatus.speed;
    }

    if (hardware_mailbox_read(p_self->p_mailbox_recv_asb_check_req, &mex))
    {
      unpack_message_can2(&o2, mex.id, mex.full_word, mex.message_size, 0);
    }

    if (p_self->m_mission <= CAR_MISSIONS_HUMAN)
    {
      p_self->m_phase = EbsPhase_0;
      continue;
    }

    switch (p_self->m_phase)
    {
      case EbsPhase_0:
        if (p_self->m_mission >= CAR_MISSIONS_HUMAN)
        {
          p_self->m_phase = EbsPhase_1;
        }
        break;
      case EbsPhase_1:
        _tank_pressure_validation(p_self);
        _send_system_status(p_self);
        if (hardware_mailbox_read(p_self->p_mailbox_recv_asb_check_req, &mex))
        {
          unpack_message_can2(&o2, mex.id, mex.full_word, mex.message_size, 0);
          if(o2.can_0x068_CheckASBReq.req)
          {
            p_self->m_phase = EbsPhase_2;
          }
        }
        break;
      case EbsPhase_2:
        _asb_check(p_self);
        _send_result_validation(p_self);
        if (hardware_mailbox_read(p_self->p_mailbox_recv_asb_check_req, &mex))
        {
          unpack_message_can2(&o2, mex.id, mex.full_word, mex.message_size, 0);
          if(o2.can_0x068_CheckASBReq.reqAck)
          {
            p_self->m_phase = EbsPhase_3;
          }
        }
        break;
      case EbsPhase_3:
        _tank_pressure_validation(p_self);
        _send_system_status(p_self);
        if (hardware_mailbox_read(p_self->p_mailbox_recv_car_status, &mex))
        {
          unpack_message_can2(&o2, mex.id, mex.full_word, mex.message_size, 0);
          p_self->m_rtd = o2.can_0x065_CarStatus.RunningStatus == 3;
        }
        if (p_self->m_rtd)
        {
          p_self->m_phase = EbsPhase_4;
        }
        break;
      case EbsPhase_4:
        if (p_self->m_rtd && hardware_mailbox_read(p_self->p_mailbox_recv_res, &mex))
        {
          unpack_message_can2(&o2, mex.id, mex.full_word, mex.message_size, 0);
          //TODO: check res message to start drive mode
          p_self->m_phase = EbsPhase_5;
        }
        if (!p_self->m_rtd)
        {
          p_self->m_phase = EbsPhase_0;
        }
        break;
      case EbsPhase_5:
        //TODO: send continuous monitoring tank pressure
        if (p_self->m_mission == CAR_MISSIONS_NONE)
        {
          p_self->m_phase = EbsPhase_0;
        }

        if (hardware_mailbox_read(p_self->p_mailbox_recv_brake_request, &mex))
        {
          if (p_self->m_current_speed < 5.0f) //INFO: km/h
          {
            p_self->m_brakes_engaged = 1;
          }
        }
        else
        {
          p_self->m_brakes_engaged =0;
        }
        break;
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

  ACTION_ON_CAN_NODE_EXTERNAL(CAN_DV, can_node)
  {
    p_self->p_mailbox_recv_asb_check_req =
      hardware_get_mailbox_single_mex(
          can_node,
          RECV_MAILBOX,
          CAN_ID_CHECKASBREQ,
          message_dlc_can2(CAN_ID_CHECKASBREQ));

    p_self->p_mailbox_recv_brake_request =
      hardware_get_mailbox_single_mex(
          can_node,
          RECV_MAILBOX,
          CAN_ID_EBSBRAKEREQ,
          message_dlc_can2(CAN_ID_EBSBRAKEREQ));

    p_self->p_mailbox_recv_car_status=
      hardware_get_mailbox_single_mex(
          can_node,
          RECV_MAILBOX,
          CAN_ID_CARSTATUS,
          message_dlc_can2(CAN_ID_CARSTATUS));

    p_self->p_mailbox_recv_res=
      hardware_get_mailbox_single_mex(
          can_node,
          RECV_MAILBOX,
          CAN_ID_RESSTATUS,
          message_dlc_can2(CAN_ID_RESSTATUS));

    p_self->p_mailbox_recv_dv_mission_status=
      hardware_get_mailbox_single_mex(
          can_node,
          RECV_MAILBOX,
          CAN_ID_CARMISSIONSTATUS,
          message_dlc_can2(CAN_ID_CARMISSIONSTATUS));
  }


  p_self->m_running=1;
  thrd_create(&p_self->thread, _start_asb, p_self);
  return 0;
}

int8_t asb_set_parameter(Asb_h* const restrict self,
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
      p_self->m_integrity_check_status = value;
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
  p_self->m_running=0;
  thrd_join(p_self->thread, NULL);

  return 0;
}
