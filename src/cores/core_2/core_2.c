#include "core_2.h"
#include "../core_utility/core_utility.h"
#include <stdint.h>

//public

void main_2(void)
{
    //setup
    CoreAliveBlink_h alive_blink;

    while (core_alive_blink_init(&alive_blink, CORE_2_ALIVE_BLINK) <0) {}

    core_status_core_ready(CORE_2);
    while (core_status_ready_state()) {}

    //loop
    for(;;){
        core_alive_blink_update(&alive_blink);
    }
}
