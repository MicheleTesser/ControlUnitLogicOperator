#ifndef __DRIVVER_INPUT_CORE_1__
#define __DRIVVER_INPUT_CORE_1__

#include <stdint.h>
#include "../log/log.h"

typedef struct Core1DriverInput_h{
    const uint8_t private_data[1];
}Core1DriverInput_h;

int8_t
core_1_driver_input_init(
        Core1DriverInput_h* const restrict self __attribute__((__nonnull__)),
        Log_h* const restrict log __attribute__((__nonnull__)));

int8_t
core_1_driver_input_update(Core1DriverInput_h* const restrict self);

#endif // !__DRIVVER_INPUT__
