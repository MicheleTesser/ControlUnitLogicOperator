#include "score_lib/test_lib.h"
#include "linux_board/linux_board.h"
#include "src/board_conf/id_conf.h"
#include "src/lib/board_dbc/dbc/out_lib/can2/can2.h"
#include "ControlUnitLogicOperator.h"
#include <stdio.h>
#include <string.h>
#include <sys/cdefs.h>
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

static int wait_fan_enable(void* args __attribute_maybe_unused__)
{
    CanMessage mex = {0};
    while (mex.id != CAN_ID_PCU) {
        memset(&mex, 0, sizeof(mex));
        hardware_read_can(CAN_MODULE_GENERAL, &mex);
    }
    can_obj_can2_h_t o;
    unpack_message_can2(&o, mex.id, mex.full_word, mex.message_size, 0);
    if (o.can_0x130_Pcu.fan_enable) {
        PASSED("fan enabled correctly\n");
    }else{
        FAILED("fan not enabled\n");
    }
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
    thrd_t enable_fan;
    thrd_create(&core_0, init_core_0, NULL);
    thrd_create(&core_1, init_core_1, NULL);
    thrd_create(&core_2, init_core_2, NULL);
    thrd_create(&enable_fan, wait_fan_enable, NULL);

    sleep(1);
    printf("test setup done\n");

    thrd_join(enable_fan, NULL);

end:
    print_SCORE();
    return err;
}
