#ifndef __VIRTUA_GPIO__
#define __VIRTUA_GPIO__

#include <stdint.h>

enum GPIO_PIN{
    GPIO_0,
    GPIO_1,
    GPIO_2,
    GPIO_3,
    GPIO_5,
    GPIO_6,
};


extern int8_t hardware_init_gpio(const enum GPIO_PIN id);
extern int8_t gpio_set_pin_mode(const enum GPIO_PIN id, uint8_t mode);
extern int8_t gpio_toggle(const enum GPIO_PIN id);
extern int8_t gpio_read_state(const enum GPIO_PIN id);
extern int8_t gpio_set_high(const enum GPIO_PIN id);
extern int8_t gpio_set_low(const enum GPIO_PIN id);

#endif // !__VIRTUA_GPIO__
