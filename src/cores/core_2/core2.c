#include "./core2.h"
#include "../../board_conf/id_conf.h"
#include "../../lib/raceup_board/raceup_board.h"
#include "../../alive_blink/alive_blink.h"

static alive_blink_fd alive_fd =0;

#define MILLIS
#define MICROS

static void setup(void)
{
    hardware_init_gpio(LED_3);
    alive_fd = i_m_alive_init(timer_time_now(), 300 MILLIS * 1000 MICROS, LED_3);
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
