#ifndef __CAR_AMK_INVERTER__
#define __CAR_AMK_INVERTER__

#include <stdint.h>

enum INVERTER_ATTRIBUTE {
    ERROR,
    WARNING,
    DERATRING,
    HV,
    HV_ACK,
    INVERTER_ON,
    INVERTER_ON_ACK,
    SYSTEM_READY,
};

void 
car_amk_inverter_class_init(void);
int8_t 
car_amk_inverter_set_attribute(const enum INVERTER_ATTRIBUTE attribute, const uint8_t engine,
        const int64_t value);

#endif // !__CAR_AMK_INVERTER__
