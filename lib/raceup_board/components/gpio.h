#ifndef __VIRTUA_GPIO__
#define __VIRTUA_GPIO__

#include <stdint.h>

enum GPIO_PIN{
    CORE_0_ALIVE_BLINK,
    CORE_1_ALIVE_BLINK,
    CORE_2_ALIVE_BLINK,
    GPIO_RTD_BUTTON,
    GPIO_RTD_SOUND,
    GPIO_AIR_PRECHARGE_INIT,
    GPIO_AIR_PRECHARGE_DONE,
    GPIO_AS_EMERGENCY_SOUND,
    GPIO_ASSI_LIGHT_BLU,
    GPIO_ASSI_LIGHT_YELLOW,
};


extern int8_t hardware_init_gpio(const enum GPIO_PIN id);
extern int8_t gpio_set_pin_mode(const enum GPIO_PIN id, uint8_t mode);
extern int8_t gpio_toggle(const enum GPIO_PIN id);
extern int8_t gpio_read_state(const enum GPIO_PIN id);
extern int8_t gpio_set_high(const enum GPIO_PIN id);
extern int8_t gpio_set_low(const enum GPIO_PIN id);

#endif // !__VIRTUA_GPIO__
