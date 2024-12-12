#include "./core0.h"
#include "../../board_conf/id_conf.h"
#include "../../lib/raceup_board/raceup_board.h"
#include "../../alive_blink/alive_blink.h"
#include <stdint.h>

//private

static alive_blink_fd alive_fd =0;

static void setup(void)
{
    hardware_init_interrupt();
    hardware_init_gpio(LED_1);
    hardware_init_gpio(SCS);
    hardware_init_serial(SERIAL);
    alive_fd = i_m_alive_init(300 MILLIS, LED_1);

    gpio_set_high(SCS); //INFO: open air for HV when the board start
}

static void loop(void)
{
    i_m_alive(alive_fd);
}

//public
//INFO: Main logic operator core
void main_0(void)
{
    setup();
    for (;;) {
        loop();
    }
}
