#include "bms_hv.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "../src/lib/board_dbc/dbc/out_lib/can2/can2.h"
#pragma GCC diagnostic pop 
#include "../src/lib/raceup_board/raceup_board.h"
#include "../../linux_board/linux_board.h"
#include <stdint.h>
#include <string.h>
#include <threads.h>

struct BmsHv_t{
  struct{
    float min;
    float max;
    float mean;
  }Voltage;
  struct{
    float min;
    float max;
    float mean;
  }Temps;
  thrd_t thread;
  uint8_t soc;
  uint8_t fan_speed;
  uint8_t running:1;
  struct CanMailbox* mailbox_send_temps;
  struct CanMailbox* mailbox_send_volts;
};

union BmsHv_h_t_conv{
  BmsHv_h* const hidden;
  struct BmsHv_t* const clear;
};

union BmsHv_h_t_conv_const{
  const BmsHv_h* const hidden;
  const struct BmsHv_t* const clear;
};

#ifdef DEBUG
char __assert_size_bms_hv[(sizeof(BmsHv_h)==sizeof(struct BmsHv_t))?+1:-1];
char __assert_align_bms_hv[(_Alignof(BmsHv_h)==_Alignof(struct BmsHv_t))?+1:-1];
#endif /* ifdef DEBUG */

int _start_bms_hv(void* arg)
{
  struct BmsHv_t* const p_self = arg;
  can_obj_can2_h_t o2 = {0};
  uint64_t mex_data_volts = {0};
  uint64_t mex_data_temps = {0};
  time_var_microseconds t_var =0;

  while (p_self->running)
  {
    ACTION_ON_FREQUENCY(t_var, 50 MILLIS)
    {
      o2.can_0x057_BmsHv1.soc = p_self->soc;
      o2.can_0x057_BmsHv1.avg_volt = p_self->Voltage.mean;
      o2.can_0x057_BmsHv1.min_volt = p_self->Voltage.min;
      o2.can_0x057_BmsHv1.max_volt = p_self->Voltage.max;

      o2.can_0x058_BmsHv2.fan_speed = p_self->fan_speed;
      o2.can_0x058_BmsHv2.avg_temp = p_self->Temps.mean;
      o2.can_0x058_BmsHv2.min_temp = p_self->Temps.min;
      o2.can_0x058_BmsHv2.max_temp = p_self->Temps.max;

      pack_message_can2(&o2, CAN_ID_BMSHV1, &mex_data_volts);
      pack_message_can2(&o2, CAN_ID_BMSHV2, &mex_data_temps);

      hardware_mailbox_send(p_self->mailbox_send_volts, mex_data_volts);
      hardware_mailbox_send(p_self->mailbox_send_temps, mex_data_temps);
    }
  }

  return 0;
}


//public

int8_t bms_hv_start(struct BmsHv_h* const restrict self)
{
  union BmsHv_h_t_conv conv = {self};
  struct BmsHv_t* const p_self = conv.clear;
  struct CanNode* new_temp_node = NULL;

  memset(p_self, 0, sizeof(*p_self));

  new_temp_node = hardware_init_new_external_node(CAN_GENERAL);
  if (!new_temp_node)
  {
    return -1;
  }

  p_self->mailbox_send_volts=
    hardware_get_mailbox_single_mex(
        new_temp_node,
        SEND_MAILBOX,
        CAN_ID_BMSHV1,
        message_dlc_can2(CAN_ID_BMSHV1));

  p_self->mailbox_send_temps=
    hardware_get_mailbox_single_mex(
        new_temp_node,
        SEND_MAILBOX,
        CAN_ID_BMSHV2,
        message_dlc_can2(CAN_ID_BMSHV2));

  hardware_init_new_external_node_destroy(new_temp_node);

  thrd_create(&p_self->thread, _start_bms_hv, p_self);
  return 0;
}

int8_t bms_hv_set_attribute(struct BmsHv_h* const restrict self,
    const enum BMS_HV_ATTRIBUTE attribute, const float value)
{
  union BmsHv_h_t_conv conv = {self};
  struct BmsHv_t* const p_self = conv.clear;

  switch (attribute)
  {
    case BMS_HV_MIN_VOLTS:
      p_self->Voltage.min = value;
      break;
    case BMS_HV_MAX_VOLTS:
      p_self->Voltage.max = value;
      break;
    case BMS_HV_MEAN_VOLTS:
      p_self->Voltage.mean = value;
      break;
    case BMS_HV_MIN_TEMPS:
      p_self->Temps.mean = value;
      break;
    case BMS_HV_MAX_TEMPS:
      p_self->Temps.mean = value;
      break;
    case BMS_HV_MEAN_TEMPS:
      p_self->Temps.mean = value;
      break;
    case BMS_HV_MEAN_SOC:
      p_self->soc = value;
      break;
    case BMS_HV_MEAN_FAN_SPEED:
      p_self->fan_speed = value;
      break;
    case __NUM_OF_BMS_HV_ATTRIBUTES__:
      return -1;
  }
  return 0;
}

int8_t bms_hv_stop(struct BmsHv_h* const restrict self)
{
  union BmsHv_h_t_conv conv = {self};
  struct BmsHv_t* const p_self = conv.clear;

  printf("stopping bms hv\n");
  p_self->running=0;
  thrd_join(p_self->thread, NULL);

  hardware_free_mailbox_can(&p_self->mailbox_send_volts);
  hardware_free_mailbox_can(&p_self->mailbox_send_temps);

  return 0;
}
