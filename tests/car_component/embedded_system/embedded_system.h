#ifndef __EMBEDDED_SYSTEM__
#define __EMBEDDED_SYSTEM__

#include <stdint.h>

typedef struct __attribute__((aligned(4))){
  const uint8_t private_data[40];
}EmbeddedSystem_h;

//INFO: copied from src/cores/core_2/feature/DV/dv.c
enum EMBEDDED_STATUS{
    EMBEDDED_STATUS_OFF = 0,
    EMDEDDED_STATUS_READY,
    EMDEDDED_STATUS_RUNNING,
    EMDEDDED_STATUS_ERROR,
    EMDEDDED_STATUS_FINISHED
};

int8_t
embedded_system_start(EmbeddedSystem_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t
embedded_system_set_dv_input(EmbeddedSystem_h* const restrict self, const int8_t value)__attribute__((__nonnull__(1)));

int8_t
embedded_system_set_mission_status(EmbeddedSystem_h* const restrict self,
    const enum EMBEDDED_STATUS embedded_status)__attribute__((__nonnull__(1)));

void
embedded_system_stop(EmbeddedSystem_h* const restrict self)__attribute__((__nonnull__(1)));

#endif // !__EMBEDDED_SYSTEM__
