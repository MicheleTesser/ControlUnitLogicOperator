#ifndef __EMBEDDED_SYSTEM__
#define __EMBEDDED_SYSTEM__

#include <stdint.h>

typedef struct __attribute__((aligned(8))) EmbeddedSystem_h{
  const uint8_t private_data[48];
}EmbeddedSystem_h;

//INFO: copied from src/cores/core_2/feature/DV/dv.c
enum MISSION_STATUS{
    MISSION_NOT_RUNNING=0,
    MISSION_RUNNING,
    MISSION_FINISHED,
};

enum EMBEDDED_DV_INPUT{ //INFO: range 0-100
  DV_INPUT_THROTTLE =0,
  DV_INPUT_BRAKE,
  DV_INPUT_STEERING_ANGLE,
  
  __NUM_OF_EMBEDDED_IMPUT__,
};

int8_t
embedded_system_start(EmbeddedSystem_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t
embedded_system_set_dv_input(EmbeddedSystem_h* const restrict self,
    const enum EMBEDDED_DV_INPUT dv_input_type, const uint8_t value)__attribute__((__nonnull__(1)));

int8_t
embedded_system_set_mission_status(EmbeddedSystem_h* const restrict self,
    const enum MISSION_STATUS mission_status)__attribute__((__nonnull__(1)));

int8_t
embedded_system_stop(EmbeddedSystem_h* const restrict self)__attribute__((__nonnull__(1)));

#endif // !__EMBEDDED_SYSTEM__
