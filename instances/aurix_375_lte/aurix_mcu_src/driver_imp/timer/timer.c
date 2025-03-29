#include "../../src/lib/raceup_board/raceup_board.h"
#include "../raceup_board/components/timer.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "IfxPort.h"
#include "Bsp.h"
#pragma GCC diagnostic pop

void timer_init(void)
{
}


time_var_microseconds timer_time_now(void)
{
  return now();
}

