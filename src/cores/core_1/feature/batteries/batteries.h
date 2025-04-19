#ifndef __CAR_BATTERIES__
#define __CAR_BATTERIES__

#include <stdint.h>
#include "../log/log.h"

typedef struct __attribute__((aligned(4))){
  const uint8_t private_data[32];
}CarBatteries_h;

int8_t
car_batteries_init(CarBatteries_h* const restrict self, Log_h* const restrict log)
  __attribute__((__nonnull__(1,2)));

int8_t
car_batteries_update(CarBatteries_h* const restrict self)__attribute__((__nonnull__(1)));

#endif // !__CAR_BATTERIES__
