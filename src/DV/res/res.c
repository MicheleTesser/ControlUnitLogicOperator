#include "res.h"
#include "../../lib/raceup_board/components/timer.h"
#include <stdint.h>

static struct DvRes{
    time_var_microseconds go_time_start;
    uint8_t init_done:1;
    uint8_t mut_ptr:1;
    uint8_t read_ptr:1;
}RES;

int8_t res_class_init(void)
{
    RES.go_time_start =0;
    RES.init_done =1;
    return 0;
}

const struct DvRes* res_class_get(void)
{
    while(!RES.init_done && RES.mut_ptr){}
    RES.read_ptr++;
    return &RES;
}

struct DvRes* res_class_get_mut(void)
{
    while(!RES.init_done && RES.mut_ptr && RES.read_ptr){}
    RES.mut_ptr++;
    return &RES;
}

int8_t res_check_go(const struct DvRes* self)
{
    return self->go_time_start && (timer_time_now() - self->go_time_start) > 5 SECONDS;
}

int8_t res_start_time(struct DvRes* self)
{
    self->go_time_start = timer_time_now();
    return 0;
}

void res_free_read_ptr(void)
{
    RES.read_ptr--;
}
void res_free_write_ptr(void)
{
    RES.mut_ptr--;
}
