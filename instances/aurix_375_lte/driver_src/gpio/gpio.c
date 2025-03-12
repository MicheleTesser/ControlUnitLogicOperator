#include "gpio.h"
#include "IfxPort.h"
#include "../raceup_board/components/gpio.h"

#include <stdint.h>

struct GpioRead_t{
  Ifx_P *port;
  uint8_t pin_index;
};

union GpioRead_h_t_conv{
  const GpioRead_h* const restrict hidden;
  const struct GpioRead_t* const restrict clear;
};

struct Gpio_t{
  const struct GpioRead_t gpio_read_permission;
  const uint8_t private_data[4];
};

union Gpio_h_t_conv{
  Gpio_h* const restrict hidden;
  struct Gpio_t* const restrict clear;
};

//public

int8_t hardware_init_gpio(Gpio_h* const restrict self ,const enum GPIO_PIN id)
{
  union Gpio_h_t_conv conv = {self};
  struct Gpio_t* p_self __attribute__((__unused__))= conv.clear;
  switch (id)
  {
    case GPIO_CORE_0_ALIVE_BLINK:
    case GPIO_CORE_1_ALIVE_BLINK:
    case GPIO_CORE_2_ALIVE_BLINK:
    case GPIO_TS_BUTTON:
    case GPIO_RTD_BUTTON:
    case GPIO_RTD_ASSI_SOUND:
    case GPIO_AIR_PRECHARGE_INIT:
    case GPIO_AIR_PRECHARGE_DONE:
    case GPIO_SCS:
    default:
      break;
  }
    return 0;
}
int8_t gpio_set_pin_mode(Gpio_h* const restrict self,uint8_t mode)
{
    return 0;
}

int8_t gpio_toggle(Gpio_h* const restrict self)
{
  union Gpio_h_t_conv conv = {self};
  struct Gpio_t* p_self = conv.clear;
  return IfxPort_togglePin(
      p_self->gpio_read_permission.port,
      p_self->gpio_read_permission.pin_index);
}
int8_t gpio_read_state(const GpioRead_h* const restrict self)
{
  const union GpioRead_h_t_conv conv = {self};
  const struct GpioRead_t* p_self = conv.clear;
  return IfxPort_readPin(
      p_self->port,
      p_self->pin_index);
}
int8_t gpio_set_high(Gpio_h* const restrict self)
{
  union Gpio_h_t_conv conv = {self};
  struct Gpio_t* p_self = conv.clear;
  return IfxPort_setPinHigh(
      p_self->gpio_read_permission.port,
      p_self->gpio_read_permission.pin_index);
}

int8_t gpio_set_low(Gpio_h* const restrict self)
{
  union Gpio_h_t_conv conv = {self};
  struct Gpio_t* p_self = conv.clear;
  return IfxPort_setPinLow(
      p_self->gpio_read_permission.port,
      p_self->gpio_read_permission.pin_index);
}
