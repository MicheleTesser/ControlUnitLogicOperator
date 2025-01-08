#include "score_lib/test_lib.h"
#include "linux_board/linux_board.h"
#include "src/board_can/can_freq_check/can_freq_check.h"
#include <stdio.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <stdint.h>

const uint16_t DUMMY_CAN_ID[3] ={16,17,18};

#define DUMMY_FUN_FOR(ID) \
static int8_t dummy_fault_##ID(void)\
{\
    printf("deadline expired for %d at %ld\n",ID,timer_time_now());\
    return 0;\
}

DUMMY_FUN_FOR(0)
DUMMY_FUN_FOR(1)
DUMMY_FUN_FOR(2)

int main(void)
{
    int8_t err=0;
    if (can_freq_class_init(10) < 0) {
        FAILED("init can freq failed");
        goto end;
    }else{
        PASSED("init can freq ok");
    }

    printf("adding sequence started at: %ld\n",timer_time_now());
    if (can_freq_add_mex_freq(DUMMY_CAN_ID[0], 3 SECONDS, dummy_fault_0) < 0){
        FAILED("adding first mex with its frequence failed");
    }else{
        PASSED("adding first mex with its frequence PASSED");
    }

    if (can_freq_add_mex_freq(DUMMY_CAN_ID[1], 5 SECONDS, dummy_fault_1) < 0){
        FAILED("adding second mex with its frequence failed");
    }else{
        PASSED("adding second mex with its frequence passed");
    }

    if (can_freq_add_mex_freq(DUMMY_CAN_ID[2], 7 SECONDS, dummy_fault_2) < 0){
        FAILED("adding third mex with its frequence failed");
    }else{
        PASSED("adding third mex with its frequence passed");
    }

    uint16_t id_fault= can_freq_check_faults();
    if (id_fault > 0) {
        FAILED("failed recognized fault too early with id: ");
        printf("%d\n", id_fault);
    }

    wait_milliseconds(3 SECONDS);

    id_fault= can_freq_check_faults();
    if (id_fault > 0) {
        if (id_fault == DUMMY_CAN_ID[0]) {
            PASSED("recognized fault with id: ");
        }else{
            FAILED("not recognized fault early with id: ");
        }
        printf("%d\n", id_fault);
    }else{
        FAILED("error not triggered");
    }
    id_fault = can_freq_check_faults();
    if (id_fault) {
        FAILED("recognized error of other messages too soon: ");
        printf("%d\n", id_fault);
    }else{
        PASSED("check deadline of other messages ok");
    }

end:
    print_SCORE();
    return err;
}
