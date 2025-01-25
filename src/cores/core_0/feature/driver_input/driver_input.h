#ifndef __CORE_0_DRIVER_INPUT__
#define __CORE_0_DRIVER_INPUT__

#include <stdint.h>

typedef struct DriverInput_h{
    uint8_t private_data[32];
}DriverInput_h;

enum INPUT_TYPES{
    THROTTLE =0,
    BRAKE,
    STEERING_ANGLE,
    REGEN,

    __NUM_OF_INPUT_TYPES__
};

enum DRIVER{
    DRIVER_NONE=0,
    DRIVER_HUMAN,
    DRIVER_EMBEDDED,

    __NUM_OF_DRIVERS__
};

int8_t 
driver_input_init(DriverInput_h* const restrict self __attribute__((__nonnull__)));

int8_t
driver_input_update(DriverInput_h* const restrict self __attribute__((__nonnull__)));

float
driver_input_get(const DriverInput_h* const restrict self __attribute__((__nonnull__)),
        const enum INPUT_TYPES driver_input);

int8_t driver_input_change_driver(struct DriverInput_h* const restrict self,
        const enum DRIVER driver);

void 
driver_input_destroy(DriverInput_h* const restrict self __attribute__((__nonnull__)));

#endif // !__CORE_0_DRIVER_INPUT__
