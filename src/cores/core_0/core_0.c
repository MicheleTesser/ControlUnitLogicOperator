#include "core_0.h"
#include "../core_status/core_status.h"
#include "driver_input/driver_input.h"
#include "giei/giei.h"
#include "giei/giei_components/engines/engine_common.h"
#include "mission/mission.h"
#include <stdint.h>

//public

void main_0(void)
{
    //setup
    Giei_h giei;
    DriverInput_h driver;
    Mission_h mission;

    while (driver_input_init(&driver) <0) {}
    while (mission_init(&mission, &driver)<0) {}
    while (giei_init(&giei, &driver, &mission) <0) {}

    //core sync
    core_status_core_ready(CORE_0);
    while (core_status_ready_state()) {}

    //loop
    for(;;){
        mission_update(&mission);
        driver_input_update(&driver);
        if (GIEI_check_running_condition(&giei) == RUNNING) {
            GIEI_compute_power(&giei);
        }
    }
}
