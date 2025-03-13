#include "ebs.h"
#include "../../../../../../lib/raceup_board/raceup_board.h"
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
  struct CanMailbox* p_ebs_mailbox;
  struct CanMailbox* p_asb_consistency_check_mailbox_send;
  struct CanMailbox* p_asb_consistency_check_mailbox_recv;
  float tank_vals[__NUM_OF_TANKS__];
  time_var_microseconds last_update;
  struct{
    enum ASB_INTEGRITY_CHECK_RESULT asb_consistency_check;
    uint8_t done:1;
  }ConsistencyCheck;
  uint8_t sanity_check_right:1;
  uint8_t sanity_check_left:1;
  uint8_t system_check:1;
  uint8_t ebs_working_properly:1;
  uint8_t ebs_activated:1; //TODO: add to update function
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

#if DEBUG
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

  ACTION_ON_CAN_NODE(CAN_GENERAL, can_node)
  {
    p_self->p_ebs_mailbox = hardware_get_mailbox_single_mex(
        can_node,
        RECV_MAILBOX,
        CAN_ID_TANKSEBS,
        message_dlc_can2(CAN_ID_TANKSEBS));

  }
  if (!p_self->p_ebs_mailbox)
  {
    goto error_mailbox_ebs;
  }

  ACTION_ON_CAN_NODE(CAN_GENERAL, can_node)
  {
    p_self->p_asb_consistency_check_mailbox_send = 
      hardware_get_mailbox_single_mex(
        can_node,
        SEND_MAILBOX,
        CAN_ID_CHECKASB,
        message_dlc_can2(CAN_ID_CHECKASB));
  }

  if (!p_self->p_asb_consistency_check_mailbox_send)
  {
    goto error_mailbox_consistency_check_send;
  }

  ACTION_ON_CAN_NODE(CAN_GENERAL, can_node)
  {
    p_self->p_asb_consistency_check_mailbox_recv = 
      hardware_get_mailbox_single_mex(
        can_node,
        RECV_MAILBOX,
        CAN_ID_CHECKASB,
        message_dlc_can2(CAN_ID_CHECKASB));
  }

  if (!p_self->p_asb_consistency_check_mailbox_recv)
  {
    goto error_mailbox_consistency_check_recv;
  }

  return 0;

error_mailbox_consistency_check_recv:
  hardware_free_mailbox_can(&p_self->p_ebs_mailbox);
  err--;
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
  struct DvEbs_t* const restrict p_self = conv.clear;
  CanMessage mex = {0};
  can_obj_can2_h_t o2 = {0};

  if (hardware_mailbox_read(p_self->p_ebs_mailbox, &mex))
  {
    unpack_message_can2(&o2, mex.id, mex.full_word, mex.message_size, timer_time_now());
    p_self->tank_vals[TANK_LEFT] = o2.can_0x03c_TanksEBS.press_left_tank;
    p_self->tank_vals[TANK_RIGHT] = o2.can_0x03c_TanksEBS.press_right_tank;

    p_self->sanity_check_left = o2.can_0x03c_TanksEBS.sanity_left_sensor;
    p_self->sanity_check_right = o2.can_0x03c_TanksEBS.sanity_right_sensor;

    p_self->system_check = o2.can_0x03c_TanksEBS.system_check;
    p_self->ebs_working_properly =1;

    p_self->last_update = timer_time_now();
  }

  if (hardware_mailbox_read(p_self->p_asb_consistency_check_mailbox_recv, &mex))
  {
    unpack_message_can2(&o2, mex.id, mex.full_word, mex.message_size, timer_time_now());
    if (o2.can_0x068_CheckASB.Mode)
    {
      p_self->ConsistencyCheck.asb_consistency_check = o2.can_0x068_CheckASB.response_status;
      p_self->ConsistencyCheck.done = 1;
    }
  }

  if ((timer_time_now() - p_self->last_update) > 500 MILLIS)
  {
    p_self->ebs_working_properly =0;
  }

  return 0;
}

enum ASB_INTEGRITY_CHECK_RESULT ebs_asb_consistency_check(DvEbs_h* const restrict self)
{
  union DvEbs_h_t_conv conv = {self};
  struct DvEbs_t* const restrict p_self = conv.clear;

  if (!p_self->ConsistencyCheck.done)
  {
    hardware_mailbox_send(p_self->p_asb_consistency_check_mailbox_send, 0);
    return EBS_NOT_YET_DONE;
  }

  return p_self->ConsistencyCheck.asb_consistency_check;
}

int8_t ebs_on(const DvEbs_h* const restrict self)
{
  const union DvEbs_h_t_conv_const conv = {self};
  const struct DvEbs_t* const restrict p_self= conv.clear;

  return p_self->ebs_working_properly;
}

int8_t ebs_activated(const DvEbs_h* const restrict self)
{
  const union DvEbs_h_t_conv_const conv = {self};
  const struct DvEbs_t* const restrict p_self= conv.clear;

  return p_self->ebs_activated;
}
