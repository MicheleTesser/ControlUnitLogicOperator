#include "alive_blink.h"
#include "../../../../lib/raceup_board/raceup_board.h"
#include <stdio.h>
#include <string.h>

struct CoreAliveBlink_t{
  Gpio_h gpio_led;
  time_var_microseconds last_time_toggle;
  time_var_microseconds freq;
};

union CoreAliveBlink_h_t_conv{
  CoreAliveBlink_h* const restrict hidden;
  struct CoreAliveBlink_t* const restrict clear;
};

#ifdef DEBUG
char __assert_size_alive_blink[(sizeof(CoreAliveBlink_h) == sizeof(struct CoreAliveBlink_t))? 1:-1];
#endif // DEBUG
       //public

  int8_t
core_alive_blink_init(
    CoreAliveBlink_h* const restrict self,
    const enum GPIO_PIN pin_led, const time_var_microseconds freq)
{
  union CoreAliveBlink_h_t_conv conv = {self};
  struct CoreAliveBlink_t* const restrict p_self = conv.clear;

  memset(p_self, 0, sizeof(*p_self));
  if (hardware_init_gpio(&p_self->gpio_led, pin_led)<0) {
    return -1;
  }
  p_self->last_time_toggle = timer_time_now();
  p_self->freq = freq;

  return 0;
}

  int8_t
core_alive_blink_update(CoreAliveBlink_h* const restrict self )
{
  union CoreAliveBlink_h_t_conv conv = {self};
  struct CoreAliveBlink_t* const restrict p_self = conv.clear;

  ACTION_ON_FREQUENCY(p_self->last_time_toggle, p_self->freq)
  {
    gpio_toggle(&p_self->gpio_led);
  }

  return 0;
}
