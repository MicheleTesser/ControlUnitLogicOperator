#ifndef __SUSPENSIONS__
#define __SUSPENSIONS__

#include <stdint.h>
#include "../log/log.h"

typedef struct Suspensions_h{
    const uint8_t private_data[1];
}Suspensions_h;

enum SUSP_ID{
    SUSP_FRONT_LEFT = 0,
    SUSP_FRONT_RIGHT,
    SUSP_REAR_LEFT,
    SUSP_REAR_RIGHT,

    __NUM_OF_SUSPS__
};


int8_t
suspensions_init(
        Suspensions_h* const restrict self ,
        Log_h* const restrict log )__attribute__((__nonnull__(1,2)));

int8_t
suspensions_update(Suspensions_h* const restrict self)__attribute__((__nonnull__(1)));

#endif // !__SUSPENSIONS__
