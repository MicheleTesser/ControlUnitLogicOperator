#ifndef __DV_EBS__
#define __DV_EBS__

#include <stdint.h>

typedef struct DvEbs_h{
    const uint8_t private_data[1];
}DvEbs_h;

int8_t ebs_class_init(DvEbs_h* const restrict self __attribute__((__nonnull__)));
int8_t ebs_update(DvEbs_h* const restrict self __attribute__((__nonnull__)));
int8_t ebs_on(DvEbs_h* const restrict self __attribute__((__nonnull__)));

#endif // !__DV_EBS__
