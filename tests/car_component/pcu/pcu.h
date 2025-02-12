#ifndef __CAR_PCU__
#define __CAR_PCU__

#include <stdint.h>

typedef struct Pcu_h{
  const uint8_t private_data[16];
}Pcu_h;

int8_t
pcu_init(struct Pcu_h* const restrict self)__attribute__((__nonnull__));

int8_t
pcu_update(struct Pcu_h* const restrict self)__attribute__((__nonnull__));

#endif // !__CAR_PCU__
