#include "score_lib/test_lib.h"
#include "ControlUnitLogicOperator.h"
#include "raceup_board/raceup_board.h"
#include <fcntl.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <unistd.h>


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
    thrd_t core_0;
    thrd_t core_1;
    thrd_t core_2;
    thrd_create(&core_0, init_core_0, NULL);
    thrd_create(&core_1, init_core_1, NULL);
    thrd_create(&core_2, init_core_2, NULL);


    wait_milliseconds(1000);
    PASSED("basic initialization worked");
    if (!access("./gpio_0", F_OK)) {
        PASSED("gpios created succesfully");
    }else{
        FAILED("gpios not created");
    }

    print_SCORE();
    return 0;
}
