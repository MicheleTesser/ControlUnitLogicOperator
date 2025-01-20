#include "core_0.h"
#include "../core_utility/core_utility.h"
#include "feature/feature.h"

//public

void main_0(void)
{
    //setup
    Giei_h giei;
    DriverInput_h driver;
    Mission_h mission;
    CoreAliveBlink_h alive_blink;

    while (driver_input_init(&driver) <0) {}
    while (mission_init(&mission, &driver)<0) {}
    while (giei_init(&giei, &driver, &mission) <0) {}
    while (core_alive_blink_init(&alive_blink, CORE_0_ALIVE_BLINK) <0) {}

    //core sync
    core_status_core_ready(CORE_0);
    while (core_status_ready_state()) {}

    //loop
    for(;;){
        core_alive_blink_update(&alive_blink);
        mission_update(&mission);
        driver_input_update(&driver);
        if (GIEI_check_running_condition(&giei) == RUNNING) {
            GIEI_compute_power(&giei);
        }
    }
}
