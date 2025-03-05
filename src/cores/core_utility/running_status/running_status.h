#ifndef __RUNNING_STATUS__
#define __RUNNING_STATUS__

#include <stdint.h>

enum RUNNING_STATUS{
    SYSTEM_OFF = 0,
    SYSTEM_PRECAHRGE =1,
    TS_READY =2,
    RUNNING =3,
};

enum RUNNING_STATUS_OBJ_PERMISSIONS{
  READ,
  WRITE=1,

  __NUM_OF_RUNNING_STATUS_OBJ_PERMISSIONS__
};

typedef struct __attribute__((aligned(4))){
  const uint8_t private_data[8];
}GlobalRunningStatus_h;


int8_t
global_running_status_init(GlobalRunningStatus_h* const restrict self,
    const enum RUNNING_STATUS_OBJ_PERMISSIONS permission)__attribute__((__nonnull__(1)));

int8_t
global_running_status_set(GlobalRunningStatus_h* const restrict self,
    const enum RUNNING_STATUS status)__attribute__((__nonnull__(1)));

enum RUNNING_STATUS
global_running_status_get(GlobalRunningStatus_h* const restrict self)
  __attribute__((__nonnull__(1)));

#endif // !__RUNNING_STATUS__
