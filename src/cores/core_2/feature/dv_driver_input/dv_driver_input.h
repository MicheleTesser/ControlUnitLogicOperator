#ifndef __DV_DRIVER_INPUT__
#define __DV_DRIVER_INPUT__

#include <stdint.h>

typedef struct __attribute__((aligned(8))) DvDriverInput_h{
    const uint8_t private_data[40];
}DvDriverInput_h;


int8_t
dv_driver_input_init(DvDriverInput_h* const restrict self )__attribute__((__nonnull__(1)));

int8_t
dv_driver_input_update(DvDriverInput_h* const restrict self )__attribute__((__nonnull__(1)));

float
dv_driver_input_get_brake(const DvDriverInput_h* const restrict self )__attribute__((__nonnull__(1)));

#endif // !__DV_DRIVVER_INPUT__
