#include "score_lib/test_lib.h"
#include "linux_board/linux_board.h"
#include "ControlUnitLogicOperator.h"
#include "src/GIEI/amk/amk.h"
#include "src/GIEI/engine_common.h"
#include "src/emergency_fault/emergency_fault.h"
#include <stdio.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <stdint.h>
#include <unistd.h>

static uint8_t engine_status[4] __attribute_maybe_unused__;
#define SPEED_LIMIT                         18000           //INFO: Typical value: 15000

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

static void test_initial_running_status(void)
{
    const enum RUNNING_STATUS running_status = amk_rtd_procedure();
    if (running_status == SYSTEM_OFF) {
        PASSED("amk initial state is SYSTEM OFF");
    }else{
        FAILED("amk initial state is not SYSTEM OFF: ");
        printf("%d\n", running_status);
    }
}

static void test_initial_inverter_status(void)
{
    const uint8_t running_status = amk_inverter_hv_status();
    if (!running_status) {
        PASSED("amk initial inverter state is OFF");
    }else{
        FAILED("amk initial inverter state is ON");
    }
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

    sleep(1);

    test_initial_running_status();
    test_initial_inverter_status();
    
    if(is_emergency_state()){
        FAILED("default state is emergency state");
    }else{
        PASSED("default state is not in emergency");
    }


end:
    print_SCORE();
    return err;
}
