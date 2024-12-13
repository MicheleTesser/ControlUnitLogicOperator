#include "./core2.h"
#include "../../board_conf/id_conf.h"
#include "../../lib/raceup_board/raceup_board.h"
#include "../../alive_blink/alive_blink.h"
#include "../../log/log.h"

static alive_blink_fd alive_fd =0;

static void setup(void)
{
    sd_log_init();
    telemetry_init();
    hardware_init_gpio(LED_3);
    alive_fd = i_m_alive_init(300 MILLIS, LED_3);
}

static void loop(void)
{
    i_m_alive(alive_fd);
}

//INFO: Main logic operator core
void main_2(void)
{
    setup();

    for(;;){
        loop();
    }
}
