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
#include <stddef.h>

//gpio

struct GpioRead_t{
  enum GPIO_PIN pin_gpio;
};

#define GPIO_OUTPUT_MODE ((IfxPort_Mode)(IfxPort_OutputMode_pushPull | IfxPort_OutputIdx_general))
#define GPIO_INPUT_MODE ((IfxPort_Mode) IfxPort_InputMode_pullDown)

static struct GpioSpec{
  Ifx_P *port;
  uint8_t pin_index;
  IfxPort_Mode mode;
}BOARD_GPIOS[__NUM_OF_GPIOS__] =
{
  {&MODULE_P33, 0, GPIO_OUTPUT_MODE}, //INFO: GPIO_CORE_0_ALIVE_BLINK, default led 0
  {&MODULE_P33, 4, GPIO_OUTPUT_MODE}, //INFO: GPIO_CORE_1_ALIVE_BLINK, debug led 2 1
  {&MODULE_P33, 6, GPIO_OUTPUT_MODE}, //INFO: GPIO_CORE_2_ALIVE_BLINK, debug led 3 2
  {&MODULE_P23, 1, GPIO_INPUT_MODE}, //TODO: GPIO_RTD_BUTTON, DRIVE_BUTTON 3
  {&MODULE_P33, 5, GPIO_OUTPUT_MODE}, //INFO: GPIO_RTD_ASSI_SOUND, RTDS 4
  {&MODULE_P32, 4, GPIO_INPUT_MODE}, //INFO: GPIO_AIR_PRECHARGE_INIT, AIR_1 5
  {&MODULE_P23, 0, GPIO_INPUT_MODE}, //INFO: GPIO_AIR_PRECHARGE_DONE, AIR_2 6
  {&MODULE_P33, 3, GPIO_OUTPUT_MODE}, //INFO: GPIO_SCS, SCS_SDC_RELAY 7
  {&MODULE_P33, 2, GPIO_OUTPUT_MODE}, //INFO: GPIO_AS_NODE, AS_SDC_RELAY 8
  {&MODULE_P33, 1, GPIO_OUTPUT_MODE}, //INFO: GPIO_RTD_BUTTON_LED, RTD 9
};

union GpioRead_h_t_conv{
  GpioRead_h* const restrict hidden;
  struct GpioRead_t* const restrict clear;
};

union GpioRead_h_t_conv_const{
  const GpioRead_h* const restrict hidden;
  const struct GpioRead_t* const restrict clear;
};

struct Gpio_t{
  struct GpioRead_t gpio_read_permission;
  uint8_t private_data[4];
};

union Gpio_h_t_conv{
  Gpio_h* const restrict hidden;
  struct Gpio_t* const restrict clear;
};
#define DEBUG
#ifdef DEBUG
char __assert_gpio_size[sizeof(GpioRead_h)==sizeof(struct GpioRead_t)?+1:-1];
char __assert_gpio_align[_Alignof(GpioRead_h)==_Alignof(struct GpioRead_t)?+1:-1];

char __assert_gpio_size[sizeof(Gpio_h)==sizeof(struct Gpio_t)?+1:-1];
char __assert_gpio_align[_Alignof(Gpio_h)==_Alignof(struct Gpio_t)?+1:-1];

#endif /* ifdef DEBUG */

//public

int8_t hardware_init_gpio(Gpio_h* const restrict self ,const enum GPIO_PIN id)
{
  union Gpio_h_t_conv conv = {self};
  struct Gpio_t* p_self = conv.clear;
  struct GpioSpec* p_info_gpio = NULL;

  if (id >= __NUM_OF_GPIOS__)
  {
    return -1;
  }

  p_info_gpio = &BOARD_GPIOS[id];

  p_self->gpio_read_permission.pin_gpio = id;

  IfxPort_setPinMode(p_info_gpio->port,p_info_gpio->pin_index,p_info_gpio->mode);
  if (p_info_gpio->mode == GPIO_OUTPUT_MODE)
  {
    gpio_set_high(self);
  }

  return 0;
}

int8_t hardware_init_read_permission_gpio(GpioRead_h* const restrict self, const uint16_t id)
{
  union GpioRead_h_t_conv conv = {self};
  struct GpioRead_t* p_self = conv.clear;
  struct GpioSpec* p_info_gpio = NULL;
  p_info_gpio = &BOARD_GPIOS[id];

  p_self->pin_gpio= id;
  IfxPort_setPinMode(p_info_gpio->port,p_info_gpio->pin_index,p_info_gpio->mode);

  return 0;
}

int8_t gpio_toggle(Gpio_h* const restrict self)
{
  union Gpio_h_t_conv conv = {self};
  struct Gpio_t* p_self = conv.clear;
  struct GpioSpec* p_info_gpio = &BOARD_GPIOS[p_self->gpio_read_permission.pin_gpio];

  IfxPort_togglePin(
      p_info_gpio->port,
      p_info_gpio->pin_index);
  return 0;
}
int8_t gpio_read_state(const GpioRead_h* const restrict self)
{
  const union GpioRead_h_t_conv_const conv = {self};
  const struct GpioRead_t* p_self = conv.clear;
  struct GpioSpec* p_info_gpio = &BOARD_GPIOS[p_self->pin_gpio];

  return IfxPort_getPinState(p_info_gpio->port,p_info_gpio->pin_index);
}

int8_t gpio_set_high(Gpio_h* const restrict self)
{
  union Gpio_h_t_conv conv = {self};
  struct Gpio_t* p_self = conv.clear;
  struct GpioSpec* p_info_gpio = &BOARD_GPIOS[p_self->gpio_read_permission.pin_gpio];

  IfxPort_setPinHigh(p_info_gpio->port,p_info_gpio->pin_index);
  return 0;
}

int8_t gpio_set_low(Gpio_h* const restrict self)
{
  union Gpio_h_t_conv conv = {self};
  struct Gpio_t* p_self = conv.clear;
  struct GpioSpec* p_info_gpio = &BOARD_GPIOS[p_self->gpio_read_permission.pin_gpio];

  IfxPort_setPinLow(p_info_gpio->port,p_info_gpio->pin_index);

  return 0;
}

//pwm

struct GpioPwm_t{
  const uint8_t private_data[16];
};

int8_t hardware_init_gpio_pwm(GpioPwm_h* const restrict self __attribute__((__unused__)),
    const enum GPIO_PWM_PIN id __attribute__((__unused__)))
{
  //TODO: not yet implemented
  return 0;
}

int8_t hardware_init_gpio_pwm_read_only(GpioPwm_h* const restrict self __attribute__((__unused__)),
    const enum GPIO_PWM_PIN id __attribute__((__unused__)))
{
  //TODO: not yet implemented
  return 0;
}

int8_t hardware_write_gpio_pwm(GpioPwm_h* const restrict self __attribute__((__unused__)),
    const uint16_t duty_cycle __attribute__((__unused__)))
{
  //TODO: not yet implemented
  return 0;
}

uint16_t hardware_read_gpio_pwm(GpioPwm_h* const restrict self __attribute__((__unused__)))
{
  //TODO: not yet implemented
  return 0;
}
