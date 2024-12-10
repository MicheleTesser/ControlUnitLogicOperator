#include "./ControlUnitLogicOperator.h"
#include <stdint.h>
#include <stddef.h>

#include "./cores/core_0/core0.h"
#include "./cores/core_1/core1.h"
#include "./cores/core_2/core2.h"

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
