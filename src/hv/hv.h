#ifndef __BASIC_HV__
#define __BASIC_HV__

#include <stdint.h>

typedef struct BasicHv_h{
    const uint8_t private_data[1];
}BasicHv_h;

enum HV_INFO{
    HV_LEM_CURRENT,
};

int8_t
basic_hv_init(BasicHv_h* const restrict self __attribute__((__nonnull__)),
        const uint16_t mailbox);

float
basic_hv_get_info(const BasicHv_h* const restrict self __attribute__((__nonnull__)),
        const enum HV_INFO info);

#endif // !__BASIC_HV__
