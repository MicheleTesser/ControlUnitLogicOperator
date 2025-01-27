#ifndef __BATTERY_HV__
#define __BATTERY_HV__

#include <stdint.h>
#include "../../log/log.h"

typedef struct Bms_h{
    const uint8_t private_data[1];
}Bms_h;

int8_t
bms_init(Bms_h* const restrict self __attribute__((__nonnull__)),
        const uint16_t bms_mex_id, const char* const restrict bms_name,
        Log_h* const restrict log __attribute__((__unused__)));

int8_t
bms_update(Bms_h* const restrict self __attribute__((__nonnull__)));

#endif // !__BATTERY_HV__
