#include "score_lib/test_lib.h"
#include "ControlUnitLogicOperator.h"
#include "./linux_board/gpio/gpio.h"
#include "./src/cores/board_conf/id_conf.h"
#include <stdint.h>
#include <stdio.h>
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
    if(create_virtual_chip() <0){
        return -1;
    }

    thrd_t core_0;
    thrd_t core_1;
    thrd_t core_2;
    thrd_create(&core_0, init_core_0, NULL);
    thrd_create(&core_1, init_core_1, NULL);
    thrd_create(&core_2, init_core_2, NULL);

    uint8_t gpio_val = gpio_read_state(LED_1);
    uint8_t new_val = gpio_val;

    while (new_val == gpio_val) {
        new_val = gpio_read_state(LED_1);
    }
    gpio_val = new_val =gpio_read_state(LED_1);
    PASSED("gpio switched 1");

    while (new_val == gpio_val) {
        new_val = gpio_read_state(LED_1);
    }
    PASSED("gpio switched 2");

    print_SCORE();


    return 0;
}
