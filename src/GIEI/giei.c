#include "giei.h"
#include "./amk/amk.h"
#include "engine_interface.h"
#include "../driver_input/driver_input.h"
#include "../board_conf/id_conf.h"
#include "../lib/raceup_board/raceup_board.h"
#include "../emergency_fault/emergency_fault.h"
#include "rege_alg/regen_alg.h"
#include "../board_can/board_can.h"
#include "../utility/arithmetic/arithmetic.h"
#include "../../lib/board_dbc/can2.h"
#include "../../lib/board_dbc/can1.h"
#include "engine_common.h"
#include "power_control/power_control.h"
#include "power_maps/power_maps.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>


//private

#define SPEED_LIMIT                         18000           //INFO: Typical value: 15000
#define DEFAULT_MAX_POS_TORQUE              15.0f
#define DEFAULT_MAX_NEG_TORQUE              -15.f
#define PEAK_REGEN_CURRENT                  150.0f
#define DEFAULT_REPARTITION                 0.5f
#define DEFAULT_POWER_LIMIT                 70000.0f       //INFO: Watt

#define M_N                         9.8f

static struct __GIEI{
    time_var_microseconds sound_start_at;
    float limit_power;
    float limit_pos_torque;
    float limit_neg_torque;
    float limit_regen;
    float limit_max_speed;
    float settings_power_repartition;

    uint8_t activate_torque_vectoring :1;
    uint32_t batteryPackTension;
    float total_power;
    float lem_current;
    enum RUNNING_STATUS running_status;
}GIEI;


/*
 * Battery pack tension is given indipendently by every motor.
 * The function seems complex because takes in consideration the case
 * that one or more motor are inactive.
 *
 * BMS precharge needs a message with the tot voltage
 */
static void computeBatteryPackTension(const float engines_voltages[NUM_OF_EGINES])
{
    uint8_t active_motors = 0;
    float sum = 0.0f;
    uint8_t max = 0;

    // find max voltage
    for (uint8_t i = 0; i < NUM_OF_EGINES; i++)
    {
        if (engines_voltages[i] > max)
        {
            max = engines_voltages[i];
        }
    }

    // Compute sum of voltages, exclude if it is below 50 V than the maximum reading
    for (uint8_t i = 0; i < NUM_OF_EGINES; i++)
    {
        if (engines_voltages[i] > (max - 50))
        {
            active_motors++;
            sum += engines_voltages[i];
        }
    }

    if (!active_motors) {
        GIEI.batteryPackTension = 0;
        GIEI.total_power = 0.0f;
    }
    else {
        GIEI.batteryPackTension = (sum / active_motors);
        GIEI.total_power = GIEI.batteryPackTension * GIEI.lem_current;
    }
}

static float torqueSetpointToNM(const int setpoint)
{
    return (setpoint/1000.0)*M_N;
}

static int NMtoTorqueSetpoint(const float torqueNM)
{
    return (torqueNM/M_N)*1000;
}


static void update_torque_NM_vectors_no_tv(
        const float throttle, float posTorquesNM[NUM_OF_EGINES], const float actual_max_pos_torque)
{
    float probability_of_success = (GIEI.settings_power_repartition/ (1 - GIEI.settings_power_repartition));
    for (uint8_t i = 0; i < NUM_OF_EGINES; i++)
    {
        /*
         * 0 - 100 throttle and brake to Nm (setpoint is relative to nominal torque and goes in 0.1% that's why there is * 10)
         * Setpoints are scaled to obtain desired torque repartition that inverts during braking
         * Negative torques are computed in regBrake() 
         */
        uint32_t setpoint =  throttle* (actual_max_pos_torque/M_N) * 10;
        switch (i) {
            case FRONT_LEFT:
            case FRONT_RIGHT:
                posTorquesNM[i] = torqueSetpointToNM(setpoint * probability_of_success);
                break;
            case REAR_LEFT:
            case REAR_RIGHT:
                posTorquesNM[i] = torqueSetpointToNM(setpoint);
                break;
        }
    }
}

