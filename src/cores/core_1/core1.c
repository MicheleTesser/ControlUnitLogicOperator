#include "./core1.h"
#include <stddef.h>

#include "../../board_conf/id_conf.h"
#include "../../lib/raceup_board/raceup_board.h"
#include "../../alive_blink/alive_blink.h"

static alive_blink_fd alive_fd;

static void setup(void)
{
    hardware_init_can(CAN_MODULE_INVERTER, 500000);
    hardware_init_can(CAN_MODULE_GENERAL, 500000);
    hardware_init_can(CAN_MODULE_DV, 500000);
    i_m_alive_init(100, LED_2);
}

static void loop(void)
{
    i_m_alive(alive_fd);
}

//INFO: Service core
void main_1(void)
{
    setup();
    for(;;){
        loop();
    }
}
