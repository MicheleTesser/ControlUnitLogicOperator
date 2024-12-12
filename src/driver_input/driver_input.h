#ifndef __DRIVER_INPUT__
#define __DRIVER_INPUT__

#include <stdint.h>

enum INPUT_TYPES{
    THROTTLE =0,
    BRAKE =1,
    REGEN =2,
};

float get_amount(enum INPUT_TYPES driver_input);
float set_amount(enum INPUT_TYPES driver_input, float percentage);

#endif // !__DRIVER_INPUT__
