#ifndef __CAR_DV__
#define __CAR_DV__

#include <stdint.h>

enum AS_STATUS{
    AS_OFF,
    AS_READY,
    AS_DRIVING,
    AS_EMERGENCY,
    AS_FINISHED,
};

int8_t dv_class_init(void);
int8_t dv_set_status(const enum AS_STATUS status);

int8_t dv_update_led(void);


#endif // !__CAR_DV__
