#ifndef __EXTERNAL_GPIO__
#define __EXTERNAL_GPIO__

#include "src/lib/raceup_board/components/gpio.h"

enum EXTERNAL_GPIO
{
  GPIO_INVERTER_RF_SIGNAL = __NUM_OF_GPIOS__,
  GPIO_PCU_EMBEDDED_SYSTEM,

  __NUM_OF_EXTERNAL_GPIO__
};

#endif // !__EXTERNAL_GPIO__
