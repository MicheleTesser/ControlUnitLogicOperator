#include "./amk.h"
#include "../../../lib/board_dbc/can1.h"
#include "../../board_conf/id_conf.h"
#include "../../board_can/board_can.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//INFO: doc/amk_datasheet.pdf page 61

struct AMK_Actual_Values_1{
    union{
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
        }fields;
        uint16_t raw;
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
    union{
        struct {
            uint8_t AMK_bReserve_start;
            uint8_t AMK_bInverterOn:1;
            uint8_t AMK_bDcOn:1;
            uint8_t AMK_bEnable:1;
            uint8_t AMK_bErrorReset:1;
            uint8_t AMK_bReserve_end :4;
        }AMK_Control_fields;
        uint16_t AMK_Control; 
    };
    int16_t AMK_TargetVelocity; //INFO: unit: rpm Speed setpoint
    int16_t AMK_TorqueLimitPositive; //INFO: unit: 0.1% MN Positive torque limit (subject to nominal torque)
    int16_t AMK_TorqueLimitNegative; //INFO: unit: 0.1% MN Negative torque limit (subject to nominal torque)
};


//private

static struct {
    struct AMK_Actual_Values_1 amk_data_1;
    struct AMK_Actual_Values_2 amk_data_2;
    uint8_t engine_on:1;
}inverter_engine_data[4];

#define POPULATE_MEX_ENGINE(om,amk_stop,engine)\
    om.engine.NegTorq = amk_stop->AMK_TorqueLimitNegative;\
    om.engine.PosTorq = amk_stop->AMK_TorqueLimitPositive;\
    om.engine.TargetVel = amk_stop->AMK_TargetVelocity;\
    om.engine.ControlWord = amk_stop->AMK_Control;

static int8_t send_message_amk(const enum ENGINES engine,
        const struct AMK_Setpoints* const restrict setpoint)
{
    int8_t err=0;
    can_obj_can1_h_t om;
    CanMessage mex;
    switch (engine) {
        case FRONT_LEFT:
            POPULATE_MEX_ENGINE(om,setpoint,can_0x184_VCUInvFL);
            mex.id = CAN_ID_VCUINVFL;
            break;
        case FRONT_RIGHT:
            POPULATE_MEX_ENGINE(om,setpoint,can_0x185_VCUInvFR);
            mex.id = CAN_ID_VCUINVFR;
            break;
        case REAR_LEFT:
            POPULATE_MEX_ENGINE(om,setpoint,can_0x188_VCUInvRL);
            mex.id = CAN_ID_VCUINVRL;
            break;
        case REAR_RIGHT:
            POPULATE_MEX_ENGINE(om,setpoint,can_0x189_VCUInvRR);
            mex.id = CAN_ID_VCUINVRR;
            break;
        default:
            goto invalid_mex_type;
    }

    mex.message_size = pack_message_can1(&om, mex.id, &mex.full_word);
    return board_can_write(CAN_MODULE_INVERTER, &mex);

invalid_mex_type:
    err--;

    return err;
}

//public

int8_t init_engine(const enum ENGINES engine)
{
    struct AMK_Setpoints setpoint;
    memset(&setpoint, 0, sizeof(setpoint));
    struct AMK_Actual_Values_1* engine_info = &inverter_engine_data[engine].amk_data_1;

    if (engine_info->AMK_STATUS.fields.bSystemReady) {
        setpoint.AMK_Control_fields.AMK_bDcOn=1;
    }

    if (inverter_engine_data[engine].engine_on && engine_info->AMK_STATUS.fields.AMK_bQuitDcOn) {
        setpoint.AMK_Control_fields.AMK_bEnable =1;
        setpoint.AMK_Control_fields.AMK_bInverterOn =1;
    }

    int8_t return_send = send_message_amk(engine, &setpoint);

    if (engine_info->AMK_STATUS.fields.bSystemReady &&
            engine_info->AMK_STATUS.fields.AMK_bQuitDcOn &&
            engine_info->AMK_STATUS.fields.AMK_bQuitInverterOn) {
        inverter_engine_data[engine].engine_on =1;
    }

    return return_send;
}

int8_t stop_engine(const enum ENGINES engine)
{
    const struct AMK_Setpoints amk_stop ={
        .AMK_Control = 0,
        .AMK_TargetVelocity =0,
        .AMK_TorqueLimitPositive = 0,
        .AMK_TorqueLimitNegative = 0
    };
    return send_message_amk(engine, &amk_stop);
}

int8_t set_regen_brake_engine(const enum ENGINES engine, const int16_t brake)
{
    const struct AMK_Setpoints amk_brake ={
        .AMK_Control = 0,
        .AMK_TargetVelocity =0,
        .AMK_TorqueLimitPositive = 0,
        .AMK_TorqueLimitNegative = brake
    };
    return send_message_amk(engine, &amk_brake);
}

