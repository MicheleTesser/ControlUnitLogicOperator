#include "score_lib/test_lib.h"
#include "src/driver_input/driver_input.h"
#include "lib/board_dbc/can2.h"
#include "./src/board_conf/id_conf.h"
#include "linux_board/linux_board.h"
#include "ControlUnitLogicOperator.h"

#include <string.h>
#include <stdint.h>
#include <sys/cdefs.h>
#include <threads.h>
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

static int test_throttle(void){
    int8_t err=0;
    uint8_t throttle_value = 40;
    can_obj_can2_h_t mex;
    CanMessage mex_c;
    memset(&mex, 0, sizeof(mex));
    memset(&mex_c, 0, sizeof(mex_c));
    mex.can_0x053_Driver.no_implausibility =1;
    mex.can_0x053_Driver.throttle = throttle_value;
    mex_c.message_size = pack_message_can2(&mex, CAN_ID_DRIVER, &mex_c.full_word);
    mex_c.id = CAN_ID_DRIVER;
    raise_interrupt(INTERRUPT_CAN_2);
    hardware_write_can(CAN_MODULE_GENERAL, &mex_c);
    sleep(1);
    uint8_t try = 0;
    while(driver_get_amount(THROTTLE) != throttle_value && try < 5) {}

    if (driver_get_amount(THROTTLE) == throttle_value) {
        PASSED("throttle value setted correctly");
    }else{
        FAILED("throttle value set fail");
        err--;
    }

    throttle_value = 67;
    memset(&mex, 0, sizeof(mex));
    memset(&mex_c, 0, sizeof(mex_c));
    mex.can_0x053_Driver.no_implausibility =1;
    mex.can_0x053_Driver.throttle = throttle_value;
    mex_c.message_size = pack_message_can2(&mex, CAN_ID_DRIVER, &mex_c.full_word);
    mex_c.id = CAN_ID_DRIVER;
    raise_interrupt(INTERRUPT_CAN_2);
    hardware_write_can(CAN_MODULE_GENERAL, &mex_c);
    sleep(1);

    if (driver_get_amount(THROTTLE) == throttle_value) {
        PASSED("throttle value upadte correctly");
    }else{
        FAILED("brake value update fail");
        err--;
    }

    return err;
}

static int test_brake(void){
    int8_t err=0;
    uint8_t brk_value = 42;
    can_obj_can2_h_t mex;
    CanMessage mex_c;
    memset(&mex, 0, sizeof(mex));
    memset(&mex_c, 0, sizeof(mex_c));
    mex.can_0x053_Driver.no_implausibility =1;
    mex.can_0x053_Driver.brake = brk_value;
    mex_c.message_size = pack_message_can2(&mex, CAN_ID_DRIVER, &mex_c.full_word);
    mex_c.id = CAN_ID_DRIVER;
    raise_interrupt(INTERRUPT_CAN_2);
    hardware_write_can(CAN_MODULE_GENERAL, &mex_c);
    sleep(1);

    if (driver_get_amount(BRAKE) == brk_value) {
        PASSED("brake value setted correctly");
    }else{
        FAILED("brake value set fail");
        err--;
    }

    brk_value = 30;
    memset(&mex, 0, sizeof(mex));
    memset(&mex_c, 0, sizeof(mex_c));
    mex.can_0x053_Driver.no_implausibility =1;
    mex.can_0x053_Driver.brake = brk_value;
    mex_c.message_size = pack_message_can2(&mex, CAN_ID_DRIVER, &mex_c.full_word);
    mex_c.id = CAN_ID_DRIVER;
    raise_interrupt(INTERRUPT_CAN_2);
    hardware_write_can(CAN_MODULE_GENERAL, &mex_c);
    sleep(1);

    if (driver_get_amount(BRAKE) == brk_value) {
        PASSED("brake value upadte correctly");
    }else{
        FAILED("brake value update fail");
        err--;
    }

    return err;
}

static int test_regen(void){
    int8_t err=0;
    uint8_t regen_value = 10;
    can_obj_can2_h_t mex;
    CanMessage mex_c;
    memset(&mex, 0, sizeof(mex));
    memset(&mex_c, 0, sizeof(mex_c));
    mex.can_0x052_Paddle.regen = regen_value;
    mex_c.message_size = pack_message_can2(&mex, CAN_ID_PADDLE, &mex_c.full_word);
    mex_c.id = CAN_ID_PADDLE;
    raise_interrupt(INTERRUPT_CAN_2);
    hardware_write_can(CAN_MODULE_GENERAL, &mex_c);
    sleep(1);

    if (driver_get_amount(REGEN) == regen_value) {
        PASSED("regen value setted correctly");
    }else{
        FAILED("regen value set fail");
        err--;
    }

    regen_value = 88;
    memset(&mex, 0, sizeof(mex));
    memset(&mex_c, 0, sizeof(mex_c));
    mex.can_0x052_Paddle.regen = regen_value;
    mex_c.message_size = pack_message_can2(&mex, CAN_ID_PADDLE, &mex_c.full_word);
    mex_c.id = CAN_ID_PADDLE;
    raise_interrupt(INTERRUPT_CAN_2);
    hardware_write_can(CAN_MODULE_GENERAL, &mex_c);
    sleep(1);

    if (driver_get_amount(REGEN) == regen_value) {
        PASSED("regen value upadte correctly");
    }else{
        FAILED("brake value update fail");
        err--;
    }

    return err;
}

