#ifndef __CORE_0_DRIVER_INPUT__
#define __CORE_0_DRIVER_INPUT__

#include <stdint.h>

struct DriverInput_h{
    uint8_t private_data[32];
};

enum INPUT_TYPES{
    THROTTLE =0,
    BRAKE =1,
    STEERING_ANGLE = 2,
    REGEN =3,

    __NUM_OF_INPUT_TYPES__
};

enum DRIVER{
    DRIVER_HUMAN =0,
    DRIVER_EMBEDDED,

    __NUM_OF_DRIVERS__
};

int8_t 
driver_input_init(
        struct DriverInput_h* const restrict self __attribute__((__nonnull__)),
        const uint16_t human_mailbox_number,
        const uint16_t dv_mailbox_number);

int8_t 
driver_input_update(struct DriverInput_h* const restrict self __attribute__((__nonnull__)));

int8_t
driver_input_change_driver(
        struct DriverInput_h* const restrict self __attribute__((__nonnull__)),
        const enum DRIVER driver);

float
driver_get_amount(
        const struct DriverInput_h* const restrict self __attribute__((__nonnull__)),
        const enum INPUT_TYPES driver_input);

void 
driver_input_destroy(struct DriverInput_h* const restrict self __attribute__((__nonnull__)));

#endif // !__CORE_0_DRIVER_INPUT__
