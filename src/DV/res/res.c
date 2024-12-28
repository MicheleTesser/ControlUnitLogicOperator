#include "res.h"
#include "../../lib/raceup_board/components/timer.h"
#include <stdint.h>

static struct{
    time_var_microseconds go_time_start;
}RES;

int8_t res_class_init(void)
{
    RES.go_time_start =0;
    return 0;
}

int8_t res_start_time(void)
{
    RES.go_time_start = timer_time_now();
    return 0;
}

int8_t res_check_go(void)
{
    return RES.go_time_start && (timer_time_now() - RES.go_time_start) > 5 SECONDS;
}
