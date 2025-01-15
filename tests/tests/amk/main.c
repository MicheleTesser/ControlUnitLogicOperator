#include "score_lib/test_lib.h"
#include "src/cores/core_status/core_status.h"
#include "linux_board/linux_board.h"
#include "ControlUnitLogicOperator.h"
#include "src/GIEI/amk/amk.h"
#include "src/GIEI/engine_common.h"
#include "src/emergency_fault/emergency_fault.h"
#include "src/missions/missons.h"
#include "src/board_conf/id_conf.h"
#include "car_component/car_component.h"
#include <stdio.h>
#include <string.h>
#include <sys/cdefs.h>
#include <sys/types.h>
#include <threads.h>
#include <stdint.h>
#include <unistd.h>

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

static void test_initial_emergency_state(void)
{
    if(is_emergency_state()){
        FAILED("default state is emergency state");
    }else{
        PASSED("default state is not in emergency");
    }
}

static void test_rtd_in_manual_mode(void)
{
    enum RUNNING_STATUS rtd_status = SYSTEM_OFF;

    if(update_current_mission(MANUALY)){
        FAILED("failed updating mission to manually");
    }

    car_amk_inverter_set_attribute(SYSTEM_READY, REAR_RIGHT, 1);
    car_amk_inverter_set_attribute(SYSTEM_READY, REAR_LEFT, 1);
    car_amk_inverter_set_attribute(SYSTEM_READY, FRONT_RIGHT, 1);
    car_amk_inverter_set_attribute(SYSTEM_READY, FRONT_LEFT, 1);

    time_var_microseconds t = timer_time_now();
    while ((timer_time_now() - t) <= 5 SECONDS) {}

    rtd_status = amk_rtd_procedure();
    if (rtd_status == SYSTEM_PRECAHRGE) {
        PASSED("rtd goes from SYSTEM OFF to PRECHARGE PROCEDURE correctly");
    }else{
        FAILED("rtd does not go from SYSTEM OFF to PRECHARGE PROCEDURE. Now in:");
        printf("%d\n",rtd_status);
    }

}

int main(void)
{
    int8_t err=0;
    if(create_virtual_chip() <0){
        FAILED("virtual chip init failed");
        err--;
        goto end;
    }

    car_amk_inverter_class_init();

    thrd_t core_0;
    thrd_t core_1;
    thrd_t core_2;
    thrd_create(&core_0, init_core_0, NULL);
    thrd_create(&core_1, init_core_1, NULL);
    thrd_create(&core_2, init_core_2, NULL);

    while (
            core_status(0) != CORE_READY || 
            core_status(1) != CORE_READY || 
            core_status(2) != CORE_READY) {}

    test_initial_running_status();
    test_initial_inverter_status();
    test_initial_emergency_state();
    
    test_rtd_in_manual_mode();

end:
    print_SCORE();
    return err;
}
