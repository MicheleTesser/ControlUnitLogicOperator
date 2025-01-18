#include "./amk.h"
#include "../../../../lib/board_dbc/dbc/out_lib/can1/can1.h"
#include "../../../../lib/raceup_board/raceup_board.h"
#include "../../../../emergency_module/emergency_module.h"
#include "../../../IO_id_0/IO_id_0.h"
#include <stdint.h>
#include <string.h>
#include <stdatomic.h>


//private

enum AMK_EMERGENCY {
    FAILED_RTD_AMK =0,
};

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

struct AMKInverter_t{
    struct{
        struct AMK_Actual_Values_1 amk_values_1;
        struct AMK_Actual_Values_2 amk_values_2;
        int16_t AMK_TargetVelocity; //INFO: unit: rpm Speed setpoint
        int16_t AMK_TorqueLimitPositive; //INFO: unit: 0.1% MN Positive torque limit (subject to nominal torque)
        int16_t AMK_TorqueLimitNegative; //INFO: unit: 0.1% MN Negative torque limit (subject to nominal torque)
    }engines[NUM_OF_EGINES];
    enum RUNNING_STATUS engine_status;
    time_var_microseconds enter_precharge_phase;
    struct EmergencyNode* amk_emergency;
    struct CanNode* can_inverter;
    const struct DriverInput_h* driver_input;
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
static uint8_t rtd_input_on;

static inline void inverter_mex_recv(void)
{
    atomic_fetch_add(&mex_inverter, 1);
}

static inline void toggle_active_rtd_input(void)
{
    rtd_input_on ^=1;
}

#define POPULATE_MEX_ENGINE(amk_stop,engine)\
    engine.NegTorq = amk_stop->AMK_TorqueLimitNegative;\
    engine.PosTorq = amk_stop->AMK_TorqueLimitPositive;\
    engine.TargetVel = amk_stop->AMK_TargetVelocity;\
    engine.ControlWord |= (setpoint->AMK_Control_fields.AMK_bInverterOn << 8);\
    engine.ControlWord |= (setpoint->AMK_Control_fields.AMK_bDcOn << 9);\
    engine.ControlWord |= (setpoint->AMK_Control_fields.AMK_bEnable << 10);\
    engine.ControlWord |= (setpoint->AMK_Control_fields.AMK_bErrorReset << 11);\

static int8_t send_message_amk(const struct AMKInverter_t* const restrict self,
        const enum ENGINES engine, 
        const struct AMK_Setpoints* const restrict setpoint)
{
    int8_t err=0;
    can_obj_can1_h_t om;
    CanMessage mex;
    switch (engine) {
        case FRONT_LEFT:
            POPULATE_MEX_ENGINE(setpoint, om.can_0x184_VCUInvFL);
            mex.id = CAN_ID_VCUINVFL;
            break;
        case FRONT_RIGHT:
            POPULATE_MEX_ENGINE(setpoint, om.can_0x185_VCUInvFR);
            mex.id = CAN_ID_VCUINVFR;
            break;
        case REAR_LEFT:
            POPULATE_MEX_ENGINE(setpoint, om.can_0x188_VCUInvRL);
            mex.id = CAN_ID_VCUINVRL;
            break;
        case REAR_RIGHT:
            POPULATE_MEX_ENGINE(setpoint, om.can_0x189_VCUInvRR);
            mex.id = CAN_ID_VCUINVRR;
            break;
        default:
            goto invalid_mex_type;
    }

    mex.message_size = pack_message_can1(&om, mex.id, &mex.full_word);
    return hardware_write_can(self->can_inverter, &mex);

invalid_mex_type:
    err--;

    return err;
}

static uint8_t amk_disable_inverter(const struct AMKInverter_t* const restrict self)
{
    FOR_EACH_ENGINE({
        struct AMK_Setpoints setpoint;
        memset(&setpoint, 0, sizeof(setpoint));
        send_message_amk(self, index_engine, &setpoint);
    })
    return 0;
}


static uint8_t amk_inverter_on(const struct AMKInverter_t* const restrict self)
{
    uint8_t res = 0xFF;
    FOR_EACH_ENGINE({
        res &= self->engines[index_engine].amk_values_1.AMK_status.bSystemReady;
    })
    return res;
}

static inline uint8_t precharge_ended(void)
{
    return gpio_read_state(AIR_PRECHARGE_INIT) && gpio_read_state(AIR_PRECHARGE_DONE);
}

static void amk_shut_down_power(struct AMKInverter_t* const restrict self)
{
    amk_disable_inverter(self);
    self->engine_status= SYSTEM_OFF;
}

static uint8_t amk_activate_hv(const struct AMKInverter_t* const restrict self)
{
    FOR_EACH_ENGINE({
        struct AMK_Setpoints setpoint;
        memset(&setpoint, 0, sizeof(setpoint));
        setpoint.AMK_Control_fields.AMK_bDcOn =1;
        send_message_amk(self, index_engine, &setpoint);
    })
    return 0;
}

static uint8_t amk_inverter_hv_status(const struct AMKInverter_t* const restrict self)
{
    const uint8_t HV_TRAP = 50;
    static uint8_t hvCounter[NUM_OF_EGINES];
    uint8_t res = 0;

    FOR_EACH_ENGINE({
        const uint8_t AMK_bQuitDcOn = 
            self->engines[index_engine].amk_values_1.AMK_status.AMK_bQuitDcOn;
        if (!(AMK_bQuitDcOn) && (hvCounter[index_engine] < HV_TRAP))
        {
            hvCounter[index_engine]++;
        }
        else if (AMK_bQuitDcOn || (hvCounter[index_engine] >= HV_TRAP))
        {
            res |= AMK_bQuitDcOn;
            hvCounter[index_engine] = 0;
        }
    })


    return res;
}

static uint8_t amk_activate_control(const struct AMKInverter_t* const restrict self)
{
    FOR_EACH_ENGINE({
        struct AMK_Setpoints setpoint;
        memset(&setpoint, 0, sizeof(setpoint));
        setpoint.AMK_Control_fields.AMK_bDcOn =1;
        setpoint.AMK_Control_fields.AMK_bEnable=1;
        setpoint.AMK_Control_fields.AMK_bInverterOn=1;
        send_message_amk(self, index_engine, &setpoint);
    })
    return 0;
}

static inline uint8_t rtd_input_request(const struct AMKInverter_t* const restrict self)
{
    const float brake = driver_get_amount(self->driver_input, BRAKE);
    return gpio_read_state(GPIO_RTD_BUTTON) && brake > 20;
}

//public

int8_t amk_module_init(struct AmkInverter_h* const restrict self,
        const struct DriverInput_h* const p_driver_input)
{
    AMK_H_T_CONV(self, p_self);
    memset(p_self, 0, sizeof(*p_self));
    p_self->engine_status =SYSTEM_OFF;
    p_self->amk_emergency = EmergencyNode_init(1); //TODO: set the correct amount
    p_self->driver_input = p_driver_input;
    if (!p_self->amk_emergency) {
        EmergencyNode_free(p_self->amk_emergency);
        return -1;
    }
    p_self->can_inverter = hardware_init_can(CAN_INVERTER, _1_MBYTE_S_);
    if (!p_self->can_inverter) {
        return -1;
    }
    if(hardware_interrupt_attach_fun(INTERRUPT_INVERTER_MEX, inverter_mex_recv) <0 || 
            hardware_trap_attach_fun(TRAP__4__, toggle_active_rtd_input) <0)
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
        hardware_read_can(p_self->can_inverter, &mex);
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

enum RUNNING_STATUS amk_rtd_procedure(struct AmkInverter_h* const restrict self)
{
    AMK_H_T_CONV(self, p_self);
    if(p_self->engine_status == SYSTEM_OFF){
        EmergencyNode_raise(p_self->amk_emergency, FAILED_RTD_AMK);
    }

    if (EmergencyNode_is_emergency_state(p_self->amk_emergency)) {
        amk_disable_inverter(p_self);
        p_self->engine_status = SYSTEM_OFF;
        return p_self->engine_status;
    }

    switch (p_self->engine_status) {
        case SYSTEM_OFF:
            if (amk_inverter_on(p_self) && !rtd_input_request(p_self))
            {
                amk_activate_hv(p_self);
                p_self->enter_precharge_phase = timer_time_now();
                if (amk_inverter_hv_status(p_self))
                {
                    p_self->engine_status = SYSTEM_PRECAHRGE;
                }
            }
            else
            {
                amk_shut_down_power(p_self);
                if(rtd_input_request(p_self))
                {
                    EmergencyNode_raise(p_self->amk_emergency, FAILED_RTD_AMK);
                }
            }
            break;
        case SYSTEM_PRECAHRGE:
            if (!amk_inverter_on(p_self) || !amk_inverter_hv_status(p_self) ||
                    rtd_input_request(p_self))
            {
                EmergencyNode_raise(p_self->amk_emergency, FAILED_RTD_AMK);
                amk_shut_down_power(p_self);
                p_self->engine_status = SYSTEM_OFF;
            }
            else if (precharge_ended())
            {
                amk_activate_control(p_self);
                p_self->engine_status = TS_READY;
            }
            break;
        case TS_READY:
            if (!amk_inverter_hv_status(p_self) || !precharge_ended() || !amk_inverter_on(p_self))
            {
                amk_shut_down_power(p_self);
                EmergencyNode_raise(p_self->amk_emergency, FAILED_RTD_AMK);
                p_self->engine_status = SYSTEM_OFF;
                break;
            }
            if (rtd_input_request(p_self))
            {
                p_self->engine_status = RUNNING;
            }
            break;
        case RUNNING:
            if (!amk_inverter_on(p_self) || !amk_inverter_hv_status(p_self) || !precharge_ended())
            {
                amk_shut_down_power(p_self);
                EmergencyNode_raise(p_self->amk_emergency, FAILED_RTD_AMK);
                p_self->engine_status = SYSTEM_OFF;
            }
            else if (!rtd_input_on)
            {
                amk_disable_inverter(p_self);
                p_self->engine_status= TS_READY;
            }
            break;
        }
    return p_self->engine_status;
}
