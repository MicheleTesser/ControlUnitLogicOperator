#include "ebs.h"
#include "../../../../../../lib/raceup_board/raceup_board.h"
#include "../../../../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#include <stdint.h>
#include <string.h>

struct DvEbs_t{
  struct CanMailbox* p_ebs_mailbox;
  struct CanMailbox* p_asb_consistency_check_mailbox_send;
  struct CanMailbox* p_asb_consistency_check_mailbox_recv;
  struct{
    GpioRead_h o_gpio_precharge_init;
    GpioRead_h o_gpio_precharge_done;
  }SDC;
  uint8_t asb_consistency_check_done;
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
        SEND_MAILBOX,
        CAN_ID_CHECKASB,
        message_dlc_can2(CAN_ID_CHECKASB));
  }

  if (!p_self->p_asb_consistency_check_mailbox_recv)
  {
    goto error_mailbox_consistency_check_recv;
  }

  if (hardware_init_read_permission_gpio(&p_self->SDC.o_gpio_precharge_init, GPIO_AIR_PRECHARGE_INIT)<0)
  {
    goto error_gpio_precharge_init; 
  }

  if (hardware_init_read_permission_gpio(&p_self->SDC.o_gpio_precharge_done, GPIO_AIR_PRECHARGE_DONE)<0)
  {
    goto error_gpio_precharge_done; 
  }


  return 0;

error_gpio_precharge_done:
  err--;
error_gpio_precharge_init:
  err--;
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

  if (!hardware_mailbox_read(p_self->p_ebs_mailbox, &mex))
  {
    unpack_message_can2(&o2, mex.id, mex.full_word, mex.message_size, timer_time_now());
  }

  if (!hardware_mailbox_read(p_self->p_asb_consistency_check_mailbox_recv, &mex))
  {
    unpack_message_can2(&o2, mex.id, mex.full_word, mex.message_size, timer_time_now());
    p_self->asb_consistency_check_done = o2.can_0x068_CheckASB.response_status;
  }

  return 0;
}

int8_t ebs_asb_consistency_check(DvEbs_h* const restrict self)
{
  union DvEbs_h_t_conv conv = {self};
  struct DvEbs_t* const restrict p_self = conv.clear;

  if (!p_self->asb_consistency_check_done) {
    
  }

  return p_self->asb_consistency_check_done;
}

int8_t ebs_on(const DvEbs_h* const restrict self)
{
  const union DvEbs_h_t_conv_const conv = {self};
  const struct DvEbs_t* const restrict p_self= conv.clear;

  return 
    gpio_read_state(&p_self->SDC.o_gpio_precharge_init) &&
    gpio_read_state(&p_self->SDC.o_gpio_precharge_done);
}
