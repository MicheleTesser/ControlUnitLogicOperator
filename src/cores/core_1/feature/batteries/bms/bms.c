#include "bms.h"
#include "../../log/log.h"
#include "../../../../../lib/raceup_board/raceup_board.h"
#include "../../../../core_utility/core_utility.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "../../../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#pragma GCC diagnostic pop 

#include <stdint.h>
#include <string.h>

enum VOLTS{
  MAX=0,
  MIN,
  AVG,

  __NUM_OF_VOLTS__
};

struct Bms_t{
  struct CanMailbox* p_mailbox;
  uint16_t m_volts[__NUM_OF_VOLTS__];
  uint8_t m_soc;
};

union Hv_h_t_conv{
  Bms_h* const restrict hidden;
  struct Bms_t* const restrict clear;
};

#ifdef DEBUG
char __assert_size_core_1_bms[(sizeof(Bms_h) == sizeof(struct Bms_t))? 1:-1];
char __assert_align_core_1_bms[(_Alignof(Bms_h) == _Alignof(struct Bms_t))? 1:-1];
#endif // DEBUG

int8_t bms_init(Bms_h* const restrict self ,
    const uint16_t bms_id, const uint8_t mex_size,
    const char* const restrict bms_name,
    Log_h* const restrict log)
{
  union Hv_h_t_conv conv = {self};
  struct Bms_t* const restrict p_self = conv.clear;
  struct CanNode* can_node = NULL;

  memset(p_self, 0, sizeof(*p_self));

  ACTION_ON_CAN_NODE(CAN_GENERAL, can_node)
  {
    p_self->p_mailbox =
      hardware_get_mailbox_single_mex(can_node, RECV_MAILBOX, bms_id, mex_size);
  }
  if (!p_self->p_mailbox)
  {
    SET_TRACE(CORE_1);
    return -1;
  }

  {
    LogEntry_h entry = {
      .data_mode = __u16__,
      .data_ptr = &p_self->m_volts[MAX],
      .log_mode = LOG_TELEMETRY| LOG_SD,
      .name = {0},
    };
    strncat(entry.name, "max_", 5);
    strncat(entry.name, bms_name, 5);
    strncat(entry.name, "_volt", 6);
    if(log_add_entry(log, &entry)<0)
    {
      SET_TRACE(CORE_1);
      return -1;
    }
  }

  {
    LogEntry_h entry = {
      .data_mode = __u16__,
      .data_ptr = &p_self->m_volts[MIN],
      .log_mode = LOG_TELEMETRY| LOG_SD,
      .name = {0},
    };
    strncat(entry.name, "min_", 5);
    strncat(entry.name, bms_name, 5);
    strncat(entry.name, "_volt", 6);
    if(log_add_entry(log, &entry)<0)
    {
      SET_TRACE(CORE_1);
      return -2;
    }
  }


  {
    LogEntry_h entry = {
      .data_mode = __u8__,
      .data_ptr = &p_self->m_soc,
      .log_mode = LOG_TELEMETRY| LOG_SD,
      .name = {0},
    };
    strncat(entry.name, bms_name, 5);
    strncat(entry.name, "_soc", 6);
    if(log_add_entry(log, &entry)<0)
    {
      SET_TRACE(CORE_1);
      return -3;
    }
  }

  return 0;
}

  int8_t
bms_update(Bms_h* const restrict self )
{
  union Hv_h_t_conv conv = {self};
  struct Bms_t* const restrict p_self = conv.clear;
  can_obj_can2_h_t o = {0};
  CanMessage mex = {0};

  if (hardware_mailbox_read(p_self->p_mailbox, &mex))
  {
    unpack_message_can2(&o, mex.id, mex.full_word, mex.message_size, timer_time_now());
    p_self->m_volts[MAX] = o.can_0x057_BmsHv1.max_volt;
    p_self->m_volts[MIN] = o.can_0x057_BmsHv1.min_volt;
    p_self->m_volts[AVG] = o.can_0x057_BmsHv1.avg_volt;
    p_self->m_soc = o.can_0x057_BmsHv1.soc;
  }

  return 0;
}