static int8_t send_can_settings_message(void)
{
    can_obj_can2_h_t o;
    memset(&o, 0, sizeof(o));
    CanMessage mex;
    o.can_0x066_CarSettings.pwr_limit = GIEI.limit_power;
    o.can_0x066_CarSettings.speed_lim = GIEI.limit_max_speed;
    o.can_0x066_CarSettings.max_neg_trq = engine_max_pos_torque(GIEI.limit_max_speed);
    o.can_0x066_CarSettings.max_neg_trq = engine_max_neg_torque(GIEI.limit_max_speed);
    o.can_0x066_CarSettings.torque_vectoring = GIEI.activate_torque_vectoring;
    o.can_0x066_CarSettings.max_regen_current = GIEI.limit_regen;
    o.can_0x066_CarSettings.rear_motor_repartition = 
        (uint8_t) (GIEI.settings_power_repartition * 100);
    o.can_0x066_CarSettings.front_motor_repartition = 
        (uint8_t) ((1.0f - GIEI.settings_power_repartition) * 100);
    mex.id = CAN_ID_CARSETTINGS;
    mex.message_size = pack_message_can2(&o, CAN_ID_CARSETTINGS, &mex.full_word);

    return board_can_write(CAN_MODULE_GENERAL, &mex);
}

static int8_t send_can_status_message(void)
{
    can_obj_can2_h_t o;
    memset(&o, 0, sizeof(o));
    CanMessage mex;
    o.can_0x065_CarStatus.HV = engine_inverter_hv_status();
    o.can_0x065_CarStatus.RF = gpio_read_state(READY_TO_DRIVE_INPUT_BUTTON); //FIX: use ack from the inverter
    o.can_0x065_CarStatus.R2D = GIEI.running_status == RUNNING;
    o.can_0x065_CarStatus.AIR1 = gpio_read_state(AIR_PRECHARGE_INIT);
    o.can_0x065_CarStatus.AIR2 = gpio_read_state(AIR_PRECHARGE_DONE);
    o.can_0x065_CarStatus.precharge = 
        gpio_read_state(AIR_PRECHARGE_INIT) && gpio_read_state(AIR_PRECHARGE_DONE);
    o.can_0x065_CarStatus.speed = 0; //TODO: not yet implemented
    mex.id = CAN_ID_CARSTATUS;
    mex.message_size = pack_message_can2(&o, CAN_ID_CARSTATUS, &mex.full_word);

    return board_can_write(CAN_MODULE_GENERAL, &mex);
}

//public

int8_t GIEI_initialize(void)
{
    GIEI.limit_power = DEFAULT_POWER_LIMIT;
    GIEI.limit_pos_torque = DEFAULT_MAX_POS_TORQUE;
    GIEI.limit_neg_torque = DEFAULT_MAX_NEG_TORQUE;
    GIEI.limit_regen = PEAK_REGEN_CURRENT;
    GIEI.limit_max_speed = SPEED_LIMIT;
    GIEI.settings_power_repartition = DEFAULT_REPARTITION;
    GIEI.activate_torque_vectoring = 0;

    engine_module_init();
    giei_power_map_init();
    regen_alg_init();

    return 0;
}

enum RUNNING_STATUS GIEI_check_running_condition(void)
{
    static uint8_t rtd_done = 0;
    const time_var_microseconds sound_duration = 3 SECONDS;
    enum RUNNING_STATUS rt = SYSTEM_OFF;

    if ((timer_time_now() - GIEI.sound_start_at) > sound_duration) {
        gpio_set_high(READY_TO_DRIVE_OUT_SOUND);
        gpio_set_high(READY_TO_DRIVE_OUT_LED);
    }
    rt = engine_rtd_procedure();
    if (rt == RUNNING && !rtd_done) {
        rtd_done =1;
        gpio_set_low(READY_TO_DRIVE_OUT_SOUND);
        gpio_set_low(READY_TO_DRIVE_OUT_LED);
        GIEI.sound_start_at = timer_time_now();
    }else if (rt != RUNNING) {
        rtd_done =0;
        gpio_set_high(READY_TO_DRIVE_OUT_SOUND);
        gpio_set_high(READY_TO_DRIVE_OUT_LED);
    }
    GIEI.running_status = rt;
    return rt;
}

int8_t GIEI_recv_data(const CanMessage* const restrict mex)
{
    switch (mex->id) {
        case CAN_ID_LEM:
            can_obj_can2_h_t o;
            unpack_message_can2(&o, mex->id, mex->full_word, mex->message_size, 0);
            GIEI.lem_current = o.can_0x3c2_Lem.current;
            break;
        case CAN_ID_INVERTERFL1:
        case CAN_ID_INVERTERFL2:
        case CAN_ID_INVERTERFR1:
        case CAN_ID_INVERTERFR2:
        case CAN_ID_INVERTERRL1:
        case CAN_ID_INVERTERRL2:
        case CAN_ID_INVERTERRR1:
        case CAN_ID_INVERTERRR2:
            engine_update_status(mex);
            break;
        default:
            return -1;
    
    }
    return 0;
}

