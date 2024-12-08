#include "./ControlUnitLogicOperator.h"
#include <stdint.h>

#define MAX_TIMERS 1
#define MAX_SERIALS 1
#define MAX_GPIOS 1
#include "./lib/raceup_board/raceup_board.h"

#define LED         &MODULE_P00,5                                           /* LED: Port, Pin definition            */
#define WAIT_TIME   500

#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "Bsp.h"

void main_0(void){
    init_new_timer_component(&virtual_board.timers[0],0);
    init_new_gpio_component(&virtual_board.gpios[0], 0);
    while(1){
             gpio_toggle(&virtual_board.gpios[0]);
             wait_milliseconds(&virtual_board.timers[0], 500);
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
