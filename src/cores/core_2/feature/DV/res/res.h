#ifndef __DV_RES__
#define __DV_RES__

#include <stdint.h>

typedef struct DvRes_h{
    const uint8_t private_data[1];
}DvRes_h;

int8_t res_class_init(DvRes_h* const restrict self __attribute__((__nonnull__)));
int8_t res_check_go(const DvRes_h* self __attribute__((__nonnull__)));
int8_t res_start_time(DvRes_h* self __attribute__((__nonnull__)));

#endif // !__DV_RES__
