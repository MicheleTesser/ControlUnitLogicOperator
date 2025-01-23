#include "batteries.h"

#include "lv/lv.h"
#include "hv/hv.h"
#include <stdint.h>

struct CarBatteries_t{
    Hv_h hv;
    Lv_h lv;
};

union CarBatteries_h_t_conv{
    CarBatteries_h* const restrict hidden;
    struct CarBatteries_t* const restrict clear;
};

int8_t
car_batteries_init(CarBatteries_h* const restrict self __attribute__((__unused__)),
        Log_h* const restrict log __attribute__((__unused__)))
{
    union CarBatteries_h_t_conv conv = {self};
    struct CarBatteries_t* const restrict p_self = conv.clear;

    if (hv_init(&p_self->hv, log) <0)
    {
        return -1;
    }

    if (lv_init(&p_self->lv, log) <0)
    {
        return -2;
    }

    return 0;
}

int8_t
car_batteries_update(CarBatteries_h* const restrict self __attribute__((__unused__)))
{
    union CarBatteries_h_t_conv conv = {self};
    struct CarBatteries_t* const restrict p_self = conv.clear;

    if (hv_update(&p_self->hv)<0) {
        return -1;
    }

    if (lv_update(&p_self->lv)<0) {
        return -2;
    }

    return 0;
}
