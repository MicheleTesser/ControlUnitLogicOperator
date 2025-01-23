#ifndef __BATTERY_LV__
#define __BATTERY_LV__

#include <stdint.h>
#include "../../log/log.h"

typedef struct Lv_h{
    const uint8_t private_data[1];
}Lv_h;

int8_t
lv_init(Lv_h* const restrict self __attribute__((__nonnull__)),
       Log_h* const restrict log __attribute__((__unused__)));

int8_t
lv_update(Lv_h* const restrict self __attribute__((__nonnull__)));

#endif // !__BATTERY_LV__
