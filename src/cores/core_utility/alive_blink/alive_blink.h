#ifndef __CORE_ALIVE_BLINK__
#define __CORE_ALIVE_BLINK__

#include <stdint.h>
#include "../../../lib/raceup_board/raceup_board.h"

typedef struct __attribute__((aligned(4))) CoreAliveBlink_h{
  const uint8_t private_data[24];
}CoreAliveBlink_h;


int8_t
core_alive_blink_init(
        CoreAliveBlink_h* const restrict self ,
        const enum GPIO_PIN pin_led, const time_var_microseconds freq)__attribute__((__nonnull__(1)));

int8_t
core_alive_blink_update(CoreAliveBlink_h* const restrict self)__attribute__((__nonnull__(1)));

#endif // !__CORE_ALIVE_BLINK__
