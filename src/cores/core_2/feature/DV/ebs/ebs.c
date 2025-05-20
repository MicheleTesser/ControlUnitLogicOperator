#include "ebs.h"
#include "../../../../../../lib/raceup_board/raceup_board.h"
#include "../../../../core_utility/core_utility.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "../../../../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#pragma GCC diagnostic pop 

#include <stdint.h>
#include <stdio.h>
#include <string.h>

enum Tanks_t{
  TANK_LEFT=0,
  TANK_RIGHT,

  __NUM_OF_TANKS__
};

struct DvEbs_t{
  SharedMessageReader_h m_recv_ebs;
  struct CanMailbox* p_asb_consistency_check_mailbox_send;
  CarMissionReader_h m_mission_reader;
  float m_tank_vals[__NUM_OF_TANKS__];
  time_var_microseconds m_last_update;
  enum ASB_INTEGRITY_CHECK_RESULT m_asb_consistency_check;
  uint8_t m_sanity_check_right:1;
  uint8_t m_sanity_check_left:1;
  uint8_t m_system_check:1;
  uint8_t m_ebs_working_properly:1;
};

union DvEbs_h_t_conv{
  DvEbs_h* const restrict hidden;
  struct DvEbs_t* const restrict clear;
};

union DvEbs_h_t_conv_const{
  const DvEbs_h* const restrict hidden;
  const struct DvEbs_t* const restrict clear;
};

//private


#ifdef DEBUG
uint8_t __assert_size_dv_ebs[(sizeof(DvEbs_h) == sizeof(struct DvEbs_t))? 1:-1];
uint8_t __assert_align_dv_ebs[(_Alignof(DvEbs_h) == _Alignof(struct DvEbs_t))? 1:-1];
#endif

int8_t ebs_class_init(DvEbs_h* const restrict self)
{
  int8_t err=0;
  union DvEbs_h_t_conv conv = {self};
  struct DvEbs_t* const restrict p_self= conv.clear;
  struct CanNode* can_node = NULL;

  memset(p_self, 0, sizeof(*p_self));

  if(car_mission_reader_init(&p_self->m_mission_reader)<0)
  {
    return -1;
  }

  if (shared_message_reader_init(&p_self->m_recv_ebs, SHARED_MEX_EBSSTATUS))
  {
    goto error_mailbox_ebs;
  }

  ACTION_ON_CAN_NODE(CAN_GENERAL, can_node)
  {
    p_self->p_asb_consistency_check_mailbox_send = 
      hardware_get_mailbox_single_mex(
        can_node,
        SEND_MAILBOX,
        CAN_ID_CHECKASBREQ,
        message_dlc_can2(CAN_ID_CHECKASBREQ));
  }

  if (!p_self->p_asb_consistency_check_mailbox_send)
  {
    goto error_mailbox_consistency_check_send;
  }

  p_self->m_last_update = timer_time_now();
  p_self->m_asb_consistency_check = EBS_NO;

  return 0;

error_mailbox_consistency_check_send:
  hardware_free_mailbox_can(&p_self->p_asb_consistency_check_mailbox_send);
  err--;
error_mailbox_ebs:
  err--;

  return err;
}

int8_t ebs_update(DvEbs_h* const restrict self)
{
  union DvEbs_h_t_conv conv = {self};
  struct DvEbs_t* const p_self = conv.clear;
  can_obj_can2_h_t o2 = {0};

  if (car_mission_reader_update(&p_self->m_mission_reader)<0)
  {
    SET_TRACE(CORE_2);
    return -1;
  }

  if (car_mission_reader_get_current_mission(&p_self->m_mission_reader)<= CAR_MISSIONS_HUMAN)
  {
    p_self->m_ebs_working_properly = 0;
    p_self->m_asb_consistency_check = EBS_NO;
    return 0;
  }
  else
  {
    p_self->m_last_update = timer_time_now();
  }
  
  if (shared_message_read_unpack_can2(&p_self->m_recv_ebs,&o2)>0)
  {
    p_self->m_tank_vals[TANK_LEFT] = o2.can_0x03c_EbsStatus.press_left_tank;
    p_self->m_tank_vals[TANK_RIGHT] = o2.can_0x03c_EbsStatus.press_right_tank;

    p_self->m_sanity_check_left = o2.can_0x03c_EbsStatus.sanity_left_sensor;
    p_self->m_sanity_check_right = o2.can_0x03c_EbsStatus.sanity_right_sensor;

    p_self->m_system_check = o2.can_0x03c_EbsStatus.system_check;
    p_self->m_ebs_working_properly =1;
    p_self->m_asb_consistency_check = o2.can_0x03c_EbsStatus.ASB_check;

    if (p_self->m_asb_consistency_check == EBS_OK)
    {
      uint64_t payload =0;
      o2.can_0x068_CheckASBReq.req=1;
      o2.can_0x068_CheckASBReq.reqAck =1;
      pack_message_can2(&o2, CAN_ID_CHECKASBREQ, &payload);
      hardware_mailbox_send(p_self->p_asb_consistency_check_mailbox_send, payload);
    }

    p_self->m_last_update = timer_time_now();
  }

  if ((timer_time_now() - p_self->m_last_update) > get_tick_from_millis(500))
  {
    serial_write_str("ebs failed\n");
    p_self->m_asb_consistency_check = EBS_NO;
    p_self->m_ebs_working_properly =0;
  }

  return 0;
}

enum ASB_INTEGRITY_CHECK_RESULT ebs_asb_consistency_check(DvEbs_h* const restrict self)
{
  union DvEbs_h_t_conv conv = {self};
  struct DvEbs_t* const restrict p_self = conv.clear;
  can_obj_can2_h_t o2 ={0};
  uint64_t payload =0;

  if (p_self->m_asb_consistency_check == EBS_NO)
  {
    o2.can_0x068_CheckASBReq.req =1;
    pack_message_can2(&o2, CAN_ID_CHECKASBREQ, &payload);
    hardware_mailbox_send(p_self->p_asb_consistency_check_mailbox_send, payload);
  }

  return p_self->m_asb_consistency_check;
}

int8_t ebs_on(const DvEbs_h* const restrict self)
{
  const union DvEbs_h_t_conv_const conv = {self};
  const struct DvEbs_t* const restrict p_self= conv.clear;

  return p_self->m_ebs_working_properly;
}

#define BAR /*Pressure*/
int8_t ebs_activated(const DvEbs_h* const restrict self)
{
  const union DvEbs_h_t_conv_const conv = {self};
  const struct DvEbs_t* const restrict p_self= conv.clear;

  return 
    p_self->m_tank_vals[TANK_LEFT] > 90 BAR &&
    p_self->m_tank_vals[TANK_RIGHT] > 90 BAR;
}
