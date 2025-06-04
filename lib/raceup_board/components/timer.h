#ifndef __VIRTUAL_TIMER__
#define __VIRTUAL_TIMER__

#include <stdint.h>

typedef int64_t time_var_microseconds; //microseconds

extern void timer_init(void);
extern time_var_microseconds timer_time_now(void);
extern void timer_wait(const time_var_microseconds ticks);

extern time_var_microseconds get_tick_from_millis(uint32_t millis);
extern time_var_microseconds get_tick_from_micros(uint32_t micros);

#define ACTION_ON_FREQUENCY(time_var, freq)\
  for(;(timer_time_now() - time_var) > freq; time_var = timer_time_now())

#endif// !__VIRTUAL_TIMER__
