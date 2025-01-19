#ifndef __CULO_CORE_STATUS__
#define __CULO_CORE_STATUS__

#include <stdint.h>

enum CORES{
    CORE_0 =0,
    CORE_1,
    CORE_2,

    __NUM_OF_CORES__
};

void core_status_core_ready(const enum CORES core);
uint8_t core_status_ready_state(void);

#endif // !__CULO_CORE_STATUS__
