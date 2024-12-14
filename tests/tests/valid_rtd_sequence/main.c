#include "score_lib/test_lib.h"
#include "linux_board/can/can_lib/canlib.h"
#include "linux_board/linux_board.h"
#include "ControlUnitLogicOperator.h"
#include "lib/board_dbc/can1.h"
#include "lib/board_dbc/can2.h"
#include "./src/board_conf/id_conf.h"
#include <stdio.h>
#include <string.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <unistd.h>

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

static int pilot_breaking(void* args __attribute_maybe_unused__){
    can_obj_can2_h_t mex;
    CanMessage mex_c;
    memset(&mex, 0, sizeof(mex));
    memset(&mex_c, 0, sizeof(mex_c));
    mex.can_0x053_Driver.no_implausibility =1;
    mex.can_0x053_Driver.brake = 40;
    mex_c.message_size = pack_message_can2(&mex, CAN_ID_DRIVER, &mex_c.full_word);
    for(;;){
        if(hardware_write_can(CAN_MODULE_GENERAL, &mex_c) < 0){
            raise_interrupt(1);
            perror("failed sending message can\n");
        }
    }
    return 0;
}

static void init_can(void){
    can_0_fd = can_init(CAN_INTERFACE_0);
    can_1_fd = can_init(CAN_INTERFACE_1);
    can_2_fd = can_init(CAN_INTERFACE_2);
}

static void rtd_sequence(void){
    wait_milliseconds(1000);
    printf("rtd closing air 1\n");
    gpio_set_low(AIR_PRECHARGE_INIT);
    wait_milliseconds(5000);
    printf("rtd closing air 2\n");
    gpio_set_low(AIR_PRECHARGE_DONE);
    wait_milliseconds(5000);
    printf("drive button start\n");
    gpio_set_low(READY_TO_DRIVE_INPUT_BUTTON);
}

static int inverter_on(void* args __attribute_maybe_unused__){
    can_obj_can1_h_t m;
    CanMessage can_m;
    memset(&m, 0, sizeof(m));
    m.can_0x283_InverterFL1.HVOn = 1;
    m.can_0x283_InverterFL1.HVOnAck = 1;
    m.can_0x283_InverterFL1.SystemReady = 1;

    can_m.id = CAN_ID_INVERTERFL1;
    can_m.message_size = pack_message_can1(&m, CAN_ID_INVERTERFL1, &can_m.full_word);
    for(;;){
        if(hardware_write_can(CAN_MODULE_INVERTER, &can_m) <0){
            perror("error sending inverter data\n");
        }
        raise_interrupt(0);
    }

    return 0;
}

int main(void)
{
    if(create_virtual_chip() <0){
        return -1;
    }

    init_can();
    if (can_0_fd < 0 || can_1_fd < 0 || can_2_fd < 0) {
        perror("test can init failed\n");
    }

    thrd_t core_0;
    thrd_t core_1;
    thrd_t core_2;
    thrd_t pilot;
    thrd_t inverter;
    thrd_create(&core_0, init_core_0, NULL);
    thrd_create(&core_1, init_core_1, NULL);
    thrd_create(&core_2, init_core_2, NULL);
    thrd_create(&pilot, pilot_breaking, NULL);
    thrd_create(&inverter, inverter_on, NULL);
    wait_milliseconds(10000);
    printf("rtd sequence start\n");
    rtd_sequence();
    
    printf("rtd sequence done\n");
    for(;;);

    print_SCORE();
    return 0;
}
