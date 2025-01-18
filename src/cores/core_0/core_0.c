#include "core_0.h"
#include "giei/giei.h"
#include "../core_status/core_status.h"
#include <stdint.h>

//public

void main_0(void)
{
    //setup
    struct Giei_h giei;
    struct DriverInput_h driver;

    while (driver_input_init(&driver,,) <0) {}
    while (giei_init(&giei,&driver) <0) {}

    //core sync
    core_status_core_ready(CORE_0);
    while (core_status_ready_state()) {}

    //loop
    for(;;){
    }
}
