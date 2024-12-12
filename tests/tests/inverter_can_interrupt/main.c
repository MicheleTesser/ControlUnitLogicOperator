#include "score_lib/test_lib.h"
#include "linux_board/linux_board.h"
#include "./linux_board/can/can_lib/canlib.h"
#include "ControlUnitLogicOperator.h"
#include <stdio.h>
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

int socket_0=-1;
int socket_1=-1;
int socket_2=-1;

static void init_can_test(void){
    socket_0 = can_init("culo_can_0");
    if(socket_0  < 0){
        perror("init can 0 failed");
    }

    socket_1 = can_init("culo_can_1");
    if( socket_1 <0){
        perror("init can 1 failed");
    }

    socket_2 = can_init("culo_can_2");
    if( socket_2 < 0){
        perror("init can 2 failed");
    }
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

    init_can_test();

    sleep(1);

    struct can_frame frame = {
        .can_id = 1,
        .len = 1,
        .data[0] = 1,
    };
    can_send_frame(socket_0, &frame);
    raise_interrupt(0);

    can_send_frame(socket_1, &frame);
    raise_interrupt(1);

    can_send_frame(socket_2, &frame);
    raise_interrupt(2);

    print_SCORE();
    return 0;
}
