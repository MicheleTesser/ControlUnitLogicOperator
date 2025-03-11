#include "core_0.h"
#include "../core_utility/core_utility.h"
#include "feature/feature.h"

//public

void main_0(void)
{
    //setup
    CoreAliveBlink_h alive_blink;
    Core0Feature_h feature;
    
    serial_write_str(0, "start init core 0");
    while (EmergencyNode_class_init()<0)
    {
      serial_write_str(0, "trying init EmergencyNode class");
    }
    while(hardware_init_can(CAN_INVERTER, _1_MBYTE_S_)<0)
    {
      serial_write_str(0, "trying init can inverter");
    }
    while (core_alive_blink_init(&alive_blink, GPIO_CORE_0_ALIVE_BLINK, 200 MILLIS) <0)
    {
      serial_write_str(0, "trying init core alive_blink core 0");
    }
    while (core_0_feature_init(&feature)<0)
    {
      serial_write_str(0, "trying init core 0 feature");
    }

    serial_write_str(0, "core 0 wait sync cores");
    //cores sync
    core_status_core_ready(CORE_0);
    while (!core_status_ready_state());

    serial_write_str(0, "core 0 init done");
    //loop
    for(;;){
        core_alive_blink_update(&alive_blink);
        core_0_feature_update(&feature);
        core_0_feature_compute_power(&feature);
    }
}
