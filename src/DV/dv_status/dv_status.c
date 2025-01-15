#include "./dv_status.h"
#include "../../emergency_fault/emergency_fault.h"
#include <stdint.h>
#include <stddef.h>

struct DvStatus{
    enum AS_STATUS status;
    uint8_t init_done: 1;
    uint8_t mut_ptr: 1;
    uint8_t read_ptr : 6;
}DV_STATUS;

int8_t dv_status_class_init(void)
{
    DV_STATUS.status = AS_OFF;
    DV_STATUS.init_done =1;
    return 0;
}
const struct DvStatus* dv_status_class_get(void)
{
    while (!DV_STATUS.init_done && DV_STATUS.mut_ptr) {}
    DV_STATUS.read_ptr++;
    return &DV_STATUS;
}

struct DvStatus* dv_status_class_get_mut(void)
{
    while (!DV_STATUS.init_done && DV_STATUS.mut_ptr && DV_STATUS.mut_ptr) {}
    DV_STATUS.mut_ptr++;
    return &DV_STATUS;
}

int8_t dv_status_set(struct DvStatus* const restrict self, const enum AS_STATUS status)
{
    self->status = status;
    if (status != AS_EMERGENCY) {
        EMERGENCY_FAULT_MUT_ACTION({
            one_emergency_solved(emergency_mut_ptr, DV_EMERGENCY_STATE);
        })
    }
    switch (status) {
        case AS_OFF:
        case AS_READY:
        case AS_DRIVING:
        case AS_FINISHED:
            break;
        case AS_EMERGENCY:
            EMERGENCY_FAULT_MUT_ACTION({
                one_emergency_raised(emergency_mut_ptr, DV_EMERGENCY_STATE);
            });
            break;
    }
    return 0;
}

int8_t dv_status_get(const struct DvStatus* const restrict self)
{
    return self->status;
}

void dv_status_free_read_ptr(void)
{
    DV_STATUS.read_ptr--;
}
void dv_status_free_write_ptr(void)
{
    DV_STATUS.mut_ptr--;
}
