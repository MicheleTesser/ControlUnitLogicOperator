#include "hv.h"
#include <string.h>

struct BasicHv_t{
    const float lem_current;
};

union BasicHv_const_conv{
    const struct BasicHv_h* const hidden;
    const struct BasicHv_t* const clear;
};

int8_t
basic_hv_init(struct BasicHv_h* const restrict self __attribute__((__nonnull__)))
{
    memset(self, 0, sizeof(*self));
    return 0;
}

float
basic_hv_get_info(const struct BasicHv_h* const restrict self __attribute__((__nonnull__)),
        const enum HV_INFO info)
{
    const union BasicHv_const_conv conv = {self};
    const struct BasicHv_t* const p_self = conv.clear;
    switch (info) {
        case HV_LEM_CURRENT:
            return p_self->lem_current;
            break;
        default:
            return -1;
    }
}
