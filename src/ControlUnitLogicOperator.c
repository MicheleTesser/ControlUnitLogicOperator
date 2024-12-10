#include <stdint.h>
#include "./ControlUnitLogicOperator.h"

#include "./cores/cores.h"

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
