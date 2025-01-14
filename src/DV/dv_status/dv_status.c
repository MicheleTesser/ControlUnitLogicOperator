#include "./dv_status.h"
#include "../../emergency_fault/emergency_fault.h"
#include <stdint.h>
#include <stddef.h>

struct DvStatus{
    enum AS_STATUS status;
    uint8_t init_done: 1;
}DV_STATUS;

struct DvStatus* dv_status_class_init(void)
{
    if (!DV_STATUS.init_done) {
        DV_STATUS.status = AS_OFF;
        DV_STATUS.init_done =1;
        return &DV_STATUS;
    }
    return NULL;
}
struct DvStatus* dv_status_class_get(void)
{
    while (!DV_STATUS.init_done) {}
    return &DV_STATUS;
}

int8_t dv_status_set(struct DvStatus* const restrict self, const enum AS_STATUS status)
{
    self->status = status;
    if (status != AS_EMERGENCY) {
        one_emergency_solved(DV_EMERGENCY_STATE);
    }
    switch (status) {
        case AS_OFF:
        case AS_READY:
        case AS_DRIVING:
        case AS_FINISHED:
            break;
        case AS_EMERGENCY:
            one_emergency_raised(DV_EMERGENCY_STATE);
            break;
    }
    return 0;
}

int8_t dv_status_get(const struct DvStatus* const restrict self)
{
    return self->status;
}
