#ifndef __RUNNING_STATUS__
#define __RUNNING_STATUS__

#include <stdint.h>

enum RUNNING_STATUS{
    SYSTEM_OFF = 0,
    SYSTEM_PRECAHRGE =1,
    TS_READY =2,
    RUNNING =3,
};

int8_t global_running_status_set(const enum RUNNING_STATUS status);
enum RUNNING_STATUS global_running_status_get(void);

#endif // !__RUNNING_STATUS__
