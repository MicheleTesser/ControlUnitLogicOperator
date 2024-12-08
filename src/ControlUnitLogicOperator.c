#include "./ControlUnitLogicOperator.h"
#include <stdint.h>

#define MAX_SERIALS 1
#define MAX_GPIOS 1
#include "./lib/raceup_board/raceup_board.h"

int8_t main_0(void){
    for(;;){}
}

int8_t main_1(void){
    for(;;){}
}

int8_t main_2(void){
    for(;;){}
}


int8_t main_cpu_x(uint8_t cpu_num)
{

    switch (cpu_num) {
        case 0:
            return main_0();
        case 1:
            return main_1();
        case 2:
            return main_2();
    
        default:
            for (;;) {}
    }

    while(1){
    }
    return 0;
}
