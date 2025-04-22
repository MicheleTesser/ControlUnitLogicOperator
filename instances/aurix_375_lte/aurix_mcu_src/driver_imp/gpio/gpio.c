#include "../../src/lib/raceup_board/raceup_board.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "IfxPort.h"
#include "Bsp.h"
#include "Ifx_Types.h"
#include "IfxGpt12.h"
#include "IfxPort.h"
#pragma GCC diagnostic pop

#include <stdint.h>

//gpio

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
int8_t gpio_set_pin_mode(Gpio_h* const restrict self __attribute__((__unused__)) ,uint8_t mode __attribute__((__unused__)))
{
    return 0;
}

int8_t gpio_toggle(Gpio_h* const restrict self)
{
  union Gpio_h_t_conv conv = {self};
  struct Gpio_t* p_self = conv.clear;
  IfxPort_togglePin(
      p_self->gpio_read_permission.port,
      p_self->gpio_read_permission.pin_index);
  return 0;
}
int8_t gpio_read_state(const GpioRead_h* const restrict self)
{
  const union GpioRead_h_t_conv conv = {self};
  const struct GpioRead_t* p_self __attribute__((__unused__)) = conv.clear;
  return IfxPort_getPinState(p_self->port,p_self->pin_index);
}
int8_t gpio_set_high(Gpio_h* const restrict self)
{
  union Gpio_h_t_conv conv = {self};
  struct Gpio_t* p_self = conv.clear;
  IfxPort_setPinHigh(
      p_self->gpio_read_permission.port,
      p_self->gpio_read_permission.pin_index);
  return 0;
}

int8_t gpio_set_low(Gpio_h* const restrict self)
{
  union Gpio_h_t_conv conv = {self};
  struct Gpio_t* p_self = conv.clear;
  IfxPort_setPinLow(
      p_self->gpio_read_permission.port,
      p_self->gpio_read_permission.pin_index);
  return 0;
}

//pwm

struct GpioPwm_t{
  const uint8_t private_data[16];
};

int8_t hardware_init_gpio_pwm(GpioPwm_h* const restrict self __attribute__((__unused__)),
    const enum GPIO_PWM_PIN id __attribute__((__unused__)))
{
}

int8_t hardware_init_gpio_pwm_read_only(GpioPwm_h* const restrict self __attribute__((__unused__)),
    const enum GPIO_PWM_PIN id __attribute__((__unused__)))
{
}

int8_t hardware_write_gpio_pwm(GpioPwm_h* const restrict self __attribute__((__unused__)),
    const uint16_t duty_cycle __attribute__((__unused__)))
{
}

uint16_t hardware_read_gpio_pwm(GpioPwm_h* const restrict self __attribute__((__unused__)))
{
}
