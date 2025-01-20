#ifndef __GENERAL_CAN__
#define __GENERAL_CAN__

#include <stdint.h>
typedef struct GeneralCan_h{
    const uint8_t private_data[1];
}GeneralCan_h;

int8_t general_can_init(GeneralCan_h* const restrict self);

#endif // !__GENERAL_CAN__
