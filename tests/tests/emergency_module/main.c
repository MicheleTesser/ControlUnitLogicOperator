#include "score_lib/test_lib.h"
#include "linux_board/linux_board.h"
#include "src/emergency_fault/emergency_fault.h"
#include <stdio.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <stdint.h>
#include <unistd.h>


static int8_t putting_one_emergency_and_clear(const enum EMERGENCY_FAULT ftype){
    int8_t err=0;
    if (one_emergency_raised(ftype)) {
        goto err;
    }
    if (is_emergency_state()) {
        PASSED("emergency test set ok");
    }else{
        FAILED("emergency test set failed");
    }

    if (one_emergency_solved(ftype)) {
        goto err;
    }

    if (!is_emergency_state()) {
        PASSED("emergency state cleared");
    }else{
        FAILED("emergency state not cleared");
    }

    return 0;

err:
    err--;
    return err;

}

static int8_t putting_double_emergency_and_clear(void){
    int8_t err=0;
    if (one_emergency_raised(FAILED_RTD_SEQ)) {
        goto err;
    }
    if (one_emergency_raised(ENGINE_FAULT)) {
        goto err;
    }
    if (is_emergency_state()) {
        PASSED("emergency test set ok");
    }else{
        FAILED("emergency test set failed");
    }

    if (one_emergency_solved(FAILED_RTD_SEQ)) {
        goto err;
    }

    if (is_emergency_state()) {
        PASSED("emergency state remain only one cleared");
    }else{
        FAILED("emergency state not remain cleared all");
    }

    if (one_emergency_solved(ENGINE_FAULT)) {
        goto err;
    }

    if (!is_emergency_state()) {
        PASSED("emergency state cleared all");
    }else{
        FAILED("emergency state remain");
    }

    return 0;

err:
    err--;
    return err;
}

static int8_t clearing_already_clean(void){
    int8_t err=0;
    if (one_emergency_solved(FAILED_RTD_SEQ)) {
        goto err;
    }
    if (!is_emergency_state()) {
        PASSED("clear does not alter what is alredy clear");
    }else{
        PASSED("clear alter what is alredy clear");
    }

    return 0;

err:
    err--;
    return err;
}

int main(void)
{
    int8_t err=0;
    if(create_virtual_chip() <0){
        err--;
        goto end;
    }

    printf("testing set of RTD_SEQ: \n");
    if(putting_one_emergency_and_clear(FAILED_RTD_SEQ))
    {
        goto end;
    }
    printf("testing set of ENGINE_FAULT: \n");
    if(putting_one_emergency_and_clear(ENGINE_FAULT))
    {
        goto end;
    }

    printf("testing multiple emergency at once: \n");
    if (putting_double_emergency_and_clear()) {
        goto end;
    }

    printf("tesint solving on already clear:\n");
    if (clearing_already_clean()) {
        goto end;
    }


end:
    print_SCORE();
    return err;
}
