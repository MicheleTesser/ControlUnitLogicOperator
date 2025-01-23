#ifndef __BATTERY_HV__
#define __BATTERY_HV__

#include <stdint.h>
#include "../../log/log.h"

typedef struct Hv_h{
    const uint8_t private_data[1];
}Hv_h;

int8_t
hv_init(Hv_h* const restrict self __attribute__((__nonnull__)),
       Log_h* const restrict log __attribute__((__unused__)));

int8_t
hv_update(Hv_h* const restrict self __attribute__((__nonnull__)));

#endif // !__BATTERY_HV__
