#include "asb.h"
#include "../../linux_board/linux_board.h"
#include <sys/cdefs.h>
#include <sys/socket.h>
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

struct Asb_t{
  thrd_t thread;
  enum CAR_MISSIONS m_mission;
  EbsPhaes_t m_phase;
  float m_current_speed;
  volatile uint8_t m_running:1;
  uint8_t m_integrity_check_status:1;
  uint8_t m_rtd:1;
  uint8_t m_brakes_engaged:1;
  uint8_t m_system_status:1;
  time_var_microseconds m_start_system_check;
  struct{
    uint8_t pressure:5;
    uint8_t sanity:1;
  }Tank[__NUM_OF_TANKS__];
  struct CanNode* can_node_general;
  struct CanMailbox* p_mailbox_recv_dv_mission_status;
  struct CanMailbox* p_mailbox_recv_asb_check_req;
  struct CanMailbox* p_mailbox_recv_car_status;
  struct CanMailbox* p_mailbox_recv_res;
  struct CanMailbox* p_mailbox_recv_brake_request;
  struct CanMailbox* p_mailbox_send_ebs_status;
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

static void _send_ebs_status(struct Asb_t* const restrict self)
{
  can_obj_can2_h_t o2 = {0};
  uint64_t payload =0;

  o2.can_0x03c_EbsStatus.press_left_tank = self->Tank[TANK_LEFT].pressure;
  o2.can_0x03c_EbsStatus.sanity_left_sensor = self->Tank[TANK_RIGHT].sanity;

  o2.can_0x03c_EbsStatus.press_right_tank = self->Tank[TANK_RIGHT].pressure;
  o2.can_0x03c_EbsStatus.sanity_right_sensor = self->Tank[TANK_RIGHT].sanity;

  o2.can_0x03c_EbsStatus.brakes_engaged = self->m_brakes_engaged;

  o2.can_0x03c_EbsStatus.system_check = self->m_system_status;

  o2.can_0x03c_EbsStatus.ASB_check =
      self->m_system_status &&
      self->m_integrity_check_status &&
      (timer_time_now() - self->m_start_system_check) > 2 SECONDS;

  pack_message_can2(&o2, CAN_ID_EBSSTATUS, &payload);

  hardware_mailbox_send(self->p_mailbox_send_ebs_status, payload);
}

static int _start_asb(void* arg)
{
  union asb_h_t_conv conv = {arg};
  struct Asb_t* const p_self = conv.clear;
  CanMessage mex = {0};
  can_obj_can2_h_t o2 = {0};
  time_var_microseconds t=0;

  while (p_self->m_running)
  {
    ACTION_ON_FREQUENCY(t, 50 MILLIS)
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

      if (p_self->m_mission <= CAR_MISSIONS_HUMAN)
      {
        asb_reset(conv.hidden);
        continue;
      }

      switch (p_self->m_phase)
      {
        case EbsPhase_0:
          if (p_self->m_mission > CAR_MISSIONS_HUMAN)
          {
            p_self->m_phase = EbsPhase_1;
          }
          break;
        case EbsPhase_1:
          p_self->m_brakes_engaged =1;
          if (hardware_mailbox_read(p_self->p_mailbox_recv_asb_check_req, &mex))
          {
            unpack_message_can2(&o2, mex.id, mex.full_word, mex.message_size, 0);
            if(o2.can_0x068_CheckASBReq.req)
            {
              p_self->m_phase = EbsPhase_2;
              if (!p_self->m_start_system_check)
              {
                p_self->m_start_system_check = timer_time_now();
              }
            }
          }
          break;
        case EbsPhase_2:
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
          if (hardware_mailbox_read(p_self->p_mailbox_recv_car_status, &mex))
          {
            unpack_message_can2(&o2, mex.id, mex.full_word, mex.message_size, 0);
            p_self->m_rtd = o2.can_0x065_CarStatus.RunningStatus == 3; //INFO: check dbc file
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
            p_self->m_brakes_engaged =0;
          }
          if (!p_self->m_rtd)
          {
            p_self->m_phase = EbsPhase_0;
          }
          break;
        case EbsPhase_5:
          if (p_self->m_mission == CAR_MISSIONS_NONE)
          {
            p_self->m_phase = EbsPhase_0;
          }

          p_self->m_brakes_engaged= 
            hardware_mailbox_read(p_self->p_mailbox_recv_brake_request, &mex) && 
            p_self->m_current_speed < 5.0f;//INFO: km/h

          break;
      }
      _send_ebs_status(p_self);
    }
  }

  thrd_exit(0);

  return 0;
}


//public

