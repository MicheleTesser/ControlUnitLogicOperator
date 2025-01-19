#include "core_1.h"
#include "../core_status/core_status.h"
#include "general_can/general_can.h"
#include "cooling/colling.h"
#include <stdint.h>


void main_1(void)
{
    //setup
    GeneralCan_h can1;
    Cooling_h cooling;

    while (general_can_init(&can1) <0) {}
    while (cooling_init(&cooling, &can1) <0) {}

    core_status_core_ready(CORE_1);
    while (core_status_ready_state()) {}

    //loop
    for(;;){
        cooling_switch_update_all(&cooling);
    }
}
