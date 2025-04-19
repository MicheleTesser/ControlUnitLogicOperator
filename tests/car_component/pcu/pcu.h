#ifndef __CAR_PCU__
#define __CAR_PCU__

#include <stdint.h>

typedef struct __attribute__((aligned(4))){
  const uint8_t private_data[40];
}Pcu_h;

int8_t
pcu_init(Pcu_h* const restrict self) __attribute__((__nonnull__(1)));

int8_t
pcu_stop(Pcu_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t
pcu_start_embedded(Pcu_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t
pcu_stop_embedded(Pcu_h* const restrict self)__attribute__((__nonnull__(1)));

#endif // !__CAR_PCU__