static int test_impls(void){
    int8_t err=0;
    uint8_t no_imp = 0;
    can_obj_can2_h_t mex;
    CanMessage mex_c;
    memset(&mex, 0, sizeof(mex));
    memset(&mex_c, 0, sizeof(mex_c));
    mex.can_0x053_Driver.no_implausibility = no_imp;
    mex.can_0x053_Driver.bre_implausibility = 1;
    mex_c.message_size = pack_message_can2(&mex, CAN_ID_DRIVER, &mex_c.full_word);
    mex_c.id = CAN_ID_DRIVER;
    raise_interrupt(INTERRUPT_CAN_2);
    hardware_write_can(CAN_MODULE_GENERAL, &mex_c);
    sleep(1);

    if (check_impls(THROTTLE_BRAKE)) {
        PASSED("THROTTLE_BRAKE implausibility setted correctly");
    }else{
        FAILED("THROTTLE_BRAKE implausibility set failed");
        err--;
    }
    clear_implausibility();
    memset(&mex, 0, sizeof(mex));
    memset(&mex_c, 0, sizeof(mex_c));
    mex.can_0x053_Driver.pad_implausibility =1;
    mex_c.message_size = pack_message_can2(&mex, CAN_ID_DRIVER, &mex_c.full_word);
    mex_c.id = CAN_ID_DRIVER;
    raise_interrupt(INTERRUPT_CAN_2);
    hardware_write_can(CAN_MODULE_GENERAL, &mex_c);
    sleep(1);

    if (check_impls(THROTTLE_PADEL)) {
        PASSED("THROTTLE_PADEL implausibility setted correctly");
    }else{
        FAILED("THROTTLE_PADEL implausibility set failed");
        err--;
    }

    clear_implausibility();
    memset(&mex, 0, sizeof(mex));
    memset(&mex_c, 0, sizeof(mex_c));
    mex.can_0x053_Driver.pot_implausibility=1;
    mex_c.message_size = pack_message_can2(&mex, CAN_ID_DRIVER, &mex_c.full_word);
    mex_c.id = CAN_ID_DRIVER;
    raise_interrupt(INTERRUPT_CAN_2);
    hardware_write_can(CAN_MODULE_GENERAL, &mex_c);
    sleep(1);

    if (check_impls(THROTTLE_POT)) {
        PASSED("THROTTLE_POT implausibility setted correctly");
    }else{
        FAILED("THROTTLE_POT implausibility set failed");
        err--;
    }

    return err;
}

static int test_steering_wheel(void){
    int8_t err=0;
    uint8_t steering_value = 10;
    can_obj_can2_h_t mex;
    CanMessage mex_c;
    memset(&mex, 0, sizeof(mex));
    memset(&mex_c, 0, sizeof(mex_c));
    mex.can_0x053_Driver.steering = steering_value;
    mex_c.message_size = pack_message_can2(&mex, CAN_ID_DRIVER, &mex_c.full_word);
    mex_c.id = CAN_ID_DRIVER;
    raise_interrupt(INTERRUPT_CAN_2);
    hardware_write_can(CAN_MODULE_GENERAL, &mex_c);
    sleep(1);

    if (driver_get_amount(STEERING_ANGLE) == steering_value) {
        PASSED("steering value setted correctly");
    }else{
        FAILED("steering value set fail");
        err--;
    }

    steering_value = 88;
    memset(&mex, 0, sizeof(mex));
    memset(&mex_c, 0, sizeof(mex_c));
    mex.can_0x053_Driver.steering = steering_value;
    mex_c.message_size = pack_message_can2(&mex, CAN_ID_DRIVER, &mex_c.full_word);
    mex_c.id = CAN_ID_DRIVER;
    raise_interrupt(INTERRUPT_CAN_2);
    hardware_write_can(CAN_MODULE_GENERAL, &mex_c);
    sleep(1);

    if (driver_get_amount(STEERING_ANGLE) == steering_value) {
        PASSED("steering value upadte correctly");
    }else{
        FAILED("brake value update fail");
        err--;
    }

    return err;
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

    test_throttle();
    test_brake();
    test_regen();
    test_steering_wheel();
    test_impls();

end:
    print_SCORE();
    return err;
}