int8_t asb_start(Asb_h* const restrict self)
{
  union asb_h_t_conv conv = {self};
  struct Asb_t* const p_self = conv.clear;

  memset(p_self, 0, sizeof(*p_self));

  p_self->can_node_general = hardware_init_new_external_node(CAN_GENERAL);

  if (!p_self->can_node_general)
  {
    return -1;
  }

  p_self->p_mailbox_recv_asb_check_req =
    hardware_get_mailbox_single_mex(
        p_self->can_node_general,
        RECV_MAILBOX,
        CAN_ID_CHECKASBREQ,
        message_dlc_can2(CAN_ID_CHECKASBREQ));

  if (!p_self->p_mailbox_recv_asb_check_req)
  {
    hardware_init_new_external_node_destroy(p_self->can_node_general);
    return -2;
  }

  p_self->p_mailbox_recv_brake_request =
    hardware_get_mailbox_single_mex(
        p_self->can_node_general,
        RECV_MAILBOX,
        CAN_ID_EBSBRAKEREQ,
        message_dlc_can2(CAN_ID_EBSBRAKEREQ));

  if (!p_self->p_mailbox_recv_brake_request)
  {
    hardware_free_mailbox_can(&p_self->p_mailbox_recv_asb_check_req);
    hardware_init_new_external_node_destroy(p_self->can_node_general);
    return -3;
  }

  p_self->p_mailbox_recv_car_status=
    hardware_get_mailbox_single_mex(
        p_self->can_node_general,
        RECV_MAILBOX,
        CAN_ID_CARSTATUS,
        message_dlc_can2(CAN_ID_CARSTATUS));

  if (!p_self->p_mailbox_recv_car_status)
  {
    hardware_free_mailbox_can(&p_self->p_mailbox_recv_brake_request);
    hardware_free_mailbox_can(&p_self->p_mailbox_recv_asb_check_req);
    hardware_init_new_external_node_destroy(p_self->can_node_general);
    return -4;
  }

  p_self->p_mailbox_recv_res=
    hardware_get_mailbox_single_mex(
        p_self->can_node_general,
        RECV_MAILBOX,
        CAN_ID_RESSTATUS,
        message_dlc_can2(CAN_ID_RESSTATUS));

  if (!p_self->p_mailbox_recv_res)
  {
    hardware_free_mailbox_can(&p_self->p_mailbox_recv_car_status);
    hardware_free_mailbox_can(&p_self->p_mailbox_recv_brake_request);
    hardware_free_mailbox_can(&p_self->p_mailbox_recv_asb_check_req);
    hardware_init_new_external_node_destroy(p_self->can_node_general);
    return -5;
  }

    p_self->p_mailbox_recv_dv_mission_status=
      hardware_get_mailbox_single_mex(
          p_self->can_node_general,
          RECV_MAILBOX,
          CAN_ID_CARMISSIONSTATUS,
          message_dlc_can2(CAN_ID_CARMISSIONSTATUS));

  if (!p_self->p_mailbox_recv_dv_mission_status)
  {
    hardware_free_mailbox_can(&p_self->p_mailbox_recv_dv_mission_status);
    hardware_free_mailbox_can(&p_self->p_mailbox_recv_car_status);
    hardware_free_mailbox_can(&p_self->p_mailbox_recv_brake_request);
    hardware_free_mailbox_can(&p_self->p_mailbox_recv_asb_check_req);
    hardware_init_new_external_node_destroy(p_self->can_node_general);
    return -6;
  }

  p_self->p_mailbox_send_ebs_status=
    hardware_get_mailbox_single_mex(
        p_self->can_node_general,
        SEND_MAILBOX,
        CAN_ID_EBSSTATUS,
        message_dlc_can2(CAN_ID_EBSSTATUS));

  if (!p_self->p_mailbox_send_ebs_status)
  {
    hardware_free_mailbox_can(&p_self->p_mailbox_recv_dv_mission_status);
    hardware_free_mailbox_can(&p_self->p_mailbox_recv_dv_mission_status);
    hardware_free_mailbox_can(&p_self->p_mailbox_recv_car_status);
    hardware_free_mailbox_can(&p_self->p_mailbox_recv_brake_request);
    hardware_free_mailbox_can(&p_self->p_mailbox_recv_asb_check_req);
    hardware_init_new_external_node_destroy(p_self->can_node_general);
    return -7;
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
    case SYSTEM_CHECK:
      p_self->m_system_status = value;
      break;
    case CURR_MISSION:
      p_self->m_mission = value;
      break;
    case __NUM_OF_ASB_CONFI__:
      return -1;
      break;
    }

  return 0;
}

void asb_reset(Asb_h* const restrict self)
{
  union asb_h_t_conv conv = {self};
  struct Asb_t* const p_self = conv.clear;

  p_self->m_system_status = 0;
  p_self->m_integrity_check_status = 0;
  p_self->m_rtd = 0;
  p_self->m_brakes_engaged = 0;
  p_self->m_start_system_check = 0;
  p_self->m_phase = EbsPhase_0;
  p_self->m_current_speed =0;
  memset(p_self->Tank,0 ,sizeof(p_self->Tank));
}

int8_t asb_stop(Asb_h* const restrict self)
{
  union asb_h_t_conv conv = {self};
  struct Asb_t* const p_self = conv.clear;

  printf("stopping asb\n");
  p_self->m_running=0;


  hardware_free_mailbox_can(&p_self->p_mailbox_send_ebs_status);
  hardware_free_mailbox_can(&p_self->p_mailbox_recv_dv_mission_status);
  hardware_free_mailbox_can(&p_self->p_mailbox_recv_car_status);
  hardware_free_mailbox_can(&p_self->p_mailbox_recv_brake_request);
  hardware_free_mailbox_can(&p_self->p_mailbox_recv_asb_check_req);
  wait_milliseconds(300 MILLIS);
  hardware_init_new_external_node_destroy(p_self->can_node_general);
  p_self->can_node_general = NULL;

  return 0;
}

EbsPhaes_t asb_current_phase(const Asb_h* const restrict self)
{
  const union asb_h_t_conv_const conv = {self};
  const struct Asb_t* const p_self = conv.clear;

  return p_self->m_phase;
}
