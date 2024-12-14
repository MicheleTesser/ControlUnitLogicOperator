#include "score_lib/test_lib.h"
#include "linux_board/can/can_lib/canlib.h"
#include "linux_board/linux_board.h"
#include "ControlUnitLogicOperator.h"
#include <stdio.h>
#include <sys/cdefs.h>
#include <threads.h>

#define CAN_INTERFACE_0 "culo_can_0"
#define CAN_INTERFACE_1 "culo_can_1"
#define CAN_INTERFACE_2 "culo_can_2"

int can_0_fd = -1;
int can_1_fd = -1;
int can_2_fd = -1;

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

void init_can(void){
    can_0_fd = can_init(CAN_INTERFACE_0);
    can_1_fd = can_init(CAN_INTERFACE_1);
    can_2_fd = can_init(CAN_INTERFACE_2);
}

int main(void)
{
    if(create_virtual_chip() <0){
        return -1;
    }

    init_can();

    thrd_t core_0;
    thrd_t core_1;
    thrd_t core_2;
    thrd_create(&core_0, init_core_0, NULL);
    thrd_create(&core_1, init_core_1, NULL);
    thrd_create(&core_2, init_core_2, NULL);

    if (can_0_fd < 0 || can_1_fd < 0 || can_2_fd < 0) {
        perror("test can init failed\n");
    }

    print_SCORE();
    return 0;
}
