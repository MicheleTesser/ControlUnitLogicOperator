#include "feature.h"
#include "driver_input/driver_input.h"
#include "giei/giei.h"
#include "mission/mission.h"

#include <stdint.h>

struct Core0Feature_t{
    Giei_h giei;
    DriverInput_h driver;
    Mission_h mission;
};

union Core0Feature_h_t_conv{
    Core0Feature_h* const restrict hidden;
    struct Core0Feature_t* const restrict clear;
};

int8_t
core_2_feature_init(Core0Feature_h* const restrict self __attribute__((__nonnull__)))
{
    union Core0Feature_h_t_conv conv = {self};
    struct Core0Feature_t* const restrict p_self = conv.clear;

    if(driver_input_init(&p_self->driver) <0) return -1;
    if(mission_init(&p_self->mission, &p_self->driver)<0) return -1;
    if(giei_init(&p_self->giei, &p_self->driver, &p_self->mission) <0) return -1;

    return 0;
}

int8_t
core_2_feature_update(Core0Feature_h* const restrict self __attribute__((__nonnull__)))
{
    union Core0Feature_h_t_conv conv = {self};
    struct Core0Feature_t* const restrict p_self = conv.clear;
    enum RUNNING_STATUS status = SYSTEM_OFF;

    if(mission_update(&p_self->mission) <0)
    {
        return -1;
    }
    if(driver_input_update(&p_self->driver)<0)
    {
        return -2;
    }
    status = GIEI_check_running_condition(&p_self->giei);
    if (status)
    {
        if(global_running_status_set(status)<0){
            return -3;
        }
        if(GIEI_compute_power(&p_self->giei)<0)
        {
            return -4;
        }
    }

    return 0;
}
