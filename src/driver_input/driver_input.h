#ifndef __DRIVER_INPUT__
#define __DRIVER_INPUT__

#include <stdint.h>
#include "../../lib/raceup_board/components/timer.h"

enum INPUT_TYPES{
    THROTTLE =0,
    BRAKE =1,
    STEERING_ANGLE = 2,
    REGEN =3,
    READY_TO_DRIVE_BUTTON =4,

    NUM_OF_INPUT_TYPES_USED_ONLY_FOR_INDEX
};

enum IMPL{
    THROTTLE_BRAKE  = (1<<0),
    THROTTLE_PADEL = (1<<1),
    THROTTLE_POT = (1<<2),
};

int8_t driver_input_init(void);
float driver_get_amount(const enum INPUT_TYPES driver_input);
uint8_t driver_set_amount(const enum INPUT_TYPES driver_input, 
        const float percentage, const time_var_microseconds timestamp);

void set_implausibility(const enum IMPL impl,const uint8_t value);
void clear_implausibility(void);

uint8_t check_impls(const uint8_t impls);

#endif // !__DRIVER_INPUT__
