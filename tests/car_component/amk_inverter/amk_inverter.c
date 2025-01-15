#include "amk_inverter.h"
#include "../../linux_board/linux_board.h"
#include "../src/board_conf/id_conf.h"
#include "../src/GIEI/engine_common.h"
#include "../src/lib/board_dbc/dbc/out_lib/can1/can1.h"

#include <stdint.h>
#include <stdio.h>
#include <threads.h>
#include <unistd.h>
#include <fcntl.h>

struct AMK_Actual_Values_1{
    struct{
        uint8_t b_reserve; //INFO: Reserved
        uint8_t bSystemReady: 1; //INFO: System ready (SBM)
        uint8_t AMK_bError :1; //INFO: Error
        uint8_t AMK_bWarn :1; //INFO: Warning
        uint8_t AMK_bQuitDcOn :1; //INFO: HV activation acknowledgment
        uint8_t AMK_bDcOn :1; //INFO: HV activation level
        uint8_t AMK_bQuitInverterOn:1; //INFO: Controller enable acknowledgment
        uint8_t AMK_bInverterOn:1; //INFO: Controller enable level
        uint8_t AMK_bDerating :1; //INFO: Derating (torque limitation active)
    }AMK_STATUS;
    //INFO:  AMK_ActualVelocity : Unit = rpm; Actual speed value
    int16_t AMK_ActualVelocity; 
    //INFO: AMK_TorqueCurrent: Raw data for calculating 'actual torque current' Iq See 'Units' on page 65.               
    int16_t AMK_TorqueCurrent; 
    //INFO: Raw data for calculating 'actual magnetizing current' Id See 'Units' on page 1.
    int16_t AMK_MagnetizingCurrent;
};

struct AMK_Actual_Values_2{
    int16_t AMK_TempMotor; //INFO: Unit= 0.1 °C. Motor temperature
    int16_t AMK_TempInverter; //INFO: Unit= 0.1 °C. Cold plate temperature
    uint16_t AMK_ErrorInfo; //INFO: Diagnostic number
    int16_t AMK_TempIGBT; //INFO: 0.1 °C. IGBT temperature
};

struct AMK_Setpoints{
    //INFO: AMK_Control: 
    //Control word See the table below: Content of the 'AMK_Control' control word
    struct {
        uint8_t AMK_bReserve_start;
        uint8_t AMK_bInverterOn:1;
        uint8_t AMK_bDcOn:1;
        uint8_t AMK_bEnable:1;
        uint8_t AMK_bErrorReset:1;
        uint8_t AMK_bReserve_end :4;
    }AMK_Control_fields;
    int16_t AMK_TargetVelocity; //INFO: unit: rpm Speed setpoint
    int16_t AMK_TorqueLimitPositive; //INFO: unit: 0.1% MN Positive torque limit (subject to nominal torque)
    int16_t AMK_TorqueLimitNegative; //INFO: unit: 0.1% MN Negative torque limit (subject to nominal torque)
};
static struct{
    struct amk_engines{
        struct AMK_Actual_Values_1 amk_data_1;
        struct AMK_Actual_Values_2 amk_data_2;
    }engines[4];
    int pipefd;
    int pipe_interrupt;
}INVERTER;

#define POPULATE_MEX_ENGINE(mex, engine)\
{\
    struct AMK_Actual_Values_1* s_w = &INVERTER.engines[engine].amk_data_1;\
\
    mex.SystemReady = s_w->AMK_STATUS.bSystemReady;\
    mex.HVOn = s_w->AMK_STATUS.AMK_bDcOn;\
    mex.HVOnAck = s_w->AMK_STATUS.AMK_bQuitDcOn;\
    mex.Derating = s_w->AMK_STATUS.AMK_bDerating;\
    mex.InverterOn = s_w->AMK_STATUS.AMK_bInverterOn;\
    mex.InverterOnAck = s_w->AMK_STATUS.AMK_bQuitInverterOn;\
\
    mex.Error = s_w->AMK_STATUS.AMK_bError;\
    mex.Warning = s_w->AMK_STATUS.AMK_bWarn;\
\
    mex.Voltage = s_w->AMK_TorqueCurrent;\
    mex.MagCurr = s_w->AMK_MagnetizingCurrent;\
    mex.ActualVelocity = s_w->AMK_ActualVelocity;\
}

static __attribute_maybe_unused__ void send_data_engine(const uint16_t can_id) 
{
    CanMessage mex = {0};
    can_obj_can1_h_t o = {0};
    switch (can_id) {
        case CAN_ID_INVERTERFL1:
            POPULATE_MEX_ENGINE(o.can_0x283_InverterFL1, FRONT_LEFT);
            break;
        case CAN_ID_INVERTERFR1:
            POPULATE_MEX_ENGINE(o.can_0x284_InverterFR1, FRONT_RIGHT);
            break;
        case CAN_ID_INVERTERRR1:
            POPULATE_MEX_ENGINE(o.can_0x288_InverterRR1, REAR_LEFT);
            break;
        case CAN_ID_INVERTERRL1:
            POPULATE_MEX_ENGINE(o.can_0x287_InverterRL1, REAR_RIGHT);
            break;
    }
    mex.id = can_id;
    mex.message_size = pack_message_can1(&o, can_id, &mex.full_word);
    uint8_t raise_interrupt_v = 1;
    write(INVERTER.pipe_interrupt, &raise_interrupt_v, sizeof(raise_interrupt_v));
    hardware_write_can(CAN_MODULE_INVERTER, &mex);
}

static int inverter_start(void* args __attribute_maybe_unused__)
{
    time_var_microseconds t = 0;
    while (1) {
        if ((timer_time_now() - t) > 100 MILLIS) {
            send_data_engine(CAN_ID_INVERTERFR1);
            send_data_engine(CAN_ID_INVERTERFL1);
            send_data_engine(CAN_ID_INVERTERRL1);
            send_data_engine(CAN_ID_INVERTERRR1);
            t = timer_time_now();
        }
    }
    return 0;
}

//public

void car_amk_inverter_class_init(void)
{
    thrd_t t;
    thrd_create(&t, inverter_start, NULL);
}

int8_t car_amk_inverter_set_attribute(
        const enum INVERTER_ATTRIBUTE attribute, const uint8_t engine,
        const int64_t value)
{
    struct amk_engines* p_engine = &INVERTER.engines[engine];
    switch (attribute) {
        case ERROR:
            p_engine->amk_data_1.AMK_STATUS.AMK_bError = value;
            break;
        case WARNING:
            p_engine->amk_data_1.AMK_STATUS.AMK_bWarn = value;
            break;
        case DERATRING:
            p_engine->amk_data_1.AMK_STATUS.AMK_bDerating = value;
            break;
        case HV:
            p_engine->amk_data_1.AMK_STATUS.AMK_bDcOn = value;
            break;
        case HV_ACK:
            p_engine->amk_data_1.AMK_STATUS.AMK_bQuitDcOn = value;
            break;
        case INVERTER_ON:
            p_engine->amk_data_1.AMK_STATUS.AMK_bInverterOn = value;
            break;
        case INVERTER_ON_ACK:
            p_engine->amk_data_1.AMK_STATUS.AMK_bQuitInverterOn = value;
            break;
        case SYSTEM_READY:
            p_engine->amk_data_1.AMK_STATUS.bSystemReady = value;
            break;
    }

    return 0;
}
