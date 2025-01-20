#ifndef __DV_SPEED__
#define __DV_SPEED__

#include <stdint.h>

typedef struct DvSpeed_h{
    const uint8_t private_data[1];
}DvSpeed_h;

int8_t
dv_speed_init(DvSpeed_h* const restrict self __attribute__((__nonnull__)));

int8_t
dv_speed_update(DvSpeed_h* const restrict self __attribute__((__nonnull__)));

float
dv_speed_get(DvSpeed_h* const restrict self __attribute__((__nonnull__)));

#endif // !__DV_SPEED__
