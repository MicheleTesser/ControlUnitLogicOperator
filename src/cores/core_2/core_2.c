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
    union SystemSettingValue_t setting_value;

    while (core_alive_blink_init(&alive_blink, GPIO_CORE_2_ALIVE_BLINK, get_tick_from_millis(600)) <0)
    {
      serial_write_str("alive blink init core 2 failed");
    }

    while (core_2_feature_init(&feature)<0)
    {
      serial_write_str("core 2 init feature failed");
    }

    //cores sync
    core_status_core_ready(CORE_2);
    while (!core_status_ready_state());

    // serial_write_str("core 2 init done");
    //loop
    for(;;){
        core_alive_blink_update(&alive_blink);
        core_2_feature_update(&feature);

      if(!system_settings_get(CORE_2_SERIAL_TRACE, &setting_value) && setting_value.u8)
      {
        errno_trace_print(CORE_2);
        errno_trace_clear(CORE_2);
      }

    }
}
