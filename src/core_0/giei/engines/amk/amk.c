#include "./amk.h"
#include "../../../../lib/board_dbc/dbc/out_lib/can1/can1.h"
#include "../../../../lib/raceup_board/raceup_board.h"
#include <stdint.h>
#include <string.h>
#include <stdatomic.h>


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

struct AMKInverter_t{
    struct{
        struct AMK_Actual_Values_1 amk_values_1;
        struct AMK_Actual_Values_2 amk_values_2;
        int16_t AMK_TargetVelocity; //INFO: unit: rpm Speed setpoint
        int16_t AMK_TorqueLimitPositive; //INFO: unit: 0.1% MN Positive torque limit (subject to nominal torque)
        int16_t AMK_TorqueLimitNegative; //INFO: unit: 0.1% MN Negative torque limit (subject to nominal torque)
    }engines[NUM_OF_EGINES];
};

union AMKConv{
    struct AmkInverter_h* hidden;
    struct AMKInverter_t* clear;
};

#define AMK_H_T_CONV(h_ptr, t_ptr_name)\
    union AMKConv __a_conv##t_ptr_name__ = {h_ptr};\
    struct AMKInverter_t* t_ptr_name = __a_conv##t_ptr_name__.clear;

#ifdef DEBUG
const uint8_t __debug_amk_size__[(sizeof(struct AmkInverter_h) == sizeof(struct AMKInverter_t))? 1 : -1];
#endif /* ifdef DEBUG */

static atomic_ulong mex_inverter;

static inline void inverter_mex_recv(void)
{
    atomic_fetch_add(&mex_inverter, 1);
}


//public

int8_t amk_module_init(struct AmkInverter_h* const restrict self)
{
    AMK_H_T_CONV(self, p_self);
    memset(p_self, 0, sizeof(*p_self));
    if(hardware_interrupt_attach_fun(INTERRUPT_0_, inverter_mex_recv) <0 || 
            hardware_init_can(CAN_INVERTER, _1_MBYTE_S_))
    {
        return -1;
    }
    return 0;
}


#define STATUS_WORD_1(values_1,mex)\
        values_1.AMK_status.bSystemReady = mex.SystemReady;\
        values_1.AMK_status.AMK_bQuitDcOn = mex.HVOnAck;\
        values_1.AMK_status.AMK_bDcOn = mex.HVOn;\
        values_1.AMK_status.AMK_bInverterOn = mex.InverterOn;\
        values_1.AMK_status.AMK_bQuitInverterOn = mex.InverterOnAck;\
        values_1.AMK_status.AMK_bDerating = mex.Derating;\
        values_1.AMK_status.AMK_bError = mex.Error;\
        values_1.AMK_status.AMK_bWarn = mex.Warning;\
        \
        values_1.AMK_ActualVelocity = mex.ActualVelocity;\
        values_1.AMK_MagnetizingCurrent = mex.MagCurr;\
        values_1.AMK_TorqueCurrent = mex.Voltage;

#define STATUS_WORD_2(values_2,mex)\
    values_2.AMK_TempIGBT = mex.TempIGBT;\
    values_2.AMK_TempMotor = mex.TempIGBT;\
    values_2.AMK_ErrorInfo = mex.ErrorInfo;\
    values_2.AMK_TempInverter = mex.TempInv;


int8_t amk_update_status(struct AmkInverter_h* const restrict self)
{
    CanMessage mex;
    can_obj_can1_h_t o;
    AMK_H_T_CONV(self, p_self);

    if (atomic_load(&mex_inverter))
    {
        hardware_read_can(CAN_INVERTER, &mex);
        unpack_message_can1(&o, mex.id, mex.full_word, mex.message_size, timer_time_now());
        switch (mex.id) {
            case CAN_ID_INVERTERFL1:
                STATUS_WORD_1(p_self->engines[FRONT_LEFT].amk_values_1, o.can_0x283_InverterFL1);
                break;
            case CAN_ID_INVERTERFL2:
                STATUS_WORD_2(p_self->engines[FRONT_LEFT].amk_values_2, o.can_0x285_InverterFL2);
                break;

            case CAN_ID_INVERTERFR1:
                STATUS_WORD_1(p_self->engines[FRONT_RIGHT].amk_values_1, o.can_0x284_InverterFR1);
                break;
            case CAN_ID_INVERTERFR2:
                STATUS_WORD_2(p_self->engines[FRONT_RIGHT].amk_values_2, o.can_0x286_InverterFR2);
                break;

            case CAN_ID_INVERTERRL1:
                STATUS_WORD_1(p_self->engines[REAR_LEFT].amk_values_1, o.can_0x287_InverterRL1);
                break;
            case CAN_ID_INVERTERRL2:
                STATUS_WORD_2(p_self->engines[REAR_LEFT].amk_values_2, o.can_0x289_InverterRL2);
                break;

            case CAN_ID_INVERTERRR1:
                STATUS_WORD_1(p_self->engines[REAR_RIGHT].amk_values_1, o.can_0x288_InverterRR1);
                break;
            case CAN_ID_INVERTERRR2:
                STATUS_WORD_2(p_self->engines[REAR_RIGHT].amk_values_2, o.can_0x28a_InverterRR2);
                break;
        }
        atomic_fetch_sub(&mex_inverter, 1);
    }
    return 0;
}
