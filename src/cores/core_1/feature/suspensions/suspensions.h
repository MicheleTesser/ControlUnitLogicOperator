#ifndef __SUSPENSIONS__
#define __SUSPENSIONS__

#include <stdint.h>
#include "../log/log.h"
#include "../../../../lib/raceup_board/raceup_board.h"

#if ARCH == 64
typedef struct __attribute__((aligned(8))) Suspensions_h{
    const uint8_t private_data[32];
}Suspensions_h;
#elif ARCH == 32
typedef struct __attribute__((aligned(4))) Suspensions_h{
    const uint8_t private_data[24];
}Suspensions_h;
#else
#endif

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
