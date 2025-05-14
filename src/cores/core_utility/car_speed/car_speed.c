#include "car_speed.h"
#include <stdatomic.h>
#include <stdint.h>

#define WHEEL_RADIUS 5.0f
#define PI 3.14f

typedef uint32_t SpeedType; //INFO: Km/h

struct CarSpeedMut_t{
  uint8_t owner;
};

union CarSpeedMut_h_t_conv
{
  CarSpeedMut_h* const restrict hidden;
  struct CarSpeedMut_t* const restrict clear;
};

#ifdef DEBUG
char __assert_size_car_speed_mut[sizeof(CarSpeedMut_h)==sizeof(struct CarSpeedMut_t)?+1:-1];
char __assert_align_car_speed_mut[_Alignof(CarSpeedMut_h)==_Alignof(struct CarSpeedMut_t)?+1:-1];
#endif /* ifdef DEBUG */

static struct{
  atomic_long value; 
  atomic_bool owned;
}SPEED;

//public

int8_t car_speed_mut_init(CarSpeedMut_h* const restrict self)
{
  union CarSpeedMut_h_t_conv conv = {self};
  struct CarSpeedMut_t* p_self = conv.clear;
  if (!atomic_load(&SPEED.owned))
  {
    atomic_store(&SPEED.owned, 1);
    p_self->owner = 1;
    return 0;
  }
  return -1;
}

int8_t car_speed_mut_update(CarSpeedMut_h* const restrict self, const float engines_rpm[4])
{
  union CarSpeedMut_h_t_conv conv = {self};
  struct CarSpeedMut_t* p_self  = conv.clear;

  if (p_self->owner)
  {
    const float avg_rpm = (engines_rpm[0] + engines_rpm[1] + engines_rpm[2] + engines_rpm[3])/4;
    const float scalar_conv = 1.130400e+02f; //INFO:  2.0f * 3.6f * PI * WHEEL_RADIUS;
    const float new_speed = scalar_conv * (avg_rpm/60.0f);

    atomic_store(&SPEED.value,new_speed);


    return 0;
  }

  return -1;
}

SpeedType car_speed_get(void)
{
  return atomic_load(&SPEED.value);
}
