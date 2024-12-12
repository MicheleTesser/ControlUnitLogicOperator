#ifndef __VIRTUAL_TIMER__
#define __VIRTUAL_TIMER__

#include <stdint.h>

typedef int64_t time_var_microseconds; //microseconds

extern void timer_init(void);
extern int8_t wait_milliseconds(const time_var_microseconds time);
extern time_var_microseconds timer_time_now(void);
extern uint8_t is_time_passed(time_var_microseconds old_time, time_var_microseconds time_tick, time_var_microseconds amount_passed);

#endif// !__VIRTUAL_TIMER__