int8_t set_throttle_engine(const enum ENGINES engine, const int16_t throttle)
{
    const struct AMK_Setpoints amk_throttle ={
        .AMK_Control = 0,
        .AMK_TargetVelocity =0,
        .AMK_TorqueLimitPositive = throttle,
        .AMK_TorqueLimitNegative = 0
    };
    return send_message_amk(engine, &amk_throttle);
}


/*
 * Tramaccio: we wait 500ms before stating that an inverter has no Hv
 */
uint8_t inverter_hv_status(void)
{
    uint8_t i;
    const uint8_t HV_TRAP = 50;
    static uint8_t hvCounter[sizeof(inverter_engine_data)/sizeof(inverter_engine_data[0])];
    static uint8_t inverterHV[sizeof(inverter_engine_data)/sizeof(inverter_engine_data[0])];

    for (i = 0; i < sizeof(inverter_engine_data)/sizeof(inverter_engine_data[0]); i++)
    {
        if (!(inverter_engine_data[i].amk_data_1.AMK_STATUS.fields.AMK_bQuitDcOn) && 
                (hvCounter[i] < HV_TRAP))
            hvCounter[i]++;

        else if (!(inverter_engine_data[i].amk_data_1.AMK_STATUS.fields.AMK_bQuitDcOn) && 
                (hvCounter[i] >= HV_TRAP))
        {
            inverterHV[i] = inverter_engine_data[i].amk_data_1.AMK_STATUS.fields.AMK_bQuitDcOn;
            hvCounter[i] = 0;
        }
        else if (inverter_engine_data[i].amk_data_1.AMK_STATUS.fields.AMK_bQuitDcOn)
        {
            inverterHV[i] = inverter_engine_data[i].amk_data_1.AMK_STATUS.fields.AMK_bQuitDcOn;
            hvCounter[i] = 0;
        }
    }

    return inverterHV[FRONT_RIGHT] | inverterHV[FRONT_LEFT]
        | inverterHV[REAR_RIGHT] | REAR_LEFT;
}

#define STATUS_WORD_1(engine,mex)\
        values_1 = &inverter_engine_data[engine].amk_data_1;\
        values_1->AMK_STATUS.fields.bSystemReady = mex.SystemReady;\
        values_1->AMK_STATUS.fields.AMK_bQuitDcOn = mex.HVOnAck;\
        values_1->AMK_STATUS.fields.AMK_bDcOn = mex.HVOn;\
        values_1->AMK_STATUS.fields.AMK_bInverterOn = mex.InverterOn;\
        values_1->AMK_STATUS.fields.AMK_bQuitInverterOn = mex.InverterOnAck;\
        values_1->AMK_STATUS.fields.AMK_bDerating = mex.Derating;\
        values_1->AMK_STATUS.fields.AMK_bError = mex.Error;\
        values_1->AMK_STATUS.fields.AMK_bWarn = mex.Warning;\
        \
        values_1->AMK_ActualVelocity = mex.ActualVelocity;\
        values_1->AMK_MagnetizingCurrent = mex.MagCurr;\
        values_1->AMK_TorqueCurrent = mex.Voltage;

#define STATUS_WORD_2(engine,mex)\
    values_2 = &inverter_engine_data[engine].amk_data_2;\
    values_2->AMK_TempIGBT = mex.TempIGBT;\
    values_2->AMK_TempMotor = mex.TempIGBT;\
    values_2->AMK_ErrorInfo = mex.ErrorInfo;\
    values_2->AMK_TempInverter = mex.TempInv;

void update_status(const CanMessage* const restrict mex)
{
    struct AMK_Actual_Values_2* values_2 = NULL;
    struct AMK_Actual_Values_1* values_1 = NULL;
    can_obj_can1_h_t o;
    unpack_message_can1(&o, mex->id, mex->full_word, mex->message_size, 0);
    switch (mex->id) {
        case CAN_ID_INVERTERFL1:
            STATUS_WORD_1(FRONT_LEFT, o.can_0x283_InverterFL1);
            break;
        case CAN_ID_INVERTERFL2:
            STATUS_WORD_2(FRONT_LEFT, o.can_0x285_InverterFL2);
            break;

        case CAN_ID_INVERTERFR1:
            STATUS_WORD_1(FRONT_RIGHT, o.can_0x284_InverterFR1);
            break;
        case CAN_ID_INVERTERFR2:
            STATUS_WORD_2(FRONT_RIGHT, o.can_0x286_InverterFR2);
            break;

        case CAN_ID_INVERTERRL1:
            STATUS_WORD_1(REAR_LEFT, o.can_0x287_InverterRL1);
            break;
        case CAN_ID_INVERTERRL2:
            STATUS_WORD_2(REAR_LEFT, o.can_0x289_InverterRL2);
            break;

        case CAN_ID_INVERTERRR1:
            STATUS_WORD_1(REAR_RIGHT, o.can_0x288_InverterRR1);
            break;
        case CAN_ID_INVERTERRR2:
            STATUS_WORD_2(REAR_RIGHT, o.can_0x28a_InverterRR2);
            break;
    }
}
