#include "./core0.h"
#include "../../board_conf/id_conf.h"
#include "../../lib/raceup_board/raceup_board.h"

//INFO: Main logic operator core
void main_0(void)
{
    hardware_init_gpio(LED_1);
    hardware_init_serial(SERIAL);
    for(;;){
        gpio_toggle(LED_1);
        wait_milliseconds(500);
    }
}
