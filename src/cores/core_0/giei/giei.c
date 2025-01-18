#include "giei.h"
#include "../../../lib/raceup_board/raceup_board.h"
#include "../../../emergency_module/emergency_module.h"
#include "../../../driver_input/driver_input.h"
#include "../IO_id_0/IO_id_0.h"
#include "engines/engine_common.h"
#include <stdint.h>


struct Giei_t{
    EngineType inverter;
    time_var_microseconds rtd_sound_start;
    enum RUNNING_STATUS running_status;
    struct DriverInput_h driver_input;
    struct EmergencyNode* giei_emergency;
    uint8_t entered_rtd : 1;
};

union Giei_conv{
    struct Giei_h* hidden;
    struct Giei_t* clear;
};

#define GIEI_H_T_CONV(h_ptr, t_ptr_name)\
    union Giei_conv __g_conv_##t_ptr_name##__ = {h_ptr};\
    struct Giei_t* t_ptr_name = __g_conv_##t_ptr_name##__.clear;

#ifdef DEBUG
const uint8_t giei_size_check[(sizeof(struct Giei_h) == sizeof(struct Giei_t))? 1 : -1];
#endif /* ifdef DEBUG */

int8_t giei_init(struct Giei_h* const restrict self)
{
    GIEI_H_T_CONV(self, p_self);
    core0_driver_input_init(&p_self->driver_input);
    inverter_module_init(&p_self->inverter, &p_self->driver_input);
    p_self->giei_emergency = EmergencyNode_init(1);

    return 0;
}
enum RUNNING_STATUS GIEI_check_running_condition(struct Giei_h* const restrict self)
{
    GIEI_H_T_CONV(self, p_self);
    enum RUNNING_STATUS rt = SYSTEM_OFF;

    if ((timer_time_now() - p_self->rtd_sound_start) > 3 SECONDS)
    {
        gpio_set_high(GPIO_RTD_SOUND);
        gpio_set_high(GPIO_RTD_BUTTON);
    }
    rt = engine_rtd_procedure(&p_self->inverter);
    if (rt == RUNNING && !p_self->entered_rtd)
    {
        p_self->entered_rtd =1;
        gpio_set_low(GPIO_RTD_SOUND);
        gpio_set_low(GPIO_RTD_BUTTON);
        p_self->rtd_sound_start = timer_time_now();
    }
    else if (rt != RUNNING)
    {
        p_self->entered_rtd =0;
        gpio_set_high(GPIO_RTD_SOUND);
        gpio_set_high(GPIO_RTD_BUTTON);
    }
    p_self->running_status = rt;
    return rt;

}

