#include "./core2.h"
#include "../core_status/core_status.h"
#include "../../board_conf/id_conf.h"
#include "../../lib/raceup_board/raceup_board.h"
#include "../../lib/DPS/dps_slave.h"
#include "../alive_blink/alive_blink.h"
#include "../../log/log.h"
#include "../../DV/dv.h"
#include "../../missions/missons.h"
#include <stdint.h>

static alive_blink_fd alive_fd =0;

static void setup(void)
{

    while(mission_class_init()<0){
        serial_write_str(SERIAL, "mission class init failed");
    };
    while(log_system_init()<0){
        serial_write_str(SERIAL, "log init failed");
    };
    while(hardware_init_gpio(CORE_ALIVE_LED_3)<0){
        serial_write_str(SERIAL, "core 2 alive led gpio init failed");
    };
    alive_fd = i_m_alive_init(300 MILLIS, CORE_ALIVE_LED_3);
    while(dv_class_init()<0){
        serial_write_str(SERIAL, "dv init failed");
    };
    while (dps_is_init_done() <0) {
    };

    serial_write_str(SERIAL, "core 2 init done");
}

static void loop(void)
{
    i_m_alive(alive_fd);
    dv_compute();
}

//INFO: Main logic operator core
void main_2(void)
{
    core_update_status(2, CORE_INIT);
    setup();
    core_update_status(2, CORE_READY);
    while (core_status(0) != CORE_READY || 
            core_status(1) != CORE_READY ||
            core_status(2) != CORE_READY) {}

    for(;;){
        loop();
    }
}
