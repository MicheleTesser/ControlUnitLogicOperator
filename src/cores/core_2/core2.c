#include "./core2.h"
#include "../../board_conf/id_conf.h"
#include "../../lib/raceup_board/raceup_board.h"
#include "../../lib/DPS/dps_slave.h"
#include "../alive_blink/alive_blink.h"
#include "../../log/log.h"
#include "../../DV/dv.h"
#include "../../missions/missons.h"

static alive_blink_fd alive_fd =0;

static void setup(void)
{
    mission_class_init();
    log_system_init();
    hardware_init_gpio(CORE_ALIVE_LED_3);
    alive_fd = i_m_alive_init(300 MILLIS, CORE_ALIVE_LED_3);
    dv_class_init();
    while (dps_is_init_done()) {}
}

static void loop(void)
{
    i_m_alive(alive_fd);
    dv_update_status();
    dv_update_led();
}

//INFO: Main logic operator core
void main_2(void)
{
    setup();

    for(;;){
        loop();
    }
}
