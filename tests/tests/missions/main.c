#include "score_lib/test_lib.h"
#include "linux_board/linux_board.h"
#include "ControlUnitLogicOperator.h"
#include "src/missions/missons.h"
#include <stdio.h>
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

    enum MISSIONS m = get_current_mission();
    if (m!=NONE) {
        FAILED("default mission is not NONE :");
        fprintf(stderr, "%d\n",m);
    }else{
        PASSED("default mission ok");
    }

    enum MISSION_STATUS s = mission_status();
    if (s!=MISSION_NOT_RUNNING) {
        FAILED("default mission status is not MISSION_NOT_RUNNING:");
        fprintf(stderr, "%d\n",s);
    }else{
        PASSED("default mission status ok");
    }

    if (update_current_mission(MANUALY) <0) {
        FAILED("failed updating mission function");
    }else{
        PASSED("passed updating mission function");
    }

    m = get_current_mission();
    if (m!=MANUALY) {
        FAILED("updaed mission is not MANUALLY:");
        fprintf(stderr, "%d\n",s);
    }else{
        PASSED("updaed mission ok");
    }

    if (mission_lock_mission() < 0) {
        FAILED("mission lock function failed");
    }else{
        PASSED("mission lock function passed");
    }

    if (update_current_mission(DV_ACCELERATION) <0) {
        PASSED("preventing updating mission function becuase of lock passed");
    }else{
        FAILED("preventing updating mission function becuase of lock failed");
    }

    if (mission_unlock_mission()<0) {
        FAILED("mission unlock failed");
    }else{
        PASSED("mission unlock passed");
    }

    if (update_current_mission(DV_ACCELERATION) <0) {
        FAILED("updating mission function after unlock failed");
    }else{
        PASSED("updating mission function after unlock passed");
    }

end:
    print_SCORE();
    return err;
}
