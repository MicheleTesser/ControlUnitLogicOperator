#include "pcu.h"
#include "../../linux_board/linux_board.h"
#include "../src/lib/board_dbc/dbc/out_lib/can2/can2.h"
#include "../external_gpio.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/cdefs.h>
#include <sys/procfs.h>
#include <threads.h>

enum CoolingDeviceType
{
  FAN_RAD_R=0,
  FAN_RAD_L,
  FAN_BAT_R,
  FAN_BAT_L,
  PUMP_L,
  PUMP_R,

  __NUM_OF_COOLING_DEVICES__
};

struct Pcu_t{
  struct CanMailbox* recv_can_node_pcu_inv;
  struct CanMailbox* send_can_node_pcu_inv;
  Gpio_h inverter_on_gpio;
  thrd_t thread;
  struct CollingDevice{
    uint8_t power_percentage: 7;
    uint8_t enable:1;
  }cooling_device[__NUM_OF_COOLING_DEVICES__];
  uint8_t run:1;
  uint8_t rf:1;
};

union Pcu_h_t_conv{
  Pcu_h* const restrict hidden;
  struct Pcu_t* const restrict clear; 
};

#ifdef DEBUG
char __assert_size_pcu[(sizeof(Pcu_h)==sizeof(struct Pcu_t))?1:-1];
#endif /* ifdef DEBUG */

static int8_t
_pcu_update(struct Pcu_t* const restrict self)__attribute_maybe_unused__;
static int8_t
_pcu_update(struct Pcu_t* const restrict self)
{
  CanMessage mex = {0};
  can_obj_can2_h_t o2={0};

  if (!hardware_mailbox_read(self->recv_can_node_pcu_inv, &mex))
  {
    unpack_message_can2(&o2, mex.id, mex.full_word, mex.message_size, 0);
    switch (mex.id)
    {
      case CAN_ID_PCURF:
        self->rf = o2.can_0x133_PcuRf.rf_signal;
        if (o2.can_0x133_PcuRf.rf_signal)
        {
          gpio_set_low(&self->inverter_on_gpio);
        }
        else
        {
          gpio_set_high(&self->inverter_on_gpio);
        }
        break;
      default:
        return -1;
    }
  }
  return hardware_mailbox_send(self->send_can_node_pcu_inv, self->rf);
}

static int
_pcu_start(void* args)
{
  struct Pcu_t* self = args;
  time_var_microseconds t =0;

  while (self->run)
  {
    if ((timer_time_now() - t) > 50 MILLIS)
    {
      _pcu_update(self);
      t = timer_time_now();
    }
  }
  return 0;
}

//public

int8_t
pcu_init(struct Pcu_h* const restrict self)
{
  union Pcu_h_t_conv conv = {self};
  struct Pcu_t* const restrict p_self = conv.clear;
  memset(p_self, 0, sizeof(*p_self));
  if (hardware_init_gpio(&p_self->inverter_on_gpio, GPIO_INVERTER_RF_SIGNAL)<0)
  {
    return -1;
  }

  struct CanNode* can_node = hardware_init_can_get_ref_node_new(CAN_GENERAL);
  if (!can_node)
  {
    return -2;
  }

  p_self->recv_can_node_pcu_inv = 
    hardware_get_mailbox_single_mex(can_node, RECV_MAILBOX, CAN_ID_PCURF, 7);
  p_self->send_can_node_pcu_inv =
    hardware_get_mailbox_single_mex(can_node, SEND_MAILBOX, CAN_ID_PCURFACK, 1);
  hardware_init_can_get_ref_node_destroy(can_node);

  if (!p_self->recv_can_node_pcu_inv)
  {
    return -3;
  }

  p_self->run=1;
  thrd_create(&p_self->thread, _pcu_start,p_self);

  return 0;
}


int8_t
pcu_stop(struct Pcu_h* const restrict self)
{
  union Pcu_h_t_conv conv = {self};
  struct Pcu_t* const restrict p_self = conv.clear;
  p_self->run=0;
  thrd_join(p_self->thread, NULL);
  hardware_free_mailbox_can(&p_self->recv_can_node_pcu_inv);
  hardware_free_mailbox_can(&p_self->send_can_node_pcu_inv);

  return 0;
}
