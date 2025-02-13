#ifndef __ATC__
#define __ATC__

#include <stdint.h>
typedef struct Atc_h{
  const uint8_t private_data[40];
}Atc_h;

enum InputAtc
{
  ATC_THROTTLE,
  ATC_BRAKE,
  ATC_STEERING_ANGLE,
};

int8_t
atc_start(Atc_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t
atc_pedals_steering_wheel(Atc_h* const restrict self, const enum InputAtc input, const float value);

int8_t
atc_stop(Atc_h* const restrict self)__attribute__((__nonnull__(1)));

#endif // !__ATC__
