#ifndef __BATTERY_HV__
#define __BATTERY_HV__

#include <stdint.h>
#include "../../log/log.h"

typedef struct __attribute__((aligned(8))){
    const uint8_t private_data[16];
}Bms_h;

int8_t
bms_init(Bms_h* const restrict self ,
        const uint16_t bms_id, const uint8_t mex_size,
        const char* const restrict bms_name,
        Log_h* const restrict log)__attribute__((__nonnull__(1,4,5)));

int8_t
bms_update(Bms_h* const restrict self)__attribute__((__nonnull__(1)));

#endif // !__BATTERY_HV__
