#ifndef __GENERAL_CAN__
#define __GENERAL_CAN__

#include <stdint.h>
struct GeneralCan_h{
    const uint8_t private_data[1];
};

int8_t general_can_init(struct GeneralCan_h* const restrict self);

#endif // !__GENERAL_CAN__
