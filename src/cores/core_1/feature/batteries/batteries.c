#include "batteries.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wconversion"
#include "../../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#pragma GCC diagnostic pop 
#include "../../../../lib/raceup_board/raceup_board.h"
#include "bms/bms.h"

#include <stdint.h>
#include <string.h>

enum BMS_S{
  BMS_LV=0,
  BMS_HV,

  __NUM_OF_BMS__
};

struct CarBatteries_t{
  Bms_h bms[__NUM_OF_BMS__];
  float lem;
  struct CanMailbox* p_mailbox_read_lem;
};

union CarBatteries_h_t_conv{
  CarBatteries_h* const restrict hidden;
  struct CarBatteries_t* const restrict clear;
};

#ifdef DEBUG
char __assert_size_core_1_batteries[(sizeof(CarBatteries_h) == sizeof(struct CarBatteries_t))? 1:-1];
char __assert_align_core_1_batteries[(_Alignof(CarBatteries_h) == _Alignof(struct CarBatteries_t))? 1:-1];
#endif // DEBUG

//private

//public

int8_t car_batteries_init(CarBatteries_h* const restrict self, Log_h* const restrict log)
{
  union CarBatteries_h_t_conv conv = {self};
  struct CarBatteries_t* const restrict p_self = conv.clear;
  struct CanNode* can_node = NULL;;

  memset(p_self, 0, sizeof(*p_self));

  if (bms_init(&p_self->bms[BMS_LV], CAN_ID_BMSLV1, (uint8_t) message_dlc_can2(CAN_ID_BMSLV1), "BMS LV" , log))
  {
    return -2;
  }

  if (bms_init(&p_self->bms[BMS_HV], CAN_ID_BMSHV1, (uint8_t) message_dlc_can2(CAN_ID_BMSHV1), "BMS HV" , log))
  {
    return -3;
  }

  ACTION_ON_CAN_NODE(CAN_GENERAL, can_node)
  {
    p_self->p_mailbox_read_lem =
      hardware_get_mailbox_single_mex(
          can_node,
          RECV_MAILBOX,
          CAN_ID_LEM,
          (uint8_t) message_dlc_can2(CAN_ID_LEM));
  }

  if (!p_self->p_mailbox_read_lem)
  {
    return -4;
  }

  LogEntry_h entry =
  {
    .data_mode = __float__,
    .data_ptr = &p_self->lem,
    .log_mode = LOG_TELEMETRY | LOG_SD,
    .name = "lem current",
  };

  if(log_add_entry(log, &entry)<0)
  {
    return -5;
  }

  return 0;
}

int8_t car_batteries_update(CarBatteries_h* const restrict self)
{
  union CarBatteries_h_t_conv conv = {self};
  struct CarBatteries_t* const restrict p_self = conv.clear;
  CanMessage mex = {0};
  can_obj_can2_h_t o2 = {0};
  int8_t err=0;

  if (hardware_mailbox_read(p_self->p_mailbox_read_lem, &mex))
  {
    unpack_message_can2(&o2, mex.id, mex.full_word, mex.message_size,0); 
    p_self->lem = (float) o2.can_0x3c2_Lem.current;
  }

  for (uint8_t i=0; i<__NUM_OF_BMS__; i++)
  {
    err--;
    if (bms_update(&p_self->bms[i])<0)
    {
      return err;
    }
  }

  return 0;
}
