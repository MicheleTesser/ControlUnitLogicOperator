#include "core_0.h"
#include "../core_utility/core_utility.h"
#include "feature/feature.h"

//public

void main_0(void)
{
    //setup
    CoreAliveBlink_h alive_blink;
    Core0Feature_h feature;
    
    while (core_alive_blink_init(&alive_blink, GPIO_CORE_0_ALIVE_BLINK, 200 MILLIS) <0);
    while (core_0_feature_init(&feature)<0);

    //cores sync
    core_status_core_ready(CORE_0);
    while (!core_status_ready_state());

    //loop
    for(;;){
        core_alive_blink_update(&alive_blink);
        core_0_feature_update(&feature);
        core_0_feature_compute_power(&feature);
    }
}
