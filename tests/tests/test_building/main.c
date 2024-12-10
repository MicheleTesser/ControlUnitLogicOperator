#include "score_lib/test_lib.h"
#include "ControlUnitLogicOperator.h"
#include <sys/cdefs.h>
#include <threads.h>


int init_core_0(void* args __attribute_maybe_unused__){
    main_cpu_x(0);
    return 0;
}

int init_core_1(void* args __attribute_maybe_unused__){
    main_cpu_x(1);
    return 0;
}

int init_core_2(void* args __attribute_maybe_unused__){
    main_cpu_x(2);
    return 0;
}


int main(void)
{
    PASSED("build successful");

    print_SCORE();
    return 0;
}
