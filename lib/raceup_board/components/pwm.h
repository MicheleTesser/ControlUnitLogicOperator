#ifndef __VIRTUAL_PWM__
#define __VIRTUAL_PWM__

#include <stdint.h>

typedef struct{
  const uint8_t private_data[1];
}Pwm_h;

typedef enum{
  PWM_DV_ASSI_LIGHT_YELLOW=0,
  PWM_DV_ASSI_LIGHT_BLUE,

  __NUM_OF_PWM_PINS__
}Pwm_Pins;

extern int8_t
hardware_pwm_init(Pwm_h* const restrict self, const Pwm_Pins pin)
  __attribute__((__nonnull__(1)));

extern int8_t
hardware_pwm_write(Pwm_h* const restrict self, const uint32_t duty_cicle)
  __attribute__((__nonnull__(1)));

#endif // !__VIRTUAL_PWM__