int8_t GIEI_set_limits(const enum GIEI_LIMITS category, const float value)
{
    switch (category) {
        case POWER_LIMIT:
            GIEI.limit_power = value;
            break;
        case MAX_POS_TORQUE_LIMIT:
            GIEI.limit_pos_torque = value;
            break;
        case MAX_NEG_TORQUE_LIMIT:
            GIEI.limit_neg_torque= value;
            break;
        case MOTOR_REPARTIION:
            GIEI.settings_power_repartition = value;
            break;
        case TORQUE_VECTORING_ACTIVATION:
            GIEI.activate_torque_vectoring = value;
            break;
    }
    printf("power limit: %f\t pos torque: %f\t neg torque: %f\t repartition: %f\t tv: %d\n",
            GIEI.limit_power, GIEI.limit_pos_torque, GIEI.limit_neg_torque, 
            GIEI.settings_power_repartition,GIEI.activate_torque_vectoring);
    return 0;
}

int8_t GIEI_input(const float throttle, const float regen)
{
    const float actual_max_pos_torque = engine_max_pos_torque(GIEI.limit_pos_torque);
    const float actual_max_neg_torque = engine_max_neg_torque(GIEI.limit_neg_torque);
    float posTorquesNM[NUM_OF_EGINES];
    float negTorquesNM[NUM_OF_EGINES];
    float engines_voltages[NUM_OF_EGINES] = {
        engine_get_info(FRONT_LEFT,ENGINE_VOLTAGE),
        engine_get_info(FRONT_RIGHT,ENGINE_VOLTAGE),
        engine_get_info(REAR_LEFT,ENGINE_VOLTAGE),
        engine_get_info(REAR_RIGHT,ENGINE_VOLTAGE),
    };
    memset(posTorquesNM, 0, sizeof(posTorquesNM));
    memset(negTorquesNM, 0, sizeof(negTorquesNM));

    if (GIEI.activate_torque_vectoring) {
        //TODO: tv
    }else{
        update_torque_NM_vectors_no_tv(throttle, posTorquesNM, actual_max_neg_torque);
    }

    computeBatteryPackTension(engines_voltages);
    if (throttle > 0 && regen >= 0){
        powerControl(GIEI.total_power, GIEI.limit_power, posTorquesNM);
    }

    const struct RegenAlgInput input ={
        .rear_left_velocity = engine_get_info(REAR_LEFT, ENGINE_VOLTAGE),
        .rear_right_velocity = engine_get_info(REAR_RIGHT, ENGINE_VOLTAGE),
        .front_left_velocity = engine_get_info(FRONT_LEFT, ENGINE_VOLTAGE),
        .front_right_velocity = engine_get_info(FRONT_RIGHT, ENGINE_VOLTAGE),
    };
    regen_alg_compute(&input, negTorquesNM);


    for (uint8_t i = 0; i < NUM_OF_EGINES; i++) {
        const float saturated_pos_torque_nm = 
            saturate_float(posTorquesNM[i], actual_max_pos_torque, 0.0f);
        const float saturated_neg_torque_nm = 
            saturate_float(negTorquesNM[i],0.0f, actual_max_neg_torque);
        const float posTorque = NMtoTorqueSetpoint(saturated_pos_torque_nm);
        const float negTorque = NMtoTorqueSetpoint(saturated_neg_torque_nm);

        if (posTorque > 0 && negTorque < 0) {
            engine_send_torque(i, 0, negTorque);
        }else {
            engine_send_torque(i, posTorque, negTorque);
        }
    }

    return 0;
}

int8_t GIEI_send_status_info_in_can(void)
{
    return send_can_status_message() | send_can_settings_message();
}


//debug

uint8_t DEBUG_GIEI_check_limits(float power_limit, float pos_torque, 
        float neg_torque, float repartition, float tv_on)
{
    return 
        GIEI.limit_power == power_limit &&
        GIEI.limit_pos_torque == pos_torque &&
        GIEI.limit_neg_torque == neg_torque &&
        GIEI.settings_power_repartition == repartition &&
        GIEI.activate_torque_vectoring == tv_on;
}
