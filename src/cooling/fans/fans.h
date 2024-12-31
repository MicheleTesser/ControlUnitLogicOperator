#ifndef __CAR_FANS__
#define __CAR_FANS__

#include <stdint.h>

//INFO: leave the incremental value of the enums
enum FAN_TYPES {
    FAN_BMS_HV =0,
    FANS_RADIATOR,

    NUMBER_OF_FAN_TYPES,//INFO: do not use this enum.
};

int8_t fan_init(void);
int8_t fan_enable(const uint8_t fans);
int8_t fan_disable(const uint8_t fans);
int8_t fan_set_value(const enum FAN_TYPES fan, const float value);

//debug

float debug_fan_get_value(const enum FAN_TYPES fan);

#endif // !__CAR_FANS__
