#ifndef __CAR_DV__
#define __CAR_DV__

#include <stdint.h>

//INFO: check dbc of can3 in message DV_system_status
enum AS_STATUS{
    AS_OFF =1,
    AS_READY =2,
    AS_DRIVING =3,
    AS_EMERGENCY =4,
    AS_FINISHED =5,
};

int8_t dv_class_init(void);
int8_t dv_set_status(const enum AS_STATUS status);
int8_t dv_go(void);
int8_t dv_compute(void);

#endif // !__CAR_DV__
