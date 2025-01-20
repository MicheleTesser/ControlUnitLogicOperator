#include "core_1.h"
#include "../core_utility/core_utility.h"
#include "feature/feature.h"
#include <stdint.h>


void main_1(void)
{
    //setup
    CoreAliveBlink_h alive_blink;
    Core1Feature_h feature;
    
    while (core_alive_blink_init(&alive_blink, CORE_1_ALIVE_BLINK) <0) {}
    while (core_1_feature_init(&feature) <0) {}

    //setup completed
    core_status_core_ready(CORE_1);
    while (core_status_ready_state()) {}

    //loop
    for(;;){
        core_alive_blink_update(&alive_blink);
    }
}
