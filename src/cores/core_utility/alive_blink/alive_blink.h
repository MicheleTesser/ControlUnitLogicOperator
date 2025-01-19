#ifndef __CORE_ALIVE_BLINK__
#define __CORE_ALIVE_BLINK__

#include <stdint.h>
#include "../../../../lib/raceup_board/raceup_board.h"

typedef struct CoreAliveBlink_h{
    const uint8_t private_data[1];
}CoreAliveBlink_h;

int8_t
core_alive_blink_init(
        CoreAliveBlink_h* const restrict self __attribute__((__nonnull__)),
        enum GPIO_PIN pin_led);

int8_t
core_alive_blink_update(CoreAliveBlink_h* const restrict self __attribute__((__nonnull__)));

#endif // !__CORE_ALIVE_BLINK__
