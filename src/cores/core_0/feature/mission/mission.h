#ifndef __CORE_O_MISSIONS__
#define __CORE_O_MISSIONS__
#include <stdint.h>
#include "../driver_input/driver_input.h"

typedef struct Mission_h{
    const uint8_t private_data[1];
}Mission_h;

int8_t mission_init(Mission_h* const restrict self __attribute__((__nonnull__)),
        DriverInput_h* const driver __attribute__((__nonnull__)));
int8_t mission_update(Mission_h* const restrict self __attribute__((__nonnull__)));
void mission_lock(Mission_h* const restrict self __attribute__((__nonnull__)));
void mission_unlock(Mission_h* const restrict self __attribute__((__nonnull__)));

#endif // !__CORE_O_MISSIONS__
