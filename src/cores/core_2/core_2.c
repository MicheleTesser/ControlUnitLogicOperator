#include "core_2.h"
#include "../core_utility/core_utility.h"
#include "feature/feature.h"
#include <stdint.h>

//public

void main_2(void)
{
    //setup
    CoreAliveBlink_h alive_blink;
    DvDriverInput_h driver;
    DvMission_h mission;
    Dv_h dv;


    while (core_alive_blink_init(&alive_blink, CORE_2_ALIVE_BLINK) <0) {}
    while(dv_driver_input_init(&driver)<0){}
    while(dv_mission_init(&mission)<0){}
    while (dv_class_init(&dv, &mission, &driver) <0) {}

    core_status_core_ready(CORE_2);
    while (core_status_ready_state()) {}

    //loop
    for(;;){
        core_alive_blink_update(&alive_blink);
        dv_mission_update(&mission);
        dv_driver_input_update(&driver);
        dv_update(&dv);
    }
}
