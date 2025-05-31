#include "feature.h"

#include "cooling/colling.h"
#include "batteries/batteries.h"
#include "core_1_driver_input/core_1_driver_input.h"
#include "core_1_imu/core_1_imu.h"
#include "log/log.h"
#include "suspensions/suspensions.h"
#include "../../core_utility/core_utility.h"

#include <stdint.h>
#include <string.h>

struct Core1Feature_t{
    Log_h m_log;
    Cooling_h m_cooling;
    CarBatteries_h m_batteries;
    Core1DriverInput_h m_core_1_driver_input;
    Core1Imu_h m_core_1_imu;
    Suspensions_h m_suspensions;
    SytemSettingOwner_h m_setting_update;
};

union Core1Feature_h_t_conv{
    Core1Feature_h* const restrict hidden;
    struct Core1Feature_t* const restrict clear;
};

#ifdef DEBUG
char __assert_size_core_1_feature[(sizeof(Core1Feature_h) >= sizeof(struct Core1Feature_t))? 1:-1];
char __assert_align_core_1_feature[(_Alignof(Core1Feature_h) == _Alignof(struct Core1Feature_t))? 1:-1];
#endif // DEBUG

int8_t core_1_feature_init(Core1Feature_h* const restrict self)
{
    union Core1Feature_h_t_conv conv = {self};
    struct Core1Feature_t* const restrict p_self = conv.clear;

    memset(p_self, 0, sizeof(*p_self));

    while(system_settings_init(&p_self->m_setting_update)<0);
    while(log_init(&p_self->m_log)<0);
    while(cooling_init(&p_self->m_cooling, &p_self->m_log) <0);
    while(car_batteries_init(&p_self->m_batteries, &p_self->m_log) <0);
    while(core_1_driver_input_init(&p_self->m_core_1_driver_input, &p_self->m_log) <0);
    while(core_1_imu_init(&p_self->m_core_1_imu, &p_self->m_log)<0);
    while(suspensions_init(&p_self->m_suspensions, &p_self->m_log)<0);

    return 0;
}

#define TRACE_ERR()\
{\
  SET_TRACE(CORE_1);\
  err--;\
}

int8_t core_1_feature_update(Core1Feature_h* const restrict self )
{
    union Core1Feature_h_t_conv conv = {self};
    struct Core1Feature_t* const restrict p_self = conv.clear;
    int8_t err=0;

    if(car_batteries_update(&p_self->m_batteries)) TRACE_ERR();
    if(core_1_driver_input_update(&p_self->m_core_1_driver_input)) TRACE_ERR();
    if(core_1_imu_update(&p_self->m_core_1_imu)) TRACE_ERR();
    if(suspensions_update(&p_self->m_suspensions)) TRACE_ERR();
    if(cooling_update(&p_self->m_cooling)<0)TRACE_ERR();
    if(log_update_and_send(&p_self->m_log)<0)TRACE_ERR();
    if(system_settings_update(&p_self->m_setting_update)<0)TRACE_ERR();

    return err;
}
