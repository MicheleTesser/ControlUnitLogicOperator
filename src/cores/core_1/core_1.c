#include "core_1.h"
#include "../core_utility/core_utility.h"
#include "feature/feature.h"
#include "feature/log/external_log_variables/external_log_variables.h"
#include "feature/log/log.h"
#include <stdint.h>


void main_1(void)
{
    //setup
    CoreAliveBlink_h alive_blink;
    Core1Feature_h feature;
    Log_h log;

    serial_write_str(0, "start init core 1");
    
    while (log_init(&log)<0);
    while (hardware_init_can(CAN_GENERAL, _500_KBYTE_S_) <0);
    while (core_alive_blink_init(&alive_blink, GPIO_CORE_1_ALIVE_BLINK, 300 MILLIS) <0);
    while (core_1_feature_init(&feature, &log) <0);

    serial_write_str(0, "core 1 wait sync cores");
    //cores sync
    core_status_core_ready(CORE_1);
    while (!core_status_ready_state());

    while (external_log_variables_add_to_log(&log)<0);

    serial_write_str(0, "core 1 init done");
    //loop
    for(;;){
        core_alive_blink_update(&alive_blink);
        core_1_feature_update(&feature);
    }
}
