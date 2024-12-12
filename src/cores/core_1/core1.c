#include "./core1.h"
#include <stddef.h>
#include <stdint.h>

#include "../../board_conf/id_conf.h"
#include "../../lib/raceup_board/raceup_board.h"
#include "../../alive_blink/alive_blink.h"
#include "../../board_can/board_can.h"

static alive_blink_fd alive_fd;

static void setup(void)
{
    board_can_init(CAN_MODULE_INVERTER, 500000);
    board_can_init(CAN_MODULE_GENERAL, 500000);
    board_can_init(CAN_MODULE_DV, 500000);
    alive_fd = i_m_alive_init(100, LED_2);
}

static void loop(void)
{
    i_m_alive(alive_fd);
    {
        CanMessage mex;
        if(board_can_read(CAN_MODULE_INVERTER, &mex) >= 0){
            //TODO: manage inverter messages
        }
    }

    {
        CanMessage mex;
        if(board_can_read(CAN_MODULE_GENERAL, &mex) >= 0){
            //TODO: manage general messages
        }
    }

    {
        CanMessage mex;
        if(board_can_read(CAN_MODULE_DV, &mex) >= 0){
            //TODO: manage dv messages
        }
    }
}

//INFO: Service core
void main_1(void)
{
    setup();
    for(;;){
        loop();
    }
}
