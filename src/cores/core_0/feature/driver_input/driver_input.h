#ifndef __CORE_0_DRIVER_INPUT__
#define __CORE_0_DRIVER_INPUT__

#include <stdint.h>

typedef struct DriverInput_h{
    uint8_t private_data[64];
}DriverInput_h;

enum INPUT_TYPES{
    THROTTLE =0,
    BRAKE,
    STEERING_ANGLE,

    __NUM_OF_INPUT_TYPES__
};

enum DRIVER{
    DRIVER_HUMAN=0,
    DRIVER_EMBEDDED,

    DRIVER_NONE,
    __NUM_OF_DRIVERS__
};

int8_t 
driver_input_init(DriverInput_h* const restrict self )__attribute__((__nonnull__(1)));

int8_t
driver_input_update(DriverInput_h* const restrict self )__attribute__((__nonnull__(1)));

float
driver_input_get(const DriverInput_h* const restrict self ,
        const enum INPUT_TYPES driver_input)__attribute__((__nonnull__(1)));

int8_t
driver_input_rtd_request(const DriverInput_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t driver_input_change_driver(struct DriverInput_h* const restrict self,
        const enum DRIVER driver)__attribute__((__nonnull__(1)));

void 
driver_input_destroy(DriverInput_h* const restrict self )__attribute__((__nonnull__(1)));


#endif // !__CORE_0_DRIVER_INPUT__
