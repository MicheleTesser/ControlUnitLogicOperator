#include "as_node.h"
#include "../mission_reader/mission_reader.h"
#include "../shared_message/shared_message.h"
#include "../../../lib/raceup_board/raceup_board.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#include "../../../lib/board_dbc/dbc/out_lib/can3/can3.h"
#pragma GCC diagnostic pop 

#include <stdatomic.h>
#include <stdint.h>
#include <string.h>

enum Tanks_t{
  TANK_LEFT=0,
  TANK_RIGHT,

  __NUM_OF_TANKS__
};

struct AsNodeRead_t{
  GpioRead_h m_gpio_read_as_node;
};

struct AsNode_t{
  Gpio_h m_gpio_as_node;
  time_var_microseconds m_last_alive_message_received;
  time_var_microseconds m_last_tank_ebs_message_received;
  time_var_microseconds m_last_res_message_received;
  time_var_microseconds m_last_enable_dv_sent;
  uint8_t m_ebs_check_ok:1;
  SharedMessageReader_h m_recv_embedded_alive;
  SharedMessageReader_h m_recv_tank_ebs;
  SharedMessageReader_h m_recv_res_on;
  SharedMessageReader_h m_recv_ebs_pressure;
  struct CanMailbox* p_mailbox_send_pcu_enable_dv;
  CarMissionReader_h* p_mission_reader;
};


union AsNode_h_t_conv{
  AsNode_h* const hidden;
  struct AsNode_t* const clear;
};

union AsNode_h_t_conv_const{
  const AsNode_h* const hidden;
  const struct AsNode_t* const clear;
};

union AsNodeRead_h_t_conv{
  AsNodeRead_h* const hidden;
  struct AsNodeRead_t* const clear;
};

union AsNodeRead_h_t_conv_const{
  const AsNodeRead_h* const hidden;
  const struct AsNodeRead_t* const clear;
};

#ifdef DEBUG
char __assert_size_as_node[(sizeof(AsNode_h)==sizeof(struct AsNode_t))?+1:-1];
char __assert_align_as_node[(_Alignof(AsNode_h)==_Alignof(struct AsNode_t))?+1:-1];

char __assert_size_as_node_read[(sizeof(AsNodeRead_h)==sizeof(struct AsNodeRead_t))?+1:-1];
char __assert_align_as_node_read[(_Alignof(AsNodeRead_h)==_Alignof(struct AsNodeRead_t))?+1:-1];
#endif /* ifdef DEBUG */

//private

static struct{
  atomic_bool m_owned;
}AS_NODE;

static inline uint8_t _check_embedded(struct AsNode_t *const restrict self)
{
  return (timer_time_now() - self->m_last_alive_message_received) < get_tick_from_millis(500);
}

static inline uint8_t _check_ebs(struct AsNode_t *const restrict self)
{
  return (timer_time_now() - self->m_last_tank_ebs_message_received) < get_tick_from_millis(500);
}

static inline uint8_t _check_res(struct AsNode_t* const restrict self)
{
  return  (timer_time_now() - self->m_last_res_message_received) < get_tick_from_millis(500);
}

static inline uint8_t _check_brakes(struct AsNode_t* const restrict self)
{
  return self->m_ebs_check_ok;
}

static inline void _as_node_enable(struct AsNode_t* const restrict self)
{
  gpio_set_high(&self->m_gpio_as_node);
}

static inline void _as_node_disable(struct AsNode_t* const restrict self)
{
  gpio_set_low(&self->m_gpio_as_node);
}

//public

