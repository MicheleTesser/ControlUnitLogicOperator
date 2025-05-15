#include "core_1.h"
#include "../core_utility/core_utility.h"
#include "feature/feature.h"
#include "feature/log/log.h"
#include <stdint.h>


void main_1(void)
{
  //setup
  CoreAliveBlink_h alive_blink;
  Core1Feature_h feature;
  Log_h log;
  SharedMessageOwner_h shared_message_owner;

  while (shared_message_owner_init(&shared_message_owner) < 0)
  {
    serial_write_str("shared message owner init failed");
  }

  while (core_alive_blink_init(&alive_blink, GPIO_CORE_1_ALIVE_BLINK, get_tick_from_millis(400)) <0)
  {
    serial_write_str("init core alive_blink core 1 failed");
  }

  while (log_init(&log)<0)
  {
    serial_write_str("init log failed");
  }

  while (core_1_feature_init(&feature, &log) <0)
  {
    serial_write_str("core 1 feature init failed");
  }

  //cores sync
  core_status_core_ready(CORE_1);
  while (!core_status_ready_state())
  {
      serial_write_str("core 1 waiting other cores");
  }

  // while (external_log_variables_add_to_log(&log)<0);

  //loop
  for(;;){
    core_alive_blink_update(&alive_blink);
    core_1_feature_update(&feature);
    log_update_and_send(&log);
  }
}
