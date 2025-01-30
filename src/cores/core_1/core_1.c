#include "core_1.h"
#include "../core_utility/core_utility.h"
#include "feature/feature.h"
#include <stdint.h>


void main_1(void)
{
    //setup
    CoreAliveBlink_h alive_blink;
    Core1Feature_h feature;
    
    while (hardware_init_trap()<0);
    while (hardware_init_can(CAN_GENERAL, _500_KBYTE_S_) <0);
    while (core_alive_blink_init(&alive_blink, GPIO_CORE_1_ALIVE_BLINK, 300 MILLIS) <0);
    while (core_1_feature_init(&feature) <0);

    //cores sync
    core_status_core_ready(CORE_1);
    while (!core_status_ready_state());

    //loop
    for(;;){
        core_alive_blink_update(&alive_blink);
        core_1_feature_update(&feature);
    }
}
