#include "../../src/lib/raceup_board/raceup_board.h"

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

void timer_wait(const time_var_microseconds ticks)
{
  waitTime(ticks);
}

time_var_microseconds get_tick_from_millis(uint32_t millis)
{
  return IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, millis);
}

time_var_microseconds get_tick_from_micros(uint32_t micros)
{
  return IfxStm_getTicksFromMicroseconds(BSP_DEFAULT_TIMER, micros);
}
