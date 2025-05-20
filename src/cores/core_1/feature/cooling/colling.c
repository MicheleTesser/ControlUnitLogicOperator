#include "colling.h"
#include "../../../../lib/raceup_board/components/can.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "../../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#pragma GCC diagnostic pop 
#include "../../../core_utility/core_utility.h"

#include <stdint.h>
#include <string.h>

#define MAX_MAILBOX_FOR_DEVICE 4

struct CoolingDevice_t{
  uint8_t enable:1;
  uint8_t speed:7;
};

struct Cooling_t{
  struct CoolingDevice_t devices[__NUM_OF_COOLING_DEVICES__];
  struct CanMailbox* p_send_mailbox_pcu;
  struct CanMailbox* p_recv_mailbox_stw;
};

union Cooling_h_t_conv{
  Cooling_h* hidden;
  struct Cooling_t* clear;
};

#ifdef DEBUG
char __assert_size_cooling[(sizeof(Cooling_h) == sizeof(struct Cooling_t))? 1:-1];
char __assert_align_cooling[(_Alignof(Cooling_h) == _Alignof(struct Cooling_t))? 1:-1];
#endif // DEBUG

//public

int8_t cooling_init(Cooling_h* const restrict self ,
    Log_h* const restrict log )
{
  union Cooling_h_t_conv conv = {self};
  struct Cooling_t* const p_self = conv.clear;
  struct CanNode* can_node = NULL;

  memset(p_self, 0, sizeof(*p_self));


  {
    LogEntry_h entry ={
      .data_mode = __u8__,
      .data_ptr = &p_self->devices[FANS_RADIATOR],
      .log_mode = LOG_SD | LOG_TELEMETRY,
      .name = "temp fan speed/enable",
    };
    if (log_add_entry(log, &entry)<0)
    {
      SET_TRACE(CORE_1);
      return -2;
    }
  }

  {
    LogEntry_h entry ={
      .data_mode = __u8__,
      .data_ptr = &p_self->devices[PUMPS],
      .log_mode = LOG_SD | LOG_TELEMETRY,
      .name = "temp pump speed/enable",
    };
    if (log_add_entry(log, &entry)<0)
    {
      SET_TRACE(CORE_1);
      return -3;
    }
  }

  ACTION_ON_CAN_NODE(CAN_GENERAL,can_node)
  {
    p_self->p_send_mailbox_pcu =
      hardware_get_mailbox_single_mex(
          can_node,
          SEND_MAILBOX,
          CAN_ID_PCU,
          message_dlc_can2(CAN_ID_PCU));

    p_self->p_recv_mailbox_stw =
      hardware_get_mailbox_single_mex(
          can_node,
          RECV_MAILBOX,
          CAN_ID_PCUSWCONTROL,
          message_dlc_can2(CAN_ID_PCUSWCONTROL));
  }
  if (!p_self->p_send_mailbox_pcu)
  {
    SET_TRACE(CORE_1);
    return -4;
  }

  if (!p_self->p_recv_mailbox_stw)
  {
    hardware_free_mailbox_can(&p_self->p_send_mailbox_pcu);
    SET_TRACE(CORE_1);
    return -5;
  }

  return 0;
}

int8_t cooling_update(Cooling_h* const restrict self)
{
  union Cooling_h_t_conv conv = {self};
  struct Cooling_t* const p_self = conv.clear;
  can_obj_can2_h_t o2;
  CanMessage mex;

  if (global_running_status_get() > SYSTEM_OFF)
  {
    p_self->devices[FANS_RADIATOR].speed = 100;
    p_self->devices[PUMPS].speed = 100;
  }

  if (hardware_mailbox_read(p_self->p_recv_mailbox_stw, &mex))
  {
    unpack_message_can2(&o2, mex.id, mex.full_word, mex.message_size, 0);

    p_self->devices[FANS_RADIATOR].enable = o2.can_0x133_PcuSwControl.fan;
    p_self->devices[PUMPS].enable = o2.can_0x133_PcuSwControl.pump;
  
    mex.id = CAN_ID_PCU;
    o2.can_0x130_Pcu.mode = 0;
    o2.can_0x130_Pcu.fanrad_enable_left = p_self->devices[FANS_RADIATOR].enable;
    o2.can_0x130_Pcu.fanrad_enable_right = p_self->devices[FANS_RADIATOR].enable;
    o2.can_0x130_Pcu.fanrad_speed_left = p_self->devices[FANS_RADIATOR].speed;
    o2.can_0x130_Pcu.fanrad_speed_left = p_self->devices[FANS_RADIATOR].speed;

    o2.can_0x130_Pcu.pump_enable_left = p_self->devices[PUMPS].enable;
    o2.can_0x130_Pcu.pump_enable_right = p_self->devices[PUMPS].enable;
    o2.can_0x130_Pcu.pump_speed_left = p_self->devices[PUMPS].speed;
    o2.can_0x130_Pcu.pump_speed_right = p_self->devices[PUMPS].speed;

    mex.message_size = pack_message_can2(&o2, mex.id, &mex.full_word);
    return hardware_mailbox_send(p_self->p_send_mailbox_pcu, mex.full_word);
  }


  return 0;

}
