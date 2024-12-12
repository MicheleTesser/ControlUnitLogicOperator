#include "./raceup_board/raceup_board.h"
#include <bits/types/struct_timeval.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/time.h>

#define MS * 1000

int8_t wait_milliseconds(const time_var_microseconds milliseconds)
{
    usleep(milliseconds MS);
    return 0;
}

time_var_microseconds timer_time_now(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (time_var_microseconds)(tv.tv_sec * 1000000LL + tv.tv_usec); // Convert to microseconds

}

uint8_t is_time_passed(time_var_microseconds old_time, time_var_microseconds time_tick, time_var_microseconds amount_passed){
    return (time_tick - old_time) >= (amount_passed);
}
