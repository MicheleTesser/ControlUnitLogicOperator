#include "core_2.h"
#include "../core_utility/core_utility.h"
#include "feature/feature.h"
#include <stdint.h>

//public

void main_2(void)
{
    //setup
    CoreAliveBlink_h alive_blink;
    Core2Feature_h feature;

    while (hardware_init_can(CAN_DV, _500_KBYTE_S_)<0);
    while (core_alive_blink_init(&alive_blink, GPIO_CORE_2_ALIVE_BLINK, 1 SECONDS) <0);
    while (core_2_feature_init(&feature)<0);

    //cores sync
    core_status_core_ready(CORE_2);
    while (!core_status_ready_state());

    //loop
    for(;;){
        core_alive_blink_update(&alive_blink);
        core_2_feature_update(&feature);
    }
}
