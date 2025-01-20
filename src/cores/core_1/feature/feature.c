#include "feature.h"

#include "cooling/colling.h"
#include "batteries/batteries.h"
#include "core_1_driver_input/core_1_driver_input.h"
#include "general_can/general_can.h"
#include "core_1_imu/core_1_imu.h"
#include "suspensions/suspensions.h"
#include "log/log.h"

#include <stdint.h>
#include <string.h>

struct Core1Feature_t{
    Core1Feature_h feature;
    Log_h log;
    GeneralCan_h can1;
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

int8_t
core_1_feature_init(Core1Feature_h* const restrict self __attribute__((__nonnull__)))
{
    union Core1Feature_h_t_conv conv = {self};
    struct Core1Feature_t* const restrict p_self = conv.clear;

    memset(p_self, 0, sizeof(*p_self));

    if(log_init(&p_self->log) <0) return -1;
    if(general_can_init(&p_self->can1) <0) return -1;
    if(cooling_init(&p_self->cooling, &p_self->can1, &p_self->log) <0) return -1;
    if(car_batteries_init(&p_self->batteries, &p_self->log) <0) return -1;
    if(core_1_driver_input_init(&p_self->core_1_driver_input, &p_self->log) <0) return -1;
    if(core_1_imu_init(&p_self->core_1_imu, &p_self->log)<0) return -1;
    if(suspensions_init(&p_self->suspensions, &p_self->log)<0) return -1;

    return 0;
}

int8_t
core_1_feature_update(Core1Feature_h* const restrict self __attribute__((__nonnull__)))
{
    union Core1Feature_h_t_conv conv = {self};
    struct Core1Feature_t* const restrict p_self = conv.clear;

    if(cooling_update_all(&p_self->cooling)<0) return -1;
    if(car_batteries_update(&p_self->batteries)) return -1;
    if(core_1_driver_input_update(&p_self->core_1_driver_input)) return -1;
    if(core_1_imu_update(&p_self->core_1_imu)) return -1;
    if(suspensions_update(&p_self->suspensions)) return -1;


    return log_update_and_send(&p_self->log);
}
