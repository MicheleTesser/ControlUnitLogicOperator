#ifndef __CORE_O_MISSIONS__
#define __CORE_O_MISSIONS__
#include <stdint.h>
#include "../driver_input/driver_input.h"

typedef struct Mission_h{
    const uint8_t private_data[24];
}Mission_h;

int8_t mission_init(Mission_h* const restrict self ,
        DriverInput_h* const driver )__attribute__((__nonnull__(1)));
int8_t mission_update(Mission_h* const restrict self )__attribute__((__nonnull__(1)));
void mission_lock(Mission_h* const restrict self )__attribute__((__nonnull__(1)));
void mission_unlock(Mission_h* const restrict self )__attribute__((__nonnull__(1)));

#endif // !__CORE_O_MISSIONS__
