#include "./amk.h"
#include "../../../lib/board_dbc/dbc/out_lib/can1/can1.h"
#include "../../board_conf/id_conf.h"
#include "../../driver_input/driver_input.h"
#include "../../emergency_fault/emergency_fault.h"
#include "../../utility/arithmetic/arithmetic.h"
#include "../../board_can/board_can.h"
#include "../engine_common.h"
#include <stdint.h>
#include <string.h>

//INFO: doc/amk_datasheet.pdf page 61

#define MAX_MOTOR_TORQUE            21.0f

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


static struct AMK_POWER{
    struct amk_engines{
        struct AMK_Actual_Values_1 amk_data_1;
        struct AMK_Actual_Values_2 amk_data_2;
    }engines[NUM_OF_EGINES];
    enum RUNNING_STATUS engine_status :2;
    uint32_t max_speed;
    time_var_microseconds enter_precharge_phase;
}inverter_engine_data;

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


static uint8_t amk_inverter_on(void)
{
    uint8_t res = 0xFF;
    FOR_EACH_ENGINE({
        res &= inverter_engine_data.engines[index_engine].amk_data_1.AMK_STATUS.fields.bSystemReady;
    })
    return res;
}

static uint8_t amk_activate_hv(void)
{
    FOR_EACH_ENGINE({
        struct AMK_Setpoints setpoint;
        memset(&setpoint, 0, sizeof(setpoint));
        setpoint.AMK_Control_fields.AMK_bDcOn =1;
        send_message_amk(index_engine, &setpoint);
    })
    return 0;
}

static uint8_t amk_disable_inverter(void)
{
    FOR_EACH_ENGINE({
        struct AMK_Setpoints setpoint;
        memset(&setpoint, 0, sizeof(setpoint));
        send_message_amk(index_engine, &setpoint);
    })
    return 0;
}
static uint8_t amk_activate_control(void)
{
    FOR_EACH_ENGINE({
        struct AMK_Setpoints setpoint;
        memset(&setpoint, 0, sizeof(setpoint));
        setpoint.AMK_Control_fields.AMK_bDcOn =1;
        setpoint.AMK_Control_fields.AMK_bEnable=1;
        setpoint.AMK_Control_fields.AMK_bInverterOn=1;
        send_message_amk(index_engine, &setpoint);
    })
    return 0;
}


static inline uint8_t rtd_input_request(void)
{
    const uint8_t brake_treshold_percentage = 10;
    float driver_brake = 0;
    
    DRIVER_INPUT_READ_ONLY_ACTION({
        driver_brake = driver_get_amount(driver_input_read_ptr, BRAKE);
    })

    return  driver_brake > brake_treshold_percentage &&
            input_rtd_check();
}

static inline uint8_t precharge_ended(void)
{
    return gpio_read_state(AIR_PRECHARGE_INIT) && gpio_read_state(AIR_PRECHARGE_DONE);
}

static float max_torque(void)
{
    float torque_max_sum = 0;
    FOR_EACH_ENGINE({
        const float actual_velocity = 
            inverter_engine_data.engines[index_engine].amk_data_1.AMK_ActualVelocity;
        torque_max_sum += MAX_MOTOR_TORQUE - 0.000857*(actual_velocity - 13000.0f);
    })
    return  torque_max_sum/4;
}

//public

int8_t amk_module_init(void)
{
    inverter_engine_data.engine_status = SYSTEM_OFF;
    return 0;
}

int8_t amk_set_max_speed(const uint32_t speed)
{
    inverter_engine_data.max_speed = speed;
    return 0;
}

int8_t amk_send_torque(const enum ENGINES engine, const float pos_torque, const float neg_torque)
{
    struct AMK_Setpoints setpoint  ={
        .AMK_Control = 0,
        .AMK_TargetVelocity = inverter_engine_data.max_speed,
        .AMK_TorqueLimitPositive = pos_torque,
        .AMK_TorqueLimitNegative = neg_torque,
    };
    return send_message_amk(engine, &setpoint);
}

#define STATUS_WORD_1(engine,mex)\
        values_1 = &inverter_engine_data.engines[engine].amk_data_1;\
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
    values_2 = &inverter_engine_data.engines[engine].amk_data_2;\
    values_2->AMK_TempIGBT = mex.TempIGBT;\
    values_2->AMK_TempMotor = mex.TempIGBT;\
    values_2->AMK_ErrorInfo = mex.ErrorInfo;\
    values_2->AMK_TempInverter = mex.TempInv;

