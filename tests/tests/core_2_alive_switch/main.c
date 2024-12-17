#include "score_lib/test_lib.h"
#include "./src/board_conf/id_conf.h"
#include "linux_board/linux_board.h"
#include "ControlUnitLogicOperator.h"
#include <sys/cdefs.h>
#include <threads.h>


static int init_core_0(void* args __attribute_maybe_unused__){
    main_cpu_x(0);
    return 0;
}

static int init_core_1(void* args __attribute_maybe_unused__){
    main_cpu_x(1);
    return 0;
}

static int init_core_2(void* args __attribute_maybe_unused__){
    main_cpu_x(2);
    return 0;
}


int main(void)
{
    int8_t err=0;
    if(create_virtual_chip() <0){
        err--;
        goto end;
    }

    thrd_t core_0;
    thrd_t core_1;
    thrd_t core_2;
    thrd_create(&core_0, init_core_0, NULL);
    thrd_create(&core_1, init_core_1, NULL);
    thrd_create(&core_2, init_core_2, NULL);


    uint8_t gpio_val = gpio_read_state(CORE_ALIVE_LED_3);
    uint8_t new_val = gpio_val;

    while (new_val == gpio_val) {
        new_val = gpio_read_state(CORE_ALIVE_LED_3);
    }
    gpio_val = new_val =gpio_read_state(CORE_ALIVE_LED_3);
    PASSED("core2 alive gpio switched 1");

    while (new_val == gpio_val) {
        new_val = gpio_read_state(CORE_ALIVE_LED_3);
    }
    PASSED("core2 alive gpio switched 2");

end:
    print_SCORE();
    return err;
}
