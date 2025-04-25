#ifndef __CAR_SPEED__
#define __CAR_SPEED__

#include <stdint.h>

typedef struct __attribute__((aligned(1))){
  const uint8_t private_data[1];
}CarSpeedMut_h;

int8_t car_speed_mut_init(CarSpeedMut_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t car_speed_mut_update(CarSpeedMut_h* const restrict self, const float engines_rpm[4])
  __attribute__((__nonnull__(1,2)));

uint32_t car_speed_get(void);

#endif // !__CAR_SPEED__
