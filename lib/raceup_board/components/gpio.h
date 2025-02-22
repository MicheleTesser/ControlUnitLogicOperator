#ifndef __VIRTUAL_GPIO__
#define __VIRTUAL_GPIO__

#include <stdint.h>

enum GPIO_PIN{
    GPIO_CORE_0_ALIVE_BLINK=0,
    GPIO_CORE_1_ALIVE_BLINK,
    GPIO_CORE_2_ALIVE_BLINK,
    GPIO_TS_BUTTON,
    GPIO_RTD_BUTTON,
    GPIO_RTD_ASSI_SOUND,
    GPIO_AIR_PRECHARGE_INIT,
    GPIO_AIR_PRECHARGE_DONE,
    GPIO_ASSI_LIGHT_BLU,
    GPIO_ASSI_LIGHT_YELLOW,
    GPIO_SCS,

    __NUM_OF_GPIOS__
};

typedef struct __attribute__((aligned(4))) GpioRead_h{
    const uint8_t private_data[4];
}GpioRead_h;

typedef struct __attribute__((aligned(4))) Gpio_h{
    const GpioRead_h gpio_read_permission;
    const uint8_t private_data[4];
}Gpio_h;


extern int8_t
hardware_init_read_permission_gpio(GpioRead_h* const restrict self ,
        const uint16_t id)__attribute__((__nonnull__(1))); 

extern int8_t
hardware_init_gpio(Gpio_h* const restrict self , 
        const uint16_t id)__attribute__((__nonnull__(1)));

extern int8_t gpio_set_pin_mode(Gpio_h* const restrict self,
        const uint8_t mode)__attribute__((__nonnull__(1)));

extern int8_t gpio_toggle(Gpio_h* const restrict self)__attribute__((__nonnull__(1)));

extern int8_t gpio_read_state(const GpioRead_h* const restrict self)__attribute__((__nonnull__(1)));

extern int8_t gpio_set_high(Gpio_h* const restrict self)__attribute__((__nonnull__(1)));

extern int8_t gpio_set_low(Gpio_h* const restrict self)__attribute__((__nonnull__(1)));

#endif // !__VIRTUAL_GPIO__
