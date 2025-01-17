#include "./amk.h"
#include "../../../../lib/board_dbc/dbc/out_lib/can1/can1.h"
#include <stdint.h>
#include <string.h>


//private

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
    }AMK_status;
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

enum AMK_MEX_IDS{
    STATUS_1 =0,
    STATUS_2,
    SETPOINT,
};

struct AMKEngine_t{
    uint16_t AMK_Control;
    int16_t AMK_TargetVelocity; //INFO: unit: rpm Speed setpoint
    int16_t AMK_TorqueLimitPositive; //INFO: unit: 0.1% MN Positive torque limit (subject to nominal torque)
    int16_t AMK_TorqueLimitNegative; //INFO: unit: 0.1% MN Negative torque limit (subject to nominal torque)
    uint16_t mex_ids[3];
};

union AMKConv{
    struct AmkEngine_h* hidden;
    struct AMKEngine_t* clear;
};

#define AMK_H_T_CONV(h_ptr, t_ptr_name)\
    union AMKConv __a_conv##t_ptr_name__ = {h_ptr};\
    struct AMKEngine_t* t_ptr_name = __a_conv##t_ptr_name__.clear;

#ifdef DEBUG
const uint8_t __debug_amk_size__[(sizeof(struct AmkEngine_h) == sizeof(struct AMKEngine_t))? 1 : -1];
#endif /* ifdef DEBUG */

//public

int8_t amk_module_init(struct AmkEngine_h* self, const enum ENGINES engine)
{
    AMK_H_T_CONV(self, p_self);
    memset(p_self, 0, sizeof(*p_self));
    switch (engine) {
        case FRONT_LEFT:
            p_self->mex_ids[STATUS_1] = CAN_ID_INVERTERFL1;
            p_self->mex_ids[STATUS_2] = CAN_ID_INVERTERFL2;
            p_self->mex_ids[SETPOINT] = CAN_ID_VCUINVFL;
            break;
        case FRONT_RIGHT:
            p_self->mex_ids[STATUS_1] = CAN_ID_INVERTERFR1;
            p_self->mex_ids[STATUS_2] = CAN_ID_INVERTERFR2;
            p_self->mex_ids[SETPOINT] = CAN_ID_VCUINVFR;
            break;
        case REAR_LEFT:
            p_self->mex_ids[STATUS_1] = CAN_ID_INVERTERRL1;
            p_self->mex_ids[STATUS_2] = CAN_ID_INVERTERRL2;
            p_self->mex_ids[SETPOINT] = CAN_ID_VCUINVRL;
            break;
        case REAR_RIGHT:
            p_self->mex_ids[STATUS_1] = CAN_ID_INVERTERRR1;
            p_self->mex_ids[STATUS_2] = CAN_ID_INVERTERRR2;
            p_self->mex_ids[SETPOINT] = CAN_ID_VCUINVRR;
            break;
        default:
            return -1;
    }
    return 0;
}

int8_t amk_update_status(struct AmkEngine_h* self)
{
    AMK_H_T_CONV(self, p_self);
    p_self->mex_ids[0] = 0; //INFO: only to remove warning
    return 0;
}
