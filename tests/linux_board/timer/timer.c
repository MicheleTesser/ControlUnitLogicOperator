#include "./timer.h"
#include <bits/types/struct_timeval.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/time.h>

int8_t wait_milliseconds(const time_var_microseconds time)
{
    usleep((unsigned int) time);
    return 0;
}

time_var_microseconds timer_time_now(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (time_var_microseconds)(tv.tv_sec SECONDS + tv.tv_usec); // Convert to microseconds

}
