#include "score_lib/test_lib.h"
#include "linux_board/linux_board.h"
#include "src/GIEI/amk/amk.h"
#include "src/GIEI/engine_common.h"
#include "src/driver_input/driver_input.h"
#include "src/emergency_fault/emergency_fault.h"
#include "src/missions/missons.h"
#include "src/board_conf/id_conf.h"
#include "lib/board_dbc/dbc/out_lib/can1/can1.h"
#include <stdio.h>
#include <sys/cdefs.h>
#include <sys/types.h>
#include <threads.h>
#include <stdint.h>
#include <unistd.h>

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


static void update_status_engine(uint8_t engine, const uint8_t attributes[5])
{
    can_obj_can1_h_t o = {0};
    CanMessage mex = {0};
    switch (engine) {
        case FRONT_LEFT:
            mex.id = CAN_ID_INVERTERFL1;
            o.can_0x283_InverterFL1.SystemReady = attributes[0];
            o.can_0x283_InverterFL1.HVOn = attributes[1];
            o.can_0x283_InverterFL1.HVOnAck = attributes[2];
            o.can_0x283_InverterFL1.InverterOn = attributes[3];
            o.can_0x283_InverterFL1.InverterOnAck = attributes[4];
            break;
        case FRONT_RIGHT:
            mex.id = CAN_ID_INVERTERFR1;
            o.can_0x284_InverterFR1.SystemReady = attributes[0];
            o.can_0x284_InverterFR1.HVOn = attributes[1];
            o.can_0x284_InverterFR1.HVOnAck = attributes[2];
            o.can_0x284_InverterFR1.InverterOn = attributes[3];
            o.can_0x284_InverterFR1.InverterOnAck = attributes[4];
            break;
        case REAR_LEFT:
            mex.id = CAN_ID_INVERTERRL1;
            o.can_0x287_InverterRL1.SystemReady = attributes[0];
            o.can_0x287_InverterRL1.HVOn = attributes[1];
            o.can_0x287_InverterRL1.HVOnAck = attributes[2];
            o.can_0x287_InverterRL1.InverterOn = attributes[3];
            o.can_0x287_InverterRL1.InverterOnAck = attributes[4];
            break;
        case REAR_RIGHT:
            mex.id = CAN_ID_INVERTERRR1;
            o.can_0x288_InverterRR1.SystemReady = attributes[0];
            o.can_0x288_InverterRR1.HVOn = attributes[1];
            o.can_0x288_InverterRR1.HVOnAck = attributes[2];
            o.can_0x288_InverterRR1.InverterOn = attributes[3];
            o.can_0x288_InverterRR1.InverterOnAck = attributes[4];
            break;
        default:
            return;
    }
    mex.message_size = pack_message_can1(&o, mex.id, &mex.full_word);
    amk_update_status(&mex);
}

static void reset(void)
{
    uint8_t attr[5] = {0};
    gpio_set_high(READY_TO_DRIVE_INPUT_BUTTON);
    gpio_set_high(AIR_PRECHARGE_INIT);
    gpio_set_high(AIR_PRECHARGE_DONE);
    gpio_set_high(READY_TO_DRIVE_INPUT_BUTTON);
    update_status_engine(FRONT_LEFT, attr);
    update_status_engine(FRONT_RIGHT, attr);
    update_status_engine(REAR_LEFT, attr);
    update_status_engine(REAR_RIGHT, attr);
    update_current_mission(NONE);
    if (amk_rtd_procedure() != SYSTEM_OFF) {
        FAILED("reset amk module failed");
    }
}

