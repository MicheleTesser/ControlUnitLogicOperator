#ifndef __CORE_STATUS__

#include <stdint.h>

enum CORE_STATUS{
    CORE_INIT = (1<<0),
    CORE_IDLE= (1<<1),
    CORE_READY= (1<<2),
    CORE_ERROR= (1<<3),
};

enum CORE_STATUS core_status(const uint8_t core);
int8_t core_update_status(const uint8_t core, const enum CORE_STATUS status);

#endif // !__CORE_STATUS__
