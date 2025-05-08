#include "res.h"
#include "../../../../../lib/raceup_board/components/timer.h"

#include <stdint.h>
#include <string.h>

struct DvRes_t{
    time_var_microseconds start_go_timeout;
};

union DvRes_h_t_conv{
    DvRes_h* const restrict hidden;
    struct DvRes_t* const restrict clear;
};

union DvRes_h_t_conv_const{
    const DvRes_h* const restrict hidden;
    const struct DvRes_t* const restrict clear;
};

#ifdef DEBUG
char __assert_size_res[(sizeof(DvRes_h) == sizeof(struct DvRes_t))? 1:-1];
char __assert_align_res[(_Alignof(DvRes_h) == _Alignof(struct DvRes_t))? 1:-1];
#endif // DEBUG

//TODO: add mailbox res message
int8_t res_class_init(DvRes_h* const restrict self )
{
    union DvRes_h_t_conv conv = {self};
    struct DvRes_t* const restrict p_self = conv.clear;
    memset(p_self, 0, sizeof(*p_self));

    p_self->start_go_timeout = -1;

    return 0;
}

int8_t res_check_go(const DvRes_h* self )
{
    union DvRes_h_t_conv_const conv = {self};
    const struct DvRes_t* const restrict p_self = conv.clear;
    return (timer_time_now() - p_self->start_go_timeout) >= get_tick_from_millis(5000); //INFO: T 14.9.3 of rules
}

int8_t res_start_time_go(DvRes_h* self )
{
    union DvRes_h_t_conv conv = {self};
    struct DvRes_t* const restrict p_self = conv.clear;

    p_self->start_go_timeout = timer_time_now();
    return 0;
}
