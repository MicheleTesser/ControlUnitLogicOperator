#ifndef __DV_SPEED__
#define __DV_SPEED__

#include <stdint.h>

typedef struct DvSpeed_h{
    const uint8_t private_data[8];
}DvSpeed_h;

int8_t
dv_speed_init(DvSpeed_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t
dv_speed_update(DvSpeed_h* const restrict self)__attribute__((__nonnull__(1)));

float
dv_speed_get(DvSpeed_h* const restrict self)__attribute__((__nonnull__(1)));

#endif // !__DV_SPEED__
