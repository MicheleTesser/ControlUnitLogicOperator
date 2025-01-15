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

int8_t dv_status_class_init(void);
const struct DvStatus* dv_status_class_get(void);
struct DvStatus* dv_status_class_get_mut(void);
int8_t dv_status_set(struct DvStatus* const restrict self, const enum AS_STATUS status);
int8_t dv_status_get(const struct DvStatus* const restrict self);

void dv_status_free_read_ptr(void);
void dv_status_free_write_ptr(void);

#define DV_STATUS_READ_ONLY_ACTION(exp) \
{\
    const struct DvStatus* dv_status_read_ptr = dv_status_class_get();\
    exp;\
    dv_status_free_read_ptr();\
}

#define DV_STATUS_MUT_ACTION(exp) \
{\
    struct DvStatus* dv_status_mut_ptr = dv_status_class_get_mut();\
    exp;\
    dv_status_free_write_ptr();\
}

#endif // !__DV_STATUS__
