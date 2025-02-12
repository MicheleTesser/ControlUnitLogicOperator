#include "pcu.h"
#include "../../linux_board/linux_board.h"
#include "../external_gpio.h"
#include <stdint.h>
#include <string.h>

struct Pcu_t{
  struct CanNode* send_can_node_pcu_inv;
  Gpio_h inverter_on_gpio;
};

union Pcu_h_t_conv{
  Pcu_h* const restrict hidden;
  struct Pcu_t* const restrict clear; 
};

#ifdef DEBUG
char __assert_size_pcu[(sizeof(Pcu_h)==sizeof(struct Pcu_t))?1:-1];
#endif /* ifdef DEBUG */

int8_t
pcu_init(struct Pcu_h* const restrict self)
{
  union Pcu_h_t_conv conv = {self};
  struct Pcu_t* const restrict p_self = conv.clear;
  memset(p_self, 0, sizeof(*p_self));

  //TODO: to define pcu messages

  p_self->send_can_node_pcu_inv = hardware_init_can_get_ref_node_new(CAN_INVERTER);
  if (!p_self->send_can_node_pcu_inv)
  {
    return -1;
  }

  if (hardware_init_gpio(&p_self->inverter_on_gpio, GPIO_INVERTER_RF_SIGNAL)<0)
  {
    return -2;
  }

  return 0;
}

int8_t
pcu_update(struct Pcu_h* const restrict self)
{
  union Pcu_h_t_conv conv = {self};
  struct Pcu_t* const restrict p_self = conv.clear;
  CanMessage mex = {0};

  if (!hardware_read_can(p_self->send_can_node_pcu_inv, &mex))
  {
    return -1;
  }

  return 0;
}
