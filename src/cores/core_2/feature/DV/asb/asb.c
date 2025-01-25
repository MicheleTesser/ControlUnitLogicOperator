#include "asb.h"
#include "ebs/ebs.h"
#include <string.h>

struct DvAsb_t{
    DvEbs_h dv_ebs;
};

union DvAsb_h_t_conv
{
    DvAsb_h* const restrict hidden;
    struct DvAsb_t* const restrict clear;
};

int8_t asb_class_init(DvAsb_h* const restrict self __attribute__((__nonnull__)))
{
    union DvAsb_h_t_conv conv = {self};
    struct DvAsb_t* const restrict p_self = conv.clear;

    memset(p_self, 0, sizeof(*p_self));

    if (ebs_class_init(&p_self->dv_ebs)<0)
    {
        return -1;
    }

    return 0;
}
int8_t asb_update(DvAsb_h* const restrict self __attribute__((__nonnull__)))
{
    union DvAsb_h_t_conv conv = {self};
    struct DvAsb_t* const restrict p_self = conv.clear;

    if (ebs_update(&p_self->dv_ebs)<0)
    {
        return -1;
    }

    return 0;
}

int8_t asb_consistency_check(DvAsb_h* const restrict self __attribute__((__nonnull__)))
{
    union DvAsb_h_t_conv conv = {self};
    struct DvAsb_t* const restrict p_self __attribute__((__nonnull__)) = conv.clear;
    
    //TODO: not yet defined

    return -1;
}