int8_t amk_update_status(const CanMessage* const restrict mex)
{
    struct AMK_Actual_Values_2* values_2 = NULL;
    struct AMK_Actual_Values_1* values_1 = NULL;
    can_obj_can1_h_t o;
    memset(&o, 0, sizeof(0));
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
        default:
            return -1;
    }

    return 0;
}

/*
 * Tramaccio: we wait 500ms before stating that an inverter has no Hv
 */
uint8_t amk_inverter_hv_status(void)
{
    const uint8_t HV_TRAP = 50;
    static uint8_t hvCounter[NUM_OF_EGINES];
    uint8_t res = 0;

    FOR_EACH_ENGINE({
        const uint8_t AMK_bQuitDcOn = 
            inverter_engine_data.engines[index_engine].amk_data_1.AMK_STATUS.fields.AMK_bQuitDcOn;
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

enum RUNNING_STATUS amk_rtd_procedure(void)
{
    if(inverter_engine_data.engine_status == SYSTEM_OFF){
        one_emergency_solved(FAILED_RTD_SEQ);
    }

    if (is_emergency_state()) {
        amk_disable_inverter();
        inverter_engine_data.engine_status = SYSTEM_OFF;
        return inverter_engine_data.engine_status;
    }

    switch (inverter_engine_data.engine_status) {
        case SYSTEM_OFF:
            if (amk_inverter_on() && !rtd_input_request())
            {
                amk_activate_hv();
                inverter_engine_data.enter_precharge_phase = timer_time_now();
                if (amk_inverter_hv_status())
                {
                    inverter_engine_data.engine_status = SYSTEM_PRECAHRGE;
                }
            }
            else
            {
                amk_shut_down_power();
                if(rtd_input_request())
                {
                    amk_shut_down_power();
                    one_emergency_raised(FAILED_RTD_SEQ);
                }
            }
            break;
        case SYSTEM_PRECAHRGE:
            if (!amk_inverter_on() || !amk_inverter_hv_status() || rtd_input_request())
            {
                amk_shut_down_power();
                one_emergency_raised(FAILED_RTD_SEQ);
                inverter_engine_data.engine_status = SYSTEM_OFF;
            }
            else if (precharge_ended())
            {
                amk_activate_control();
                inverter_engine_data.engine_status = TS_READY;
            }
            break;
        case TS_READY:
            if (!amk_inverter_hv_status() || !precharge_ended() || !amk_inverter_on())
            {
                amk_shut_down_power();
                one_emergency_raised(FAILED_RTD_SEQ);
                inverter_engine_data.engine_status = SYSTEM_OFF;
                break;
            }
            if (rtd_input_request())
            {
                inverter_engine_data.engine_status = RUNNING;
            }
            break;
        case RUNNING:
            if (!amk_inverter_on() || !amk_inverter_hv_status() || 
                    !precharge_ended() || !amk_inverter_on())
            {
                amk_shut_down_power();
                one_emergency_raised(FAILED_RTD_SEQ);
                inverter_engine_data.engine_status = SYSTEM_OFF;
            }
            else if (!input_rtd_check())
            {
                amk_disable_inverter();
                inverter_engine_data.engine_status= TS_READY;
            }
            break;
        }
    return inverter_engine_data.engine_status;
}

void amk_shut_down_power(void)
{
    amk_disable_inverter();
    inverter_engine_data.engine_status= SYSTEM_OFF;
}

float amk_get_info(const enum ENGINES engine, const enum ENGINE_INFO info)
{
    switch (info) {
        case ENGINE_VOLTAGE:
            return inverter_engine_data.engines[engine].amk_data_1.AMK_TorqueCurrent;
            break;
        case ENGINE_RPM:
            return inverter_engine_data.engines[engine].amk_data_1.AMK_ActualVelocity;
        default:
            return -1;
    }
    return 0;
}

float amk_max_pos_torque(const float limit_max_pos_torque)
{
    const float v_max_torque = max_torque();
    return saturate_float(v_max_torque, limit_max_pos_torque, 0.0f);

}
float amk_max_neg_torque(const float limit_max_neg_torque)
{
    const float v_max_torque = max_torque();
    return  -saturate_float(v_max_torque, 0.0f, limit_max_neg_torque);
}
