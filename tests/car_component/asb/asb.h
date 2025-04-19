#ifndef __DUMMY__
#define __DUMMY__

#include <stdint.h>

typedef struct __attribute__((aligned(4))){
  const uint8_t private_data[44];
}Asb_h;

enum ASB_CONFIG{
  TANK_LEFT_PRESSURE=0,
  TANK_LEFT_SANITY,

  TANK_RIGHT_PRESSURE,
  TANK_RIGHT_SANITY,

  INTEGRITY_CHECK_STATUS,

  __NUM_OF_ASB_CONFI__
};

int8_t
asb_start(Asb_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t
asb_set_parameter(Asb_h* const restrict self,
    const enum ASB_CONFIG param_type, const uint8_t value)__attribute__((__nonnull__(1)));

int8_t
asb_stop(Asb_h* const restrict self)__attribute__((__nonnull__(1)));

#endif // !__DUMMY__
