#include "hv.h"
#include "../../../../../../lib/raceup_board/raceup_board.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "../../../../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#pragma GCC diagnostic pop 
#include <stdint.h>
#include <string.h>

struct GieiHv_t{
  struct CanMailbox* lem_mailbox;
  struct CanMailbox* send_mailbox_bms_hv;
  float hv_public_data[__NUM_OF_GIEI_HV_INFO__];
  float lem_current;
  float pack_tension;
};

union GieiHv_conv{
  Hv_h* const hidden;
  struct GieiHv_t* const clear;
};

union GieiHv_const_conv{
  const Hv_h* const  hidden;
  const struct GieiHv_t* const  clear;
};

#ifdef DEBUG
char __assert_size_GieiHv[(sizeof(Hv_h) == sizeof(struct GieiHv_t))? 1:-1];
char __assert_align_GieiHv[(_Alignof(Hv_h) == _Alignof(struct GieiHv_t))? 1:-1];
#endif // DEBUG


//public

int8_t hv_init(Hv_h* const restrict self)
{
  union GieiHv_conv conv = {self};
  struct GieiHv_t* p_self = conv.clear;
  struct CanNode* can_node = NULL;

  memset(p_self, 0, sizeof(*p_self));
  ACTION_ON_CAN_NODE(CAN_GENERAL,can_node)
  {
    p_self->lem_mailbox =
      hardware_get_mailbox_single_mex(
          can_node,
          RECV_MAILBOX,
          CAN_ID_LEM,
          message_dlc_can2(CAN_ID_LEM));

    if (!p_self->lem_mailbox)
    {
      return -1;
    }

    p_self->send_mailbox_bms_hv =
      hardware_get_mailbox_single_mex(
          can_node,
          SEND_MAILBOX,
          CAN_ID_INVVOLT,
          message_dlc_can2(CAN_ID_INVVOLT));

    if (!p_self->send_mailbox_bms_hv)
    {
      hardware_free_mailbox_can(&p_self->lem_mailbox);
    }
  }

  return 0;
}

int8_t hv_update(Hv_h* const restrict self)
{
  union GieiHv_conv conv = {self};
  struct GieiHv_t* p_self = conv.clear;
  CanMessage mex; 
  if(hardware_mailbox_read(p_self->lem_mailbox,&mex)>=0)
  {
    can_obj_can2_h_t o2= {0};
    unpack_message_can2(&o2, mex.id, mex.full_word, mex.message_size, timer_time_now());

    union {
      uint32_t u32;
      float f;
    }conv;
    conv.u32 = o2.can_0x3c2_Lem.current;
    p_self->lem_current = conv.f;
  }

  return 0;
}

float hv_get_info(const Hv_h* const restrict self, const enum GIEI_HV_INFO info)
{
  union GieiHv_const_conv conv = {self};
  const struct GieiHv_t* const p_self = conv.clear;

  if (info!=__NUM_OF_GIEI_HV_INFO__)
  {
    return p_self->hv_public_data[info];
  }
  return -1;
}

/*
 * Battery pack tension is given indipendently by every motor.
 * The function seems complex because takes in consideration the case
 * that one or more motor are inactive.
 *
 * BMS precharge needs a message with the tot voltage
 */
int8_t hv_computeBatteryPackTension(Hv_h* const restrict self, 
    const float* const engines_voltages, const uint8_t num_of_voltages)
{
  union GieiHv_conv conv = {self};
  struct GieiHv_t* const restrict p_self = conv.clear;
  uint8_t active_motors = 0;
  float sum = 0.0f;
  uint8_t max = 0;
  can_obj_can2_h_t o;
  uint64_t data_mex=0;

  // find max voltage
  for (uint8_t i = 0; i < num_of_voltages; i++)
  {
    if (engines_voltages[i] > max)
    {
      max = engines_voltages[i];
    }
  }

  // Compute sum of voltages, exclude if it is below 50 V than the maximum reading
  for (uint8_t i = 0; i < num_of_voltages; i++)
  {
    if (engines_voltages[i] > (max - 50))
    {
      active_motors++;
      sum += engines_voltages[i];
    }
  }

  if (!active_motors) {
    p_self->hv_public_data[HV_BATTERY_PACK_TENSION]= 0;
    p_self->hv_public_data[HV_TOTAL_POWER]= 0;
  }
  else {
    p_self->pack_tension = sum/ active_motors;
    p_self->hv_public_data[HV_BATTERY_PACK_TENSION]= p_self->pack_tension;
    p_self->hv_public_data[HV_TOTAL_POWER]= p_self->pack_tension * p_self->lem_current;
  }

  o.can_0x120_InvVolt.car_voltage = p_self->pack_tension;
  pack_message_can2(&o, CAN_ID_INVVOLT, &data_mex);
  return hardware_mailbox_send(p_self->send_mailbox_bms_hv, data_mex);
}
