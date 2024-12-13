#ifndef __DRIVER_INPUT__
#define __DRIVER_INPUT__

#include <stdint.h>

enum INPUT_TYPES{
    THROTTLE =0,
    BRAKE =1,
    REGEN =2,
};

enum IMPL{
    THROTTLE_BRAKE,
    THROTTLE_PADEL,
    THROTTLE_POT,
};

float driver_get_amount(const enum INPUT_TYPES driver_input);
float driver_set_amount(const enum INPUT_TYPES driver_input, const float percentage);

void save_implausibility(const enum IMPL impl);
void clear_implausibility(void);

uint8_t check_imp(const enum IMPL impl);

#endif // !__DRIVER_INPUT__
