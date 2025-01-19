#ifndef __COOLING_DEVICE__
#define __COOLING_DEVICE__

#include "../../general_can/general_can.h"
#include <stdint.h>

typedef struct CoolingDevice_h{
    const uint8_t private_data[1];
}CoolingDevice_h;

int8_t cooling_device_init(CoolingDevice_h* const restrict self);
int8_t cooling_device_toggle(CoolingDevice_h* const restrict self);
int8_t cooling_device_set_speed(CoolingDevice_h* const restrict self, const float value);
int8_t cooling_device_get_speed(CoolingDevice_h* const restrict self);
uint8_t cooling_device_is_enable(CoolingDevice_h* const restrict self);

#endif // !__COOLING_DEVICE__
