#include "batteries.h"
#include "../../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#include "bms/bms.h"

#include <stdint.h>

enum BMS_S{
    BMS_LV=0,
    BMS_HV,

    __NUM_OF_BMS__
};

struct CarBatteries_t{
    Bms_h bms[__NUM_OF_BMS__];
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

    if (bms_init(&p_self->bms[BMS_LV], CAN_ID_BMSLV1, "BMS LV" , log))
    {
        return -1;
    }

    if (bms_init(&p_self->bms[BMS_HV], CAN_ID_BMSHV1, "BMS HV" , log))
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
    int8_t err=0;

    for (uint8_t i=0; i<__NUM_OF_BMS__; i++) {
        err--;
        if (bms_update(&p_self->bms[i])<0)
        {
            return err;
        }
    }

    return 0;
}
