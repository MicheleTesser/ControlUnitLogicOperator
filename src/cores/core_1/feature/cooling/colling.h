#ifndef __COOLING__
#define __COOLING__

#include <stdint.h>
#include "../../../../lib/raceup_board/components/can.h"
#include "../log/log.h"

typedef struct __attribute__((aligned(8))){
  const uint8_t private_data[32];
}Cooling_h;

enum COOLING_DEVICES{
  PUMPS=0,
  FANS_RADIATOR,

  __NUM_OF_COOLING_DEVICES__
};

int8_t
cooling_init(Cooling_h* const restrict self ,
    Log_h* const restrict log )__attribute__((__nonnull__(1,2)));

int8_t
cooling_update(Cooling_h* const restrict self)__attribute__((__nonnull__(1)));

#endif // !__COOLING__
