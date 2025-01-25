#ifndef __VIRTUAL_GPIO__
#define __VIRTUAL_GPIO__

#include <stdint.h>

enum GPIO_PIN{
    GPIO_CORE_0_ALIVE_BLINK,
    GPIO_CORE_1_ALIVE_BLINK,
    GPIO_CORE_2_ALIVE_BLINK,
    GPIO_RTD_BUTTON,
    GPIO_RTD_SOUND,
    GPIO_AIR_PRECHARGE_INIT,
    GPIO_AIR_PRECHARGE_DONE,
    GPIO_AS_EMERGENCY_SOUND,
    GPIO_ASSI_LIGHT_BLU,
    GPIO_ASSI_LIGHT_YELLOW,
    GPIO_SCS,
};

typedef struct GpioRead_h{
    const uint8_t private_data[1];
}GpioRead_h;

typedef struct Gpio_h{
    const GpioRead_h gpio_read_permission;
    const uint8_t private_data[1];
}Gpio_h;


extern int8_t
hardware_init_read_permission_gpio(GpioRead_h* const restrict self ,
        const enum GPIO_PIN id)__attribute__((__nonnull__(1))); 

extern int8_t
hardware_init_gpio(Gpio_h* const restrict self , 
        const enum GPIO_PIN id)__attribute__((__nonnull__(1)));

extern int8_t gpio_set_pin_mode(Gpio_h* const restrict self,
        const uint8_t mode)__attribute__((__nonnull__(1)));

extern int8_t gpio_toggle(Gpio_h* const restrict self)__attribute__((__nonnull__(1)));

extern int8_t gpio_read_state(const GpioRead_h* const restrict self)__attribute__((__nonnull__(1)));

extern int8_t gpio_set_high(Gpio_h* const restrict self)__attribute__((__nonnull__(1)));

extern int8_t gpio_set_low(Gpio_h* const restrict self)__attribute__((__nonnull__(1)));

#endif // !__VIRTUAL_GPIO__
