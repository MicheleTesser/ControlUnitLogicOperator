#include "./ControlUnitLogicOperator.h"
#include <stdint.h>
#include <stddef.h>

#include "./lib/raceup_board/raceup_board.h"
#include "./board_conf/id_conf.h"

//INFO: Main logic operator core
void main_0(void){
    hardware_init_gpio(LED_1);
    hardware_init_can(CAN_MODULE_INVERTER, 500000);
    hardware_init_serial(SERIAL);
    for(;;){
        gpio_toggle(LED_1);
        wait_milliseconds(500);
    }
}

//INFO: Service core
void main_1(void){
    hardware_init_can(CAN_MODULE_INVERTER, 500000);
    hardware_init_can(CAN_MODULE_GENERAL, 500000);
    hardware_init_can(CAN_MODULE_DV, 500000);
    for(;;){
        hardware_read_can(CAN_MODULE_INVERTER, NULL);
        hardware_read_can(CAN_MODULE_GENERAL, NULL);
        hardware_read_can(CAN_MODULE_DV, NULL);
    }
}

//INFO: Dv core
void main_2(void){
    for(;;){}
}


void main_cpu_x(uint8_t cpu_num)
{
    switch (cpu_num) {
        case 0:
            main_0();
            break;
        case 1:
            main_1();
            break;
        case 2:
            main_2();
            break;
        default:
            for (;;) {}
    }
}
