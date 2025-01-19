#include "core_1.h"
#include "../core_status/core_status.h"
#include "batteries/batteries.h"
#include "core_1_driver_input/core_1_driver_input.h"
#include "general_can/general_can.h"
#include "cooling/colling.h"
#include "core_1_imu/core_1_imu.h"
#include "suspensions/suspensions.h"
#include "log/log.h"
#include <stdint.h>


void main_1(void)
{
    //setup
    Log_h log;
    GeneralCan_h can1;
    Cooling_h cooling;
    CarBatteries_h batteries;
    Core1DriverInput_h core_1_driver_input;
    Core1Imu_h core_1_imu;
    Suspensions_h suspensions;
    

    while (log_init(&log) <0){}
    while (general_can_init(&can1) <0) {}
    while (cooling_init(&cooling, &can1, &log) <0) {}
    while (car_batteries_init(&batteries, &log) <0) {}
    while (core_1_driver_input_init(&core_1_driver_input, &log) <0) {}
    while (core_1_imu_init(&core_1_imu, &log)<0) {}
    while (suspensions_init(&suspensions, &log)<0) {}


    core_status_core_ready(CORE_1);
    while (core_status_ready_state()) {}

    //loop
    for(;;){
        cooling_update_all(&cooling);
        car_batteries_update(&batteries);
        core_1_driver_input_update(&core_1_driver_input);
        core_1_imu_update(&core_1_imu);
        suspensions_update(&suspensions);


        log_update_and_send(&log);
    }
}
