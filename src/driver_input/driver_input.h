#ifndef __DRIVER_INPUT__
#define __DRIVER_INPUT__

#include <stdint.h>
#include "../../lib/raceup_board/components/timer.h"
#include "input_rtd/input_rtd.h"

enum INPUT_TYPES{
    THROTTLE =0,
    BRAKE =1,
    STEERING_ANGLE = 2,
    REGEN =3,

    NUM_OF_INPUT_TYPES_USED_ONLY_FOR_INDEX
};

enum IMPL{
    THROTTLE_BRAKE  = (1<<0),
    THROTTLE_PADEL = (1<<1),
    THROTTLE_POT = (1<<2),
};

struct DriverInput;

int8_t driver_input_init(void);
const struct DriverInput* driver_input_get(void);
struct DriverInput* driver_input_get_mut(void);

float driver_get_amount(const struct DriverInput* const restrict self,
        const enum INPUT_TYPES driver_input);
uint8_t driver_set_amount(struct DriverInput* const restrict self,
        const enum INPUT_TYPES driver_input, 
        const float percentage, const time_var_microseconds timestamp);

void set_implausibility(struct DriverInput* const restrict self,
        const enum IMPL impl,const uint8_t value);
void clear_implausibility(struct DriverInput* const restrict self);
uint8_t check_impls(const struct DriverInput* const restrict self, const uint8_t impls);

void driver_input_free_read_ptr(void);
void driver_input_free_mut_ptr(void);

#define DRIVER_INPUT_READ_ONLY_ACTION(exp)\
{\
    const struct DriverInput* const driver_input_read_ptr = driver_input_get();\
    exp;\
    driver_input_free_read_ptr();\
}

#define DRIVER_INPUT_MUT_ACTION(exp)\
{\
    struct DriverInput* const driver_input_mut_ptr = driver_input_get_mut();\
    exp;\
    driver_input_free_mut_ptr();\
}

#endif // !__DRIVER_INPUT__
