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
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#define CAN_INTERFACE_0 "culo_can_0"
#define CAN_INTERFACE_1 "culo_can_1"
#define CAN_INTERFACE_2 "culo_can_2"

int can_0_fd = -1;
int can_1_fd = -1;
int can_2_fd = -1;

static void block_signal_usr1_usr2(void)
{
    sigset_t main_set;

    sigemptyset(&main_set);
    sigaddset(&main_set, SIGUSR1);
    sigaddset(&main_set, SIGUSR2);

    pthread_sigmask(SIG_BLOCK, &main_set, NULL);

}

static void* init_core_0(void* args __attribute_maybe_unused__){
    block_signal_usr1_usr2();
    main_cpu_x(0);
    return NULL;
}

static void* init_core_1(void* args __attribute_maybe_unused__){
    block_signal_usr1_usr2();
    main_cpu_x(1);
    return NULL;
}

static void* init_core_2(void* args __attribute_maybe_unused__){
    block_signal_usr1_usr2();
    main_cpu_x(2);
    return NULL;
}

static void* pilot_breaking(void* args __attribute_maybe_unused__){
    block_signal_usr1_usr2();
    can_obj_can2_h_t mex;
    CanMessage mex_c;
    memset(&mex, 0, sizeof(mex));
    memset(&mex_c, 0, sizeof(mex_c));
    mex.can_0x053_Driver.no_implausibility =1;
    mex.can_0x053_Driver.brake = 40;
    mex_c.message_size = pack_message_can2(&mex, CAN_ID_DRIVER, &mex_c.full_word);
    mex_c.id = CAN_ID_DRIVER;
    printf("sending mex driver. ID: %d, data: %ld\n",mex_c.id,mex_c.full_word);
    for(;;){
        raise_interrupt(INTERRUPT_CAN_2);
        hardware_write_can(CAN_MODULE_GENERAL, &mex_c);
    }
    // for(;;){
    // }
    return NULL;
}

static void init_can(void){
    can_0_fd = can_init(CAN_INTERFACE_0);
    can_1_fd = can_init(CAN_INTERFACE_1);
    can_2_fd = can_init(CAN_INTERFACE_2);
}

static void rtd_sequence(void){
    block_signal_usr1_usr2();
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

static void* inverter_on(void* args __attribute_maybe_unused__){
    block_signal_usr1_usr2();
    can_obj_can1_h_t m;
    CanMessage can_m;
    memset(&m, 0, sizeof(m));
    m.can_0x283_InverterFL1.HVOn = 1;
    m.can_0x283_InverterFL1.HVOnAck = 1;
    m.can_0x283_InverterFL1.SystemReady = 1;
    can_m.id = CAN_ID_INVERTERFL1;
    can_m.message_size = pack_message_can1(&m, CAN_ID_INVERTERFL1, &can_m.full_word);
    printf("sending mex inverter. ID: %d, data: %ld\n",can_m.id,can_m.full_word);
    for(;;){
        raise_interrupt(INTERRUPT_CAN_1);
        hardware_write_can(CAN_MODULE_INVERTER, &can_m);
    }
    // for(;;){
    // }

    return NULL;
}

static void* interrupt_manager__can_0(void* args __attribute_maybe_unused__){
    block_signal_usr1_usr2();
    sigset_t set_2;
    sigemptyset(&set_2);
    sigaddset(&set_2, SIGUSR1);

    int signal;
    for(;;){
        sigwait(&set_2, &signal);
    
    }


    return NULL;
}

static void* interrupt_manager__can_1(void* args __attribute_maybe_unused__){
    block_signal_usr1_usr2();
    sigset_t set_2;
    sigemptyset(&set_2);
    sigaddset(&set_2, SIGUSR2);

    int signal;
    for(;;){
        sigwait(&set_2, &signal);
    }


    return NULL;
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

    pthread_t core_0;
    pthread_t core_1;
    pthread_t core_2;
    pthread_t pilot;
    pthread_t inverter;
    pthread_t interrupt_manager_1;
    pthread_t interrupt_manager_2;
    pthread_create(&interrupt_manager_1, NULL, interrupt_manager__can_0, NULL);
    pthread_create(&interrupt_manager_2, NULL, interrupt_manager__can_1, NULL);
    pthread_create(&core_0, NULL, init_core_0, NULL);
    pthread_create(&core_1, NULL, init_core_1, NULL);
    pthread_create(&core_2, NULL, init_core_2, NULL);
    pthread_create(&pilot, NULL, pilot_breaking, NULL);
    pthread_create(&inverter, NULL, inverter_on, NULL);
    wait_milliseconds(10000);
    printf("rtd sequence start\n");
    // rtd_sequence();
    
    printf("rtd sequence done\n");
    pthread_join(inverter, NULL);
    pthread_join(pilot, NULL);

    pthread_join(interrupt_manager_1, NULL);
    pthread_join(interrupt_manager_2, NULL);
    pthread_join(core_0, NULL);
    pthread_join(core_1, NULL);
    pthread_join(core_2, NULL);

    print_SCORE();
    return 0;
}
