#include "batteries.h"
#include "bms/bms.h"
#include "../../../../lib/raceup_board/raceup_board.h"
#include "../../../core_utility/core_utility.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "../../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#pragma GCC diagnostic pop 

#include <stdint.h>
#include <string.h>

enum BMS_S{
  BMS_LV=0,
  BMS_HV,

  __NUM_OF_BMS__
};

struct CarBatteries_t{
  Bms_h m_bms[__NUM_OF_BMS__];
  float m_lem;
  SharedMessageReader_h m_recv_lem;
};

union CarBatteries_h_t_conv{
  CarBatteries_h* const restrict hidden;
  struct CarBatteries_t* const restrict clear;
};

#ifdef DEBUG
char __assert_size_core_1_batteries[(sizeof(CarBatteries_h) == sizeof(struct CarBatteries_t))? 1:-1];
char __assert_align_core_1_batteries[(_Alignof(CarBatteries_h) == _Alignof(struct CarBatteries_t))? 1:-1];
#endif // DEBUG

//public

int8_t car_batteries_init(CarBatteries_h* const restrict self, Log_h* const restrict log)
{
  union CarBatteries_h_t_conv conv = {self};
  struct CarBatteries_t* const restrict p_self = conv.clear;

  memset(p_self, 0, sizeof(*p_self));

  if (bms_init(&p_self->m_bms[BMS_LV], CAN_ID_BMSLV1, message_dlc_can2(CAN_ID_BMSLV1), "lv" , log))
  {
    return -2;
  }

  if (bms_init(&p_self->m_bms[BMS_HV], CAN_ID_BMSHV1, message_dlc_can2(CAN_ID_BMSHV1), "hv" , log))
  {
    return -3;
  }

  if (shared_message_reader_init(&p_self->m_recv_lem, SHARED_MEX_LEM))
  {
    return -4;
  }

  LogEntry_h entry =
  {
    .data_mode = __float__,
    .data_ptr = &p_self->m_lem,
    .log_mode = LOG_TELEMETRY | LOG_SD,
    .name = "lem_current",
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
  can_obj_can2_h_t o2 = {0};
  int8_t err=0;

  if(shared_message_read_unpack_can2(&p_self->m_recv_lem, &o2)>0)
  {
    p_self->m_lem = o2.can_0x3c2_Lem.current;
  }

  for (uint8_t i=0; i<__NUM_OF_BMS__; i++)
  {
    err--;
    if (bms_update(&p_self->m_bms[i])<0)
    {
      SET_TRACE(CORE_1);
      return err;
    }
  }

  return 0;
}
