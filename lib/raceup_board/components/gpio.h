#ifndef __VIRTUA_GPIO__
#define __VIRTUA_GPIO__

#include <stdint.h>
#include "./common_idx/common_idx.h"

extern int8_t hardware_init_gpio(const BoardComponentId id);
extern int8_t gpio_set_pin_mode(const BoardComponentId id, uint8_t mode);
extern int8_t gpio_toggle(const BoardComponentId id);
extern int8_t gpio_read_state(const BoardComponentId id);
extern int8_t gpio_set_high(const BoardComponentId id);
extern int8_t gpio_set_low(const BoardComponentId id);

#endif // !__VIRTUA_GPIO__
