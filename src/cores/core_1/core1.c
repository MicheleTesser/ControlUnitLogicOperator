#include "./core1.h"
#include <stddef.h>

#include "../../board_conf/id_conf.h"
#include "../../lib/raceup_board/raceup_board.h"

//INFO: Service core
void main_1(void)
{
    hardware_init_can(CAN_MODULE_INVERTER, 500000);
    hardware_init_can(CAN_MODULE_GENERAL, 500000);
    hardware_init_can(CAN_MODULE_DV, 500000);
    for(;;){
        // hardware_read_can(CAN_MODULE_INVERTER, NULL);
        // hardware_read_can(CAN_MODULE_GENERAL, NULL);
        // hardware_read_can(CAN_MODULE_DV, NULL);
    }
}
