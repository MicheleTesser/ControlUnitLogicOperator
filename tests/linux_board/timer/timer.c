#include "./timer.h"
#include <bits/types/struct_timeval.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#define MILLIS * 1000
#define SECONDS * 1000 MILLIS

int8_t wait_milliseconds(const time_var_microseconds time)
{
    usleep(time);
    return 0;
}

time_var_microseconds get_tick_from_millis(uint32_t millis)
{
  return millis MILLIS;
}

time_var_microseconds get_tick_from_micros(uint32_t micros)
{
  return micros;
}

time_var_microseconds timer_time_now(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (time_var_microseconds)( ( (uint64_t) tv.tv_sec) SECONDS + (uint64_t)tv.tv_usec); // Convert to microseconds

}