int8_t as_node_init(AsNode_h* const restrict self,
    CarMissionReader_h* const restrict p_car_mission_reader)
{
  union AsNode_h_t_conv conv = {self};
  struct AsNode_t* const p_self = conv.clear;
  struct CanNode* can_node = NULL;
  int8_t err=0;

  if (atomic_load(&AS_NODE.m_owned))
  {
    return -1;
  }

  atomic_store(&AS_NODE.m_owned, 1);
  memset(p_self, 0, sizeof(*p_self));

  if (hardware_init_gpio(&p_self->m_gpio_as_node, GPIO_AS_NODE)<0)
  {
    err=-2;
    goto err;
  }

  if (shared_message_reader_init(&p_self->m_recv_embedded_alive, SHARED_MEX_EMBEDDEDALIVECHECK))
  {
    err=-3;
    goto err;
  }

  if (shared_message_reader_init(&p_self->m_recv_res_on, SHARED_MEX_DV_RES_ON))
  {
    err=-4;
    goto err;
  }


  if (shared_message_reader_init(&p_self->m_recv_tank_ebs, SHARED_MEX_EBSSTATUS))
  {
    err=-5;
    goto err;
  }

  if (shared_message_reader_init(&p_self->m_recv_ebs_pressure, SHARED_MEX_EBSSTATUS))
  {
    err=-6;
    goto err;
  }

  ACTION_ON_CAN_NODE(CAN_GENERAL, can_node)
  {

    p_self->p_mailbox_send_pcu_enable_dv =
      hardware_get_mailbox_single_mex(
          can_node,
          SEND_MAILBOX,
          CAN_ID_PCU,
          message_dlc_can2(CAN_ID_PCU));
  }

  if (!p_self->p_mailbox_send_pcu_enable_dv)
  {
    err=-7;
    goto err;
  }


  p_self->p_mission_reader = p_car_mission_reader;

  return 0;

err:
  atomic_store(&AS_NODE.m_owned, 0);
  return err;
}

int8_t as_node_update(AsNode_h* const restrict self)
{
  union AsNode_h_t_conv conv = {self};
  struct AsNode_t* const p_self = conv.clear;
  CanMessage mex = {0};
  can_obj_can2_h_t o2 = {0};
  uint64_t pcu_payload = 0;

  if (shared_message_read(&p_self->m_recv_embedded_alive, &mex.full_word))
  {
    p_self->m_last_alive_message_received = timer_time_now();
  }

  if (shared_message_read(&p_self->m_recv_tank_ebs, &mex.full_word))
  {
    p_self->m_last_tank_ebs_message_received = timer_time_now();
  }

  if (shared_message_read(&p_self->m_recv_res_on, &mex.full_word))
  {
    p_self->m_last_res_message_received = timer_time_now();
  }

  if (shared_message_read_unpack_can2(&p_self->m_recv_ebs_pressure, &o2)>0)
  {
    p_self->m_ebs_check_ok = o2.can_0x03c_EbsStatus.system_check;
  }


  switch (car_mission_reader_get_current_mission(p_self->p_mission_reader))
  {
    case CAR_MISSIONS_HUMAN:
      if (!_check_ebs(p_self))
      {
        _as_node_enable(p_self);
      }
      else
      {
        _as_node_disable(p_self);
      }
      break;
    case CAR_MISSIONS_DV_SKIDPAD:
    case CAR_MISSIONS_DV_AUTOCROSS:
    case CAR_MISSIONS_DV_TRACKDRIVE:
    case CAR_MISSIONS_DV_EBS_TEST:
    case CAR_MISSIONS_DV_INSPECTION:
      if (_check_ebs(p_self) && _check_embedded(p_self) && _check_res(p_self) && _check_brakes(p_self))
      {
        _as_node_enable(p_self);
      }
      else
      {
        _as_node_disable(p_self);
      }
      o2.can_0x130_Pcu.enable_dv = 1;
      o2.can_0x130_Pcu.enable_embedded = 1;
      break;
    default:
      _as_node_disable(p_self);
      break;
  }

  o2.can_0x130_Pcu.mode =2;

  ACTION_ON_FREQUENCY(p_self->m_last_enable_dv_sent, get_tick_from_millis(100))
  {
    pack_message_can2(&o2, CAN_ID_PCU, &pcu_payload);
    hardware_mailbox_send(p_self->p_mailbox_send_pcu_enable_dv, pcu_payload);
  }

  return 0;

}

int8_t as_node_read_init(AsNodeRead_h* const restrict self)
{
  union AsNodeRead_h_t_conv conv = {self};
  struct AsNodeRead_t* const p_self = conv.clear;

  memset(p_self, 0, sizeof(*p_self));

  if (hardware_init_read_permission_gpio(&p_self->m_gpio_read_as_node, GPIO_AS_NODE)<0)
  {
    return -1;
  }

  return 0;
}

uint8_t as_node_get_status(const AsNode_h* const restrict self)
{
  const union AsNode_h_t_conv_const conv = {self};
  const struct AsNode_t* const p_self = conv.clear;

  return !gpio_read_state(&p_self->m_gpio_as_node.gpio_read_permission);
}

uint8_t as_node_read_get_status(const AsNodeRead_h* const restrict self)
{
  const union AsNodeRead_h_t_conv_const conv = {self};
  const struct AsNodeRead_t* const p_self = conv.clear;

  return !gpio_read_state(&p_self->m_gpio_read_as_node);
}
