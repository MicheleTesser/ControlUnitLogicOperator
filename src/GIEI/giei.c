#include "giei.h"
#include "./amk/amk.h"
#include "engine_interface.h"
#include "../driver_input/driver_input.h"
#include "../board_conf/id_conf.h"
#include "../lib/raceup_board/raceup_board.h"
#include "../emergency_fault/emergency_fault.h"
#include "rege_alg/regen_alg.h"
#include "speed_alg/speed_alg.h"
#include "torque_vec_alg/torque_vec_alg.h"
#include "../utility/arithmetic/arithmetic.h"
#include "../batteries/hv/hv.h"
#include "../IMU/imu.h"
#include "../missions/missons.h"
#include "../../lib/board_dbc/dbc/out_lib/can1/can1.h"
#include "../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#include "engine_common.h"
#include "power_control/power_control.h"
#include "power_maps/power_maps.h"
#include <stdint.h>
#include <string.h>


//private

#define SPEED_LIMIT                         18000           //INFO: Typical value: 15000
#define DEFAULT_MAX_POS_TORQUE              15.0f
#define DEFAULT_MAX_NEG_TORQUE              -15.f
#define PEAK_REGEN_CURRENT                  150.0f
#define DEFAULT_REPARTITION                 0.5f
#define DEFAULT_POWER_LIMIT                 70000.0f       //INFO: Watt

#define M_N                                 9.8f

