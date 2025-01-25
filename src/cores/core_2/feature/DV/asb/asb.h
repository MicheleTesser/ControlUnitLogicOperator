#ifndef __DV_ASB__
#define __DV_ASB__

#include <stdint.h>
#include "ebs/ebs.h"

typedef struct DvAsb_h{
    DvEbs_h dv_ebs;
    const uint8_t private_data[1];
}DvAsb_h;

int8_t asb_class_init(DvAsb_h* const restrict self)__attribute__((__nonnull__(1)));
int8_t asb_update(DvAsb_h* const restrict self)__attribute__((__nonnull__(1)));
int8_t asb_consistency_check(DvAsb_h* const restrict self)__attribute__((__nonnull__(1)));


#endif // !__DV_ASB__
