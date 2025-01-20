#include "dv.h"
#include "../../../core_utility/core_utility.h"
#include "../../../../lib/raceup_board/components/gpio.h"
#include "../mission/mission.h"
#include "res/res.h"
#include "asb/asb.h"
#include "../dv_driver_input/dv_driver_input.h"
#include "../mission/mission.h"
#include "./steering_wheel_alg/stw_alg.h"
#include <stdint.h>
#include <string.h>

//private

enum AS_STATUS{
    AS_OFF =1,
    AS_READY =2,
    AS_DRIVING =3,
    AS_EMERGENCY =4,
    AS_FINISHED =5,
};

struct Dv_t{
    enum AS_STATUS status;
    uint8_t sound_start;
    time_var_microseconds driving_last_time_on;
    time_var_microseconds emergency_last_time_on;
    time_var_microseconds emergency_sound_last_time_on;
    DvRes_h dv_res;
    DvAsb_h dv_asb;
    DvMission_h* dv_mission;
    DvDriverInput_h* dv_driver_input;
    struct EmergencyNode* emergency_node;
};

enum DV_EMERGENCY{
    EMERENGENCY_DV_EMERGENCY =0,

    __NUM_OF_DV_EMERGENCIES__
};

union Dv_h_t_conv{
    struct Dv_h* const restrict hidden;
    struct Dv_t* const restrict clear;
};

//INFO: check dbc of can3 in message DV_system_status

static uint8_t sdc_closed(const struct Dv_t* const restrict self)
{
    return  gpio_read_state(GPIO_AIR_PRECHARGE_INIT) &&
            gpio_read_state(GPIO_AIR_PRECHARGE_DONE) &&
            EmergencyNode_is_emergency_state(self->emergency_node);
}

static void update_dv_status(struct Dv_t* const restrict self, const enum AS_STATUS status)
{
    if (status == AS_EMERGENCY)
    {
        EmergencyNode_raise(self->emergency_node, EMERENGENCY_DV_EMERGENCY);
    }
    else
    {
        EmergencyNode_solve(self->emergency_node, EMERENGENCY_DV_EMERGENCY);
    }
    self->status = status;
}

static int8_t dv_update_led(struct Dv_t* const restrict self)
{
    if ((timer_time_now() - self->emergency_sound_last_time_on) > 8 SECONDS) {
        gpio_set_high(GPIO_AS_EMERGENCY_SOUND);
        self->emergency_sound_last_time_on = timer_time_now();
    }

    switch (self->status) {
        case AS_OFF:
            gpio_set_high(GPIO_ASSI_LIGHT_BLU);
            gpio_set_high(GPIO_ASSI_LIGHT_YELLOW);
            gpio_set_high(GPIO_AS_EMERGENCY_SOUND);
            break;
        case AS_READY:
            gpio_set_high(GPIO_ASSI_LIGHT_BLU);
            gpio_set_low(GPIO_ASSI_LIGHT_YELLOW);
            gpio_set_high(GPIO_AS_EMERGENCY_SOUND);
            res_start_time(&self->dv_res);
            break;
        case AS_DRIVING:
            gpio_set_high(GPIO_ASSI_LIGHT_BLU);
            gpio_set_high(GPIO_AS_EMERGENCY_SOUND);
            if ((timer_time_now() - self->driving_last_time_on) > 100 MILLIS ) {
                gpio_toggle(GPIO_ASSI_LIGHT_YELLOW);
                self->driving_last_time_on = timer_time_now();
            }
            break;
        case AS_EMERGENCY:
            gpio_set_high(GPIO_ASSI_LIGHT_YELLOW);
            if ((timer_time_now() - self->emergency_last_time_on) > 100 MILLIS ) {
                gpio_toggle(GPIO_ASSI_LIGHT_BLU);
                self->emergency_last_time_on = timer_time_now();
            }
            if (!self->sound_start) {
                self->sound_start =1;
                gpio_set_low(GPIO_AS_EMERGENCY_SOUND);
                self->emergency_sound_last_time_on = timer_time_now();
            }
            break;
        case AS_FINISHED:
            gpio_set_high(GPIO_AS_EMERGENCY_SOUND);
            gpio_set_high(GPIO_ASSI_LIGHT_YELLOW);
            gpio_set_low(GPIO_ASSI_LIGHT_BLU);
            break;
    }


    return 0;
}

//INFO: Flowchart T 14.9.2
static int8_t dv_update_status(struct Dv_t* const restrict self)
{
    float giei_speed = 0; //TODO: how to get it
    float driver_brake = dv_driver_input_get_brake(self->dv_driver_input);
    const enum RUNNING_STATUS giei_status; //TODO: how to get it

    if (EmergencyNode_is_emergency_state(self->emergency_node)) {
        self->status = AS_EMERGENCY;
        EmergencyNode_raise(self->emergency_node, EMERENGENCY_DV_EMERGENCY);
    }

    if (!ebs_on(&self->dv_asb.dv_ebs)) 
    {
        if (dv_mission_get_current(self->dv_mission) > MANUALY &&
                asb_consistency_check(&self->dv_asb)
                && giei_status >= TS_READY)
        {
            if (giei_status == RUNNING)
            {
                update_dv_status(self, AS_DRIVING);
            }
            else if(driver_brake > 50)
            {
                update_dv_status(self, AS_READY);
            }
            else
            {
                update_dv_status(self, AS_OFF);
            }
        }
        else
        {
            update_dv_status(self, AS_OFF);
        }
    }

    
    else if (dv_mission_get_status(self->dv_mission) == MISSION_FINISHED &&
            !giei_speed
            && sdc_closed(self))
    {
        update_dv_status(self, AS_FINISHED);
    }
    else
    {
        update_dv_status(self, AS_EMERGENCY);
    }
    return 0;
}

//public

int8_t
dv_class_init(Dv_h* const restrict self __attribute__((__nonnull__)),
        DvMission_h* const restrict mission __attribute__((__nonnull__)),
        DvDriverInput_h* const restrict driver __attribute__((__nonnull__)))
{
    union Dv_h_t_conv conv = {self};
    struct Dv_t* const p_self = conv.clear;
    memset(p_self, 0, sizeof(*p_self));

    dv_stw_alg_init();

    if(res_class_init(&p_self->dv_res))
    {
        return -1;
    }
    if(asb_class_init(&p_self->dv_asb) <0)
    {
        return -1;
    }
    p_self->dv_mission = mission;
    p_self->dv_driver_input = driver;
    p_self->emergency_node = EmergencyNode_new(__NUM_OF_DV_EMERGENCIES__); //TODO: not yet defined
    if (!p_self->emergency_node) {
        return -1;
    }

    return 0;
}

int8_t dv_update(Dv_h* const restrict self __attribute__((__nonnull__)))
{
    union Dv_h_t_conv conv = {self};
    struct Dv_t* const p_self = conv.clear;
    dv_update_status(p_self);
    dv_update_led(p_self);
    if (p_self->status == AS_DRIVING && dv_mission_get_current(p_self->dv_mission) > MANUALY)
    {
        dv_stw_alg_compute(0, 0); //TODO: not yet implemented
        return 0;
    }
    return -1;
}