static struct GIEI{
    time_var_microseconds sound_start_at;
    float limit_power;
    float limit_pos_torque;
    float limit_neg_torque;
    float limit_regen;
    float limit_max_speed;
    float settings_power_repartition;
    enum RUNNING_STATUS running_status;
    uint8_t activate_torque_vectoring :1;
    uint8_t init_done: 1;
    uint8_t mut_ptr: 1;
    uint8_t read_ptr:5;
}GIEI;



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
        switch (i)
        {
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

//public

int8_t GIEI_init(void)
{
    GIEI.limit_power = DEFAULT_POWER_LIMIT;
    GIEI.limit_pos_torque = DEFAULT_MAX_POS_TORQUE;
    GIEI.limit_neg_torque = DEFAULT_MAX_NEG_TORQUE;
    GIEI.limit_regen = PEAK_REGEN_CURRENT;
    GIEI.limit_max_speed = SPEED_LIMIT;
    GIEI.settings_power_repartition = DEFAULT_REPARTITION;
    GIEI.activate_torque_vectoring = 0;

    engine_module_init();
    engine_set_max_speed(GIEI.limit_max_speed);
    giei_power_map_init();
    regen_alg_init();
    giei_speed_alg_class_init();
    
    GIEI.init_done =1;

    return 0;
}

const struct GIEI* GIEI_get(void)
{
    while (!GIEI.init_done && GIEI.mut_ptr) {}
    GIEI.read_ptr++;
    return &GIEI;
}
struct GIEI* GIEI_get_mut(void)
{
    while (!GIEI.init_done && (GIEI.mut_ptr || GIEI.read_ptr)) {}
    GIEI.mut_ptr++;
    return &GIEI;
}

enum RUNNING_STATUS GIEI_check_running_condition(void)
{
    static uint8_t rtd_done = 0;
    const time_var_microseconds sound_duration = 3 SECONDS;
    enum RUNNING_STATUS rt = SYSTEM_OFF;

    if ((timer_time_now() - GIEI.sound_start_at) > sound_duration)
    {
        gpio_set_high(READY_TO_DRIVE_OUT_SOUND);
        gpio_set_high(READY_TO_DRIVE_OUT_LED);
    }
    rt = engine_rtd_procedure();
    if (rt > SYSTEM_OFF)
    {
        mission_lock_mission();
    }
    else
    {
        mission_unlock_mission();
    }
    if (rt == RUNNING && !rtd_done)
    {
        rtd_done =1;
        gpio_set_low(READY_TO_DRIVE_OUT_SOUND);
        gpio_set_low(READY_TO_DRIVE_OUT_LED);
        GIEI.sound_start_at = timer_time_now();
    }
    else if (rt != RUNNING)
    {
        rtd_done =0;
        gpio_set_high(READY_TO_DRIVE_OUT_SOUND);
        gpio_set_high(READY_TO_DRIVE_OUT_LED);
    }
    GIEI.running_status = rt;
    return rt;
}

int8_t GIEI_recv_data(const CanMessage* const restrict mex)
{
    switch (mex->id)
    {
        case CAN_ID_INVERTERFL1:
        case CAN_ID_INVERTERFL2:
        case CAN_ID_INVERTERFR1:
        case CAN_ID_INVERTERFR2:
        case CAN_ID_INVERTERRL1:
        case CAN_ID_INVERTERRL2:
        case CAN_ID_INVERTERRR1:
        case CAN_ID_INVERTERRR2:
            return engine_update_status(mex);
        default:
            return -1;
    
    }
}

int8_t GIEI_set_limits(const enum GIEI_LIMITS category, const float value)
{
    switch (category)
    {
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
    float driver_regen = 0;
    float driver_throttle =0;
    float driver_sterring_angle =0;
    float imu_acc_x =0;
    float imu_acc_y =0;
    float imu_acc_z =0;

    DRIVER_INPUT_READ_ONLY_ACTION({
        driver_regen = driver_get_amount(driver_input_read_ptr, REGEN);
        driver_throttle = driver_get_amount(driver_input_read_ptr, THROTTLE);
        driver_sterring_angle = driver_get_amount(driver_input_read_ptr, STEERING_ANGLE);
    });

    IMU_READ_ONLY_ACTION({
        imu_acc_x = imu_get_info(imu_read_ptr, IMU_accelerations, axis_X);
        imu_acc_y = imu_get_info(imu_read_ptr, IMU_accelerations, axis_Y);
        imu_acc_z = imu_get_info(imu_read_ptr, IMU_accelerations, axis_Z);
    });

    memset(posTorquesNM, 0, sizeof(posTorquesNM));
    memset(negTorquesNM, 0, sizeof(negTorquesNM));

    if (GIEI.activate_torque_vectoring)
    {
        struct TVInputArgs tv_input =
        {
            .ax = imu_acc_x,
            .ay = imu_acc_y,
            .yaw_r = imu_acc_z,
            .throttle = driver_regen,
            .regenpaddle = driver_throttle,
            .brakepressurefront = 0, //TODO: not yet implemented in the code
            .brakepressurerear = 0, //TODO: not yet implemented in the code
            .steering = driver_sterring_angle,
            .rpm[0] = engine_get_info(FRONT_LEFT, ENGINE_RPM),
            .rpm[1] = engine_get_info(FRONT_RIGHT, ENGINE_RPM),
            .rpm[2] = engine_get_info(REAR_RIGHT, ENGINE_RPM),
            .rpm[3] = engine_get_info(REAR_RIGHT, ENGINE_RPM),
            .voltage = 0,
        };
        HV_READ_ONLY_ACTION({
            hv_get_info(hv_read_ptr, HV_BATTERY_PACK_TENSION, 
                    &tv_input.voltage, sizeof(tv_input.voltage));
        });
        tv_alg_compute(&tv_input, posTorquesNM);
    }
    else
    {
        update_torque_NM_vectors_no_tv(throttle, posTorquesNM, actual_max_pos_torque);
    }

    HV_MUT_ACTION({
        hv_computeBatteryPackTension(hv_mut_ptr, engines_voltages, NUM_OF_EGINES);
    })
    if (throttle > 0 && regen >= 0)
    {
        float total_power;
        HV_READ_ONLY_ACTION({
            if (!hv_get_info(hv_read_ptr, HV_TOTAL_POWER, &total_power, sizeof(total_power)))
            {
                powerControl(total_power, GIEI.limit_power, posTorquesNM);
            }
        })
    }

    const struct RegenAlgInput input =
    {
        .rear_left_velocity = engine_get_info(REAR_LEFT, ENGINE_VOLTAGE),
        .rear_right_velocity = engine_get_info(REAR_RIGHT, ENGINE_VOLTAGE),
        .front_left_velocity = engine_get_info(FRONT_LEFT, ENGINE_VOLTAGE),
        .front_right_velocity = engine_get_info(FRONT_RIGHT, ENGINE_VOLTAGE),
    };
    regen_alg_compute(&input, negTorquesNM);


    for (uint8_t i = 0; i < NUM_OF_EGINES; i++)
    {
        const float saturated_pos_torque_nm = 
            saturate_float(posTorquesNM[i], actual_max_pos_torque, 0.0f);
        const float saturated_neg_torque_nm = 
            saturate_float(negTorquesNM[i],0.0f, actual_max_neg_torque);
        const float posTorque = NMtoTorqueSetpoint(saturated_pos_torque_nm);
        const float negTorque = NMtoTorqueSetpoint(saturated_neg_torque_nm);

        if (posTorque > 0 && negTorque < 0)
        {
            engine_send_torque(i, 0, negTorque);
        }
        else
        {
            engine_send_torque(i, posTorque, negTorque);
        }
    }

    return 0;
}

uint8_t GIEI_get_speed(void)
{
    return giei_speed_alg_get_speed();
}

float GIEI_get_info(const struct GIEI* const restrict self, const enum GIEI_INFO info)
{
    switch (info) {
        case GIEI_INFO_LIMIT_POWER:
            return self->limit_power;
        case GIEI_INFO_MAX_SPEED:
            return self->limit_max_speed;
        case GIEI_INFO_MAX_POS_TORQUE:
            return engine_max_pos_torque(self->limit_pos_torque);
        case GIEI_INFO_MAX_NEG_TORQUE:
            return engine_max_neg_torque(self->limit_pos_torque);
        case GIEI_INFO_LIMIT_REGEN:
            return self->limit_regen;
        case GIEI_INFO_TV:
            return self->activate_torque_vectoring;
        case GIEI_INFO_REAR_REPARTITION:
            return self->settings_power_repartition * 100;
        case GIEI_INFO_FRONT_REPARTITION:
            return (1.0f - self->settings_power_repartition) * 100;
        case GIEI_INFO_STATUS_HV:
            return engine_inverter_hv_status();
        case GIEI_INFO_STATUS_RF:
            return GIEI_check_running_condition() >= TS_READY;
        case GIEI_INFO_CURRENT_SPEED:
            return giei_speed_alg_get_speed();
            break;
    }
    return -1;
}

void GIEI_free_read_ptr(void)
{
    GIEI.read_ptr--;
}
void GIEI_free_write_ptr(void)
{
    GIEI.mut_ptr--;
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
