#ifndef __VIRTUAL_TIMER__
#define __VIRTUAL_TIMER__

#include <stdint.h>

#define MICROS * 1 
#define MILLIS * 1000 MICROS
#define SECONDS * 1000 MILLIS 

typedef int64_t time_var_microseconds; //microseconds

extern void timer_init(void);
extern time_var_microseconds timer_time_now(void);

#define ACTION_ON_FREQUENCY(time_var, freq)\
  for(;(timer_time_now() - time_var) > freq; time_var = timer_time_now())

#endif// !__VIRTUAL_TIMER__
