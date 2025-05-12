#ifndef __RUNNING_STATUS__
#define __RUNNING_STATUS__

#include <stdint.h>

enum RUNNING_STATUS{
    SYSTEM_OFF = 0,
    SYSTEM_PRECAHRGE =1,
    TS_READY =2,
    RUNNING =3,
};

typedef struct __attribute__((aligned(1))){
  const uint8_t private_data[1];
}GlobalRunningStatusMut_h;


int8_t
global_running_status_mut_init(GlobalRunningStatusMut_h* const restrict self)
  __attribute__((__nonnull__(1)));

int8_t
global_running_status_set(GlobalRunningStatusMut_h* const restrict self,
    const enum RUNNING_STATUS status)__attribute__((__nonnull__(1)));

enum RUNNING_STATUS
global_running_status_get(void);

#endif // !__RUNNING_STATUS__
