#ifndef __DV_STATUS__
#define __DV_STATUS__

#include <stdint.h>

//INFO: check dbc of can3 in message DV_system_status
enum AS_STATUS{
    AS_OFF =1,
    AS_READY =2,
    AS_DRIVING =3,
    AS_EMERGENCY =4,
    AS_FINISHED =5,
};

struct DvStatus;

struct DvStatus* dv_status_class_init(void);
struct DvStatus* dv_status_class_get(void);
int8_t dv_status_set(struct DvStatus* const restrict self, const enum AS_STATUS status);
int8_t dv_status_get(const struct DvStatus* const restrict self);

#endif // !__DV_STATUS__
