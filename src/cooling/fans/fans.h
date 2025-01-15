#ifndef __CAR_FANS__
#define __CAR_FANS__

#include <stdint.h>

//INFO: leave the incremental value of the enums
enum FAN_TYPES {
    FAN_BMS_HV =0,
    FANS_RADIATOR,

    NUMBER_OF_FAN_TYPES,//INFO: do not use this enum.
};

struct Fan;

int8_t fan_init(void);
struct Fan* fan_get_mut(const enum FAN_TYPES type);

int8_t fan_enable(struct Fan* const restrict self);
int8_t fan_disable(struct Fan* const restrict self);
int8_t fan_set_speed(struct Fan* const restrict self, const float value);

#define FAN_MUT_ACTION(type,exp)\
{\
    struct Fan* fan_mut_ptr = fan_get_mut(type);\
    exp;\
}

//debug

float debug_fan_get_value(const enum FAN_TYPES fan);

#endif // !__CAR_FANS__
