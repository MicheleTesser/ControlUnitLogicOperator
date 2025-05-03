#ifndef __VIRTUAL_GPIO__
#define __VIRTUAL_GPIO__

#include <stdint.h>


//common_gpio

typedef struct __attribute__((aligned(4))){
  const uint8_t private_data[4];
}GpioRead_h;

extern int8_t
hardware_init_read_permission_gpio(GpioRead_h* const restrict self ,
    const uint16_t id)__attribute__((__nonnull__(1))); 

extern int8_t gpio_read_state(const GpioRead_h* const restrict self)__attribute__((__nonnull__(1)));

//basic gpio

enum GPIO_PIN{
  GPIO_CORE_0_ALIVE_BLINK=0,
  GPIO_CORE_1_ALIVE_BLINK,
  GPIO_CORE_2_ALIVE_BLINK,
  GPIO_RTD_BUTTON,
  GPIO_RTD_ASSI_SOUND,
  GPIO_AIR_PRECHARGE_INIT,
  GPIO_AIR_PRECHARGE_DONE,
  GPIO_SCS,
  GPIO_AS_NODE,

  __NUM_OF_GPIOS__
};

typedef struct __attribute__((aligned(4))){
  const GpioRead_h gpio_read_permission;
  const uint8_t private_data[4];
}Gpio_h;


extern int8_t
hardware_init_gpio(Gpio_h* const restrict self , 
    const enum GPIO_PIN id)__attribute__((__nonnull__(1)));

extern int8_t gpio_toggle(Gpio_h* const restrict self)__attribute__((__nonnull__(1)));

extern int8_t gpio_set_high(Gpio_h* const restrict self)__attribute__((__nonnull__(1)));

extern int8_t gpio_set_low(Gpio_h* const restrict self)__attribute__((__nonnull__(1)));

//pwm gpio

enum GPIO_PWM_PIN{
  PWM_GPIO_ASSI_LIGHT_BLU = 0,
  PWM_GPIO_ASSI_LIGHT_YELLOW,

  __NUM_OF_PWM_GPIOS__
};

typedef struct __attribute__((aligned(4))){
  const uint8_t private_data[16];
}GpioPwm_h;

extern int8_t
hardware_init_gpio_pwm(GpioPwm_h* const restrict self , 
    const enum GPIO_PWM_PIN id)__attribute__((__nonnull__(1)));

extern int8_t
hardware_init_gpio_pwm_read_only(GpioPwm_h* const restrict self , 
    const enum GPIO_PWM_PIN id)__attribute__((__nonnull__(1)));

extern int8_t
hardware_write_gpio_pwm(GpioPwm_h* const restrict self, const uint16_t duty_cycle)
  __attribute__((__nonnull__(1)));

extern uint16_t
hardware_read_gpio_pwm(GpioPwm_h* const restrict self) __attribute__((__nonnull__(1)));


#endif // !__VIRTUAL_GPIO__
