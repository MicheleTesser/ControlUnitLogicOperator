#include "feature.h"

#include "cooling/colling.h"
#include "batteries/batteries.h"
#include "core_1_driver_input/core_1_driver_input.h"
#include "core_1_imu/core_1_imu.h"
#include "suspensions/suspensions.h"
#include "log/log.h"

#include <stdint.h>
#include <string.h>

struct Core1Feature_t{
    Log_h log;
    Cooling_h cooling;
    CarBatteries_h batteries;
    Core1DriverInput_h core_1_driver_input;
    Core1Imu_h core_1_imu;
    Suspensions_h suspensions;
};

union Core1Feature_h_t_conv{
    Core1Feature_h* const restrict hidden;
    struct Core1Feature_t* const restrict clear;
};

#ifdef DEBUG
char __assert_size_core_1_feature[(sizeof(Core1Feature_h) == sizeof(struct Core1Feature_t))? 1:-1];
char __assert_align_core_1_feature[(_Alignof(Core1Feature_h) == _Alignof(struct Core1Feature_t))? 1:-1];
#endif // DEBUG

int8_t
core_1_feature_init(Core1Feature_h* const restrict self )
{
    union Core1Feature_h_t_conv conv = {self};
    struct Core1Feature_t* const restrict p_self = conv.clear;

    memset(p_self, 0, sizeof(*p_self));

    if(log_init(&p_self->log) <0) return -1;
    if(cooling_init(&p_self->cooling, &p_self->log) <0) return -2;
    if(car_batteries_init(&p_self->batteries, &p_self->log) <0) return -3;
    if(core_1_driver_input_init(&p_self->core_1_driver_input, &p_self->log) <0) return -4;
    if(core_1_imu_init(&p_self->core_1_imu, &p_self->log)<0) return -5;
    if(suspensions_init(&p_self->suspensions, &p_self->log)<0) return -6;

    return 0;
}

int8_t
core_1_feature_update(Core1Feature_h* const restrict self )
{
    union Core1Feature_h_t_conv conv = {self};
    struct Core1Feature_t* const restrict p_self = conv.clear;

    if(car_batteries_update(&p_self->batteries)) return -1;
    if(core_1_driver_input_update(&p_self->core_1_driver_input)) return -2;
    if(core_1_imu_update(&p_self->core_1_imu)) return -3;
    if(suspensions_update(&p_self->suspensions)) return -4;


    return log_update_and_send(&p_self->log);
}
