#include "can_log.h"
#include "../external_log_variables/external_log_variables.h"
#include "../../../../../lib/raceup_board/raceup_board.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "../../../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#pragma GCC diagnostic pop 
#include "../../../../core_utility/core_utility.h"

#include <stdint.h>
#include <string.h>

struct CanLog_t{
  time_var_microseconds m_last_sent;
  GpioRead_h m_start_precharge_gpio;
  GpioRead_h m_done_precharge_gpio;
  AsNodeRead_h m_as_node_read;
  const uint16_t* p_log_var_amk_status_fl;
  const uint16_t* p_log_var_amk_status_fr;
  const uint16_t* p_log_var_amk_status_rl;
  const uint16_t* p_log_var_amk_status_rr;
  struct CanMailbox* p_mailbox_send_car_status;
};

union CanLog_h_t_conv{
  CanLog_h* const hidden;
  struct CanLog_t* const clear;
};

#ifdef DEBUG
char __assert_size_can_log[sizeof(CanLog_h)==sizeof(struct CanLog_t)?+1:-1];
char __assert_align_can_log[_Alignof(CanLog_h)==_Alignof(struct CanLog_t)?+1:-1];
#define CHECK_INIT(self)\
if (!self->p_mailbox_send_car_status)\
{\
  SET_TRACE(CORE_1);\
  return -1;\
}
#else
#define CHECK_INIT(self)
#endif /* ifdef DEBUG */

//public

int8_t can_log_init(CanLog_h* const restrict self)
{
  union CanLog_h_t_conv conv = {self};
  struct CanLog_t* const p_self = conv.clear;
  struct CanNode* can_node = NULL;

  memset(p_self, 0, sizeof(*p_self));

  if (as_node_read_init(&p_self->m_as_node_read)<0)
  {
    return -1;
  }

  p_self->p_log_var_amk_status_fl = external_log_extract_data_ptr_r_only(AMK_STATUS_FL);
  p_self->p_log_var_amk_status_rl = external_log_extract_data_ptr_r_only(AMK_STATUS_RL);

  p_self->p_log_var_amk_status_fr = external_log_extract_data_ptr_r_only(AMK_STATUS_FR);
  p_self->p_log_var_amk_status_rr = external_log_extract_data_ptr_r_only(AMK_STATUS_RR);

  if (!p_self->p_log_var_amk_status_fl ||
      !p_self->p_log_var_amk_status_fr ||
      !p_self->p_log_var_amk_status_rr ||
      !p_self->p_log_var_amk_status_rl)
  {
    SET_TRACE(CORE_1);
    return -2;
  }

  if (hardware_init_read_permission_gpio(&p_self->m_start_precharge_gpio, GPIO_AIR_PRECHARGE_INIT)<0)
  {
    SET_TRACE(CORE_1);
    return -3;
  }

  if (hardware_init_read_permission_gpio(&p_self->m_done_precharge_gpio, GPIO_AIR_PRECHARGE_DONE)<0)
  {
    SET_TRACE(CORE_1);
    return -4;
  }

  ACTION_ON_CAN_NODE(CAN_GENERAL, can_node)
  {
    p_self->p_mailbox_send_car_status =
      hardware_get_mailbox_single_mex(
          can_node,
          SEND_MAILBOX,
          CAN_ID_CARSTATUS,
          message_dlc_can2(CAN_ID_CARSTATUS));
  }

  if (!p_self->p_mailbox_send_car_status)
  {
    SET_TRACE(CORE_1);
    return -5;
  }

  return 0;
}

int8_t can_log_update(CanLog_h* const restrict self)
{
  union CanLog_h_t_conv conv = {self};
  struct CanLog_t* const p_self = conv.clear;
  can_obj_can2_h_t o2 = {0};
  uint64_t payload = {0};

  CHECK_INIT(p_self);

  // uint16_t f = 0b00001000;

  ACTION_ON_FREQUENCY(p_self->m_last_sent, get_tick_from_millis(100))
  {
    //HACK: look amk.c AMK_Actual_Values_1 to check if the position checked with the shift are right
    o2.can_0x065_CarStatus.HV= 
      !!((*p_self->p_log_var_amk_status_fl >> 8) & (1<<3)) ||
      !!((*p_self->p_log_var_amk_status_fr >> 8) & (1<<3)) ||
      !!((*p_self->p_log_var_amk_status_rl >> 8) & (1<<3)) ||
      !!((*p_self->p_log_var_amk_status_rr >> 8) & (1<<3));


    o2.can_0x065_CarStatus.AIR1= !!gpio_read_state(&p_self->m_start_precharge_gpio);
    o2.can_0x065_CarStatus.precharge = !!gpio_read_state(&p_self->m_done_precharge_gpio);
    o2.can_0x065_CarStatus.AS_NODE= as_node_read_get_status(&p_self->m_as_node_read);
    o2.can_0x065_CarStatus.RunningStatus = (uint8_t) global_running_status_get();
    o2.can_0x065_CarStatus.speed = (uint8_t) car_speed_get();

    pack_message_can2(&o2, CAN_ID_CARSTATUS, &payload);

    return hardware_mailbox_send(p_self->p_mailbox_send_car_status, payload);
  }

  return 0;
}
