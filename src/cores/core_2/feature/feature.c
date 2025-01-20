#include "feature.h"

#include "DV/dv.h"
#include "../../../lib/raceup_board/components/can.h"
#include "dv_driver_input/dv_driver_input.h"
#include "mission/mission.h"

#include <stdint.h>

struct Core2Feature_t{
    struct CanNode* can_2;
    DvDriverInput_h driver;
    DvMission_h mission;
    Dv_h dv;
};

union Core2Feature_h_t_conv{
    Core2Feature_h* const restrict hidden;
    struct Core2Feature_t* const restrict clear;
};

int8_t
core_2_feature_init(Core2Feature_h* const restrict self __attribute__((__nonnull__)))
{
    union Core2Feature_h_t_conv conv = {self};
    struct Core2Feature_t* const restrict p_self = conv.clear;

    p_self->can_2 = hardware_init_can(CAN_DV, _500_KBYTE_S_);
    if(!p_self->can_2) return -1;
    if(dv_driver_input_init(&p_self->driver)<0) return -1;
    if(dv_mission_init(&p_self->mission)<0) return -1;
    if(dv_class_init(&p_self->dv, &p_self->mission, &p_self->driver) <0) return -1;

    return 0;
}

int8_t
core_2_feature_update(Core2Feature_h* const restrict self __attribute__((__nonnull__)))
{
    union Core2Feature_h_t_conv conv = {self};
    struct Core2Feature_t* const restrict p_self = conv.clear;

    if(dv_mission_update(&p_self->mission) <0) return -1;
    if(dv_driver_input_update(&p_self->driver)) return -1;
    if(dv_update(&p_self->dv)) return -1;

    return 0;
}
