#include "pcu.h"
#include "../../linux_board/linux_board.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "../src/lib/board_dbc/dbc/out_lib/can2/can2.h"
#pragma GCC diagnostic pop 
#include "../external_gpio.h"
#include "../embedded_system/embedded_system.h"

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
  struct CanNode* can_node;
  Gpio_h inverter_on_gpio;
  thrd_t thread;
  struct CollingDevice{
    uint8_t power_percentage: 7;
    uint8_t enable:1;
  }cooling_device[__NUM_OF_COOLING_DEVICES__];
  Gpio_h m_gpio_embedded_system;
  Gpio_h m_gpio_dv;
  uint8_t run:1;
  uint8_t rf:1;
};

union Pcu_h_t_conv{
  Pcu_h* const restrict hidden;
  struct Pcu_t* const restrict clear; 
};

#ifdef DEBUG
char __assert_size_pcu[(sizeof(Pcu_h)==sizeof(struct Pcu_t))?1:-1];
char __assert_align_pcu[(_Alignof(Pcu_h)==_Alignof(struct Pcu_t))?1:-1];
#endif /* ifdef DEBUG */

static int8_t
_pcu_update(struct Pcu_t* const restrict self)__attribute_maybe_unused__;
static int8_t
_pcu_update(struct Pcu_t* const restrict self)
{
  CanMessage mex = {0};
  can_obj_can2_h_t o2={0};

  if (hardware_mailbox_read(self->recv_can_node_pcu_inv, &mex)
      && unpack_message_can2(&o2, CAN_ID_PCU, mex.full_word, mex.message_size, 0)>=0)
  {
    switch(o2.can_0x130_Pcu.mode)
    {
      case 1:
        self->rf = o2.can_0x130_Pcu.rf;
        if (self->rf)
        {
          gpio_set_low(&self->inverter_on_gpio);
        }
        else
        {
          gpio_set_high(&self->inverter_on_gpio);
        }
        break;
      case 2:
        if (o2.can_0x130_Pcu.enable_embedded)
        {
          gpio_set_low(&self->m_gpio_embedded_system);
        }
        else
        {
          gpio_set_high(&self->m_gpio_embedded_system);
        }
        if (o2.can_0x130_Pcu.enable_dv)
        {
          gpio_set_low(&self->m_gpio_dv);
        }
        else
        {
          gpio_set_high(&self->m_gpio_dv);
        }
        break;
      default:
        break;
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
    ACTION_ON_FREQUENCY(t, get_tick_from_millis(50))
    {
      _pcu_update(self);
    }
  }
  return 0;
}

//public

int8_t pcu_init(Pcu_h* const restrict self)
{
  union Pcu_h_t_conv conv = {self};
  struct Pcu_t* const restrict p_self = conv.clear;
  memset(p_self, 0, sizeof(*p_self));
  if (hardware_init_gpio(&p_self->inverter_on_gpio, (enum GPIO_PIN) GPIO_INVERTER_RF_SIGNAL)<0)
  {
    return -1;
  }

  p_self->can_node = hardware_init_new_external_node(CAN_GENERAL);
  if (!p_self->can_node)
  {
    return -2;
  }

  p_self->recv_can_node_pcu_inv = 
    hardware_get_mailbox_single_mex(
        p_self->can_node,
        RECV_MAILBOX,
        CAN_ID_PCU,
        message_dlc_can2(CAN_ID_PCU));

  p_self->send_can_node_pcu_inv =
    hardware_get_mailbox_single_mex(
        p_self->can_node,
        SEND_MAILBOX,
        CAN_ID_PCURFACK,
        message_dlc_can2(CAN_ID_PCURFACK));


  if (!p_self->recv_can_node_pcu_inv)
  {
    return -3;
  }

  if (hardware_init_gpio(&p_self->m_gpio_embedded_system, (enum GPIO_PIN) GPIO_PCU_EMBEDDED_SYSTEM)<0)
  {
    return -4;
  }

  if (hardware_init_gpio(&p_self->m_gpio_dv, (enum GPIO_PIN) GPIO_PCU_DV)<0)
  {
    return -5;
  }

  p_self->run=1;
  thrd_create(&p_self->thread, _pcu_start,p_self);

  return 0;
}


int8_t pcu_stop(Pcu_h* const restrict self)
{
  union Pcu_h_t_conv conv = {self};
  struct Pcu_t* const restrict p_self = conv.clear;

  printf("stopping pcu\n");
  p_self->run=0;
  thrd_join(p_self->thread, NULL);
  hardware_free_mailbox_can(&p_self->recv_can_node_pcu_inv);
  hardware_free_mailbox_can(&p_self->send_can_node_pcu_inv);
  hardware_init_new_external_node_destroy(p_self->can_node);

  return 0;
}

int8_t pcu_start_embedded(Pcu_h* const restrict self)
{
  union Pcu_h_t_conv conv = {self};
  struct Pcu_t* const restrict p_self = conv.clear;

  return gpio_set_low(&p_self->m_gpio_embedded_system);
}

int8_t pcu_stop_embedded(Pcu_h* const restrict self)
{
  union Pcu_h_t_conv conv = {self};
  struct Pcu_t* const restrict p_self = conv.clear;

  return gpio_set_high(&p_self->m_gpio_embedded_system);
}

int8_t pcu_start_res(Pcu_h* const restrict self)
{
  union Pcu_h_t_conv conv = {self};
  struct Pcu_t* const restrict p_self = conv.clear;

  return gpio_set_low(&p_self->m_gpio_dv);
}

int8_t pcu_stop_res(Pcu_h* const restrict self)
{
  union Pcu_h_t_conv conv = {self};
  struct Pcu_t* const restrict p_self = conv.clear;

  return gpio_set_high(&p_self->m_gpio_dv);
}