static void test_rtd_correct_in_manual_mode(void)
{
    enum RUNNING_STATUS rtd_status = SYSTEM_OFF;

    if(update_current_mission(MANUALY)){
        FAILED("failed updating mission to manually");
    }

    uint8_t engine_attr[5] = {0};
    engine_attr[0] = 1;
    engine_attr[1] = 1;
    engine_attr[2] = 1;

    FOR_EACH_ENGINE({
        update_status_engine(index_engine,engine_attr);
    })

    rtd_status = amk_rtd_procedure();
    if (rtd_status == SYSTEM_PRECAHRGE) {
        PASSED("rtd goes from SYSTEM OFF to SYSTEM_PRECHARGE correctly");
    }else{
        FAILED("rtd does not go from SYSTEM OFF to SYSTEM_PRECHARGE. Now in:");
        printf("%d\n",rtd_status);
    }

    gpio_set_low(AIR_PRECHARGE_INIT);

    FOR_EACH_ENGINE({
        update_status_engine(index_engine,engine_attr);
    })

    rtd_status = amk_rtd_procedure();
    if (rtd_status == SYSTEM_PRECAHRGE) {
        PASSED("air 1 open and still in SYSTEM_PRECAHRGE");
    }else{
        FAILED("air 1 open and not in SYSTEM_PRECAHRGE. Now in:");
        printf("%d\n",rtd_status);
    }

    gpio_set_low(AIR_PRECHARGE_DONE);
    FOR_EACH_ENGINE({
        update_status_engine(index_engine,engine_attr);
    })
    engine_attr[3] = 1;
    engine_attr[4] = 1;


    rtd_status = amk_rtd_procedure();
    if (rtd_status == TS_READY) {
        PASSED("rtd goes from SYSTEM_PRECHARGE to TS_READY correctly");
    }else{
        FAILED("rtd does not go from SYSTEM_PRECHARGE to TS_READY. Now in:");
        printf("%d\n",rtd_status);
    }

    driver_set_amount(BRAKE, 20, 0);
    gpio_set_low(READY_TO_DRIVE_INPUT_BUTTON);

    rtd_status = amk_rtd_procedure();
    if (rtd_status == RUNNING) {
        PASSED("rtd goes from from TS_READY to RUNNING correctly");
    }else{
        FAILED("rtd does not go from TS_READY to RUNNING. Now in:");
        printf("%d\n",rtd_status);
    }

    gpio_set_high(READY_TO_DRIVE_INPUT_BUTTON);
    rtd_status = amk_rtd_procedure();
    if (rtd_status == TS_READY) {
        PASSED("rtd goes from from RUNNING to TS_READY correctly");
    }else{
        FAILED("rtd does not go from RUNNING to TS_READY. Now in:");
        printf("%d\n",rtd_status);
    }
}


static void test_rtd_failed_in_manual_mode(void)
{
    enum RUNNING_STATUS rtd_status __attribute_maybe_unused__ = SYSTEM_OFF;

    if(update_current_mission(MANUALY)){
        FAILED("failed updating mission to manually");
    }

    uint8_t engine_attr[5] = {0};
    engine_attr[0] = 1;

    FOR_EACH_ENGINE({
        update_status_engine(index_engine,engine_attr);
    })

    rtd_status = amk_rtd_procedure();
    if (rtd_status == SYSTEM_OFF && !is_emergency_state()) {
        PASSED("rtd still SYSTEM_OFF with no emergency, waiting ack from inverter for HV");
    }else{
        FAILED("rtd still SYSTEM_OFF with no emergency, waiting ack from inverter for HV");
        printf("%d\n",rtd_status);
    }

    engine_attr[1] = 1;
    engine_attr[2] = 1;

    FOR_EACH_ENGINE({
        update_status_engine(index_engine,engine_attr);
    })

    const enum RUNNING_STATUS rtd_old = amk_rtd_procedure();

    engine_attr[1] = 0;
    engine_attr[2] = 0;
    FOR_EACH_ENGINE({
        update_status_engine(index_engine,engine_attr);
    })

    rtd_status = amk_rtd_procedure();
    if (rtd_old == SYSTEM_PRECAHRGE && rtd_status == SYSTEM_OFF && is_emergency_state()) {
        PASSED("recognized emergency in PRECHARGE because HV was OFF");
    }else{
        PASSED("not recognized emergency in PRECHARGE when HV was OFF");
    }

}

int main(void)
{
    if(create_virtual_chip() <0){
        FAILED("virtual chip init failed");
        goto end;
    }

    if (amk_module_init() < 0) {
        FAILED("init module amk failed");
        goto end;
    }

    reset();
    test_initial_running_status();
    reset();
    test_initial_inverter_status();
    reset();
    test_initial_emergency_state();

    reset();
    test_rtd_correct_in_manual_mode();
    reset();
    test_rtd_failed_in_manual_mode();

end:
    print_SCORE();
    return 0;
}
