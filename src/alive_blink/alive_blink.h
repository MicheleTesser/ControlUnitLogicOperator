#ifndef __ALIVE_BLINK_CORE_0__
#define __ALIVE_BLINK_CORE_0__

#include "../lib/raceup_board/raceup_board.h"
#include <stdint.h>

#define MILLIS * 1000

typedef uint8_t alive_blink_fd;

alive_blink_fd i_m_alive_init(
        const time_var_microseconds frequency,
        const uint8_t pin_blink);

void i_m_alive(const alive_blink_fd id);

#endif // !__ALIVE_BLINK_CORE_0__
