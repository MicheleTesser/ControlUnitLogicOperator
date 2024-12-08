#ifndef __AURIX_GPIO__
#define __AURIX_GPIO__

#include <stdint.h>

typedef struct component_gpio component_gpio;

int8_t hardware_init_gpio(const uint16_t id);
int8_t gpio_set_pin_mode(const component_gpio* const restrict  comp,uint8_t mode);
int8_t gpio_toggle(const component_gpio* const restrict  comp);
int8_t gpio_read_state(const component_gpio* const restrict comp);
int8_t gpio_set_high(const component_gpio* const restrict comp);
int8_t gpio_set_low(const component_gpio* const restrict comp);

#endif // !__AURIX_GPIO__
