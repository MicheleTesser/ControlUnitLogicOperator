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


    PASSED("basic initialization worked");
    wait_milliseconds(1000);
    int pin= open("./gpio_0", O_RDONLY, 666);
    if (pin>= 0) {
        PASSED("gpios created succesfully");
    }else{
        FAILED("gpios not created");
    }

    int v='1';
    while (v == '0') read(pin, &v, sizeof(v));
    PASSED("toggle one ok");
    while (v == '1') read(pin, &v, sizeof(v));
    PASSED("toggle two ok");
    
    print_SCORE();
    return 0;
}
