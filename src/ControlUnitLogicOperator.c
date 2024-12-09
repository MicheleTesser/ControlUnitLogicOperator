#include "./ControlUnitLogicOperator.h"
#include <stdint.h>

#include "./lib/raceup_board/raceup_board.h"

void main_0(void){
    hardware_init_timer(0);
    hardware_init_gpio(0);
    while(1){
        gpio_set_low(0);
        wait_milliseconds(0, 1000);
    }
}

void main_1(void){
    for(;;){}
}

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
