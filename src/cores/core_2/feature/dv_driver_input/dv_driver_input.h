#ifndef __DV_DRIVER_INPUT__
#define __DV_DRIVER_INPUT__

#include <stdint.h>

typedef struct DvDriverInput_h{
    const uint8_t private_data[1];
}DvDriverInput_h;


int8_t
dv_driver_input_init(DvDriverInput_h* const restrict self __attribute__((__nonnull__)));

int8_t
dv_driver_input_update(DvDriverInput_h* const restrict self __attribute__((__nonnull__)));

float
dv_driver_input_get_brake(const DvDriverInput_h* const restrict self __attribute__((__nonnull__)));

#endif // !__DV_DRIVVER_INPUT__
