#include "giei.h"
#include "../../../lib/raceup_board/raceup_board.h"
#include "../../../emergency_module/emergency_module.h"
#include "../../../driver_input/driver_input.h"
#include "../../../imu/imu.h"
#include "giei_components/torque_vec_alg/torque_vec_alg.h"
#include "giei_components/regen_alg/regen_alg.h"
#include "giei_components/power_control/power_control.h"
#include "giei_components/giei_hv/giei_hv.h"
#include "giei_components/maps/maps.h"
#include "giei_components/engines/engines.h"
#include "math_saturated/saturated.h"

#include <stdint.h>
#include <string.h>

//private

#define M_N                                 9.8f

struct Giei_t{
    EngineType inverter;
    time_var_microseconds rtd_sound_start;
    enum RUNNING_STATUS running_status;
    const DriverInput_h* driver_input;
    DrivingMaps_h driving_maps;
    Imu_h imu;
    GieiHv_h hv;
    const EmergencyNode* giei_emergency;
    uint8_t entered_rtd : 1;
};

union Giei_conv{
    struct Giei_h* hidden;
    struct Giei_t* clear;
};

#define GIEI_H_T_CONV(h_ptr, t_ptr_name)\
    union Giei_conv __g_conv_##t_ptr_name##__ = {h_ptr};\
    struct Giei_t* t_ptr_name = __g_conv_##t_ptr_name##__.clear;

#ifdef DEBUG
const uint8_t giei_size_check[(sizeof(struct Giei_h) == sizeof(struct Giei_t))? 1 : -1];
#endif /* ifdef DEBUG */

static inline int NMtoTorqueSetpoint(const float torqueNM)
{
    return (torqueNM/M_N)*1000;
}

static float torqueSetpointToNM(const int setpoint)
{
    return (setpoint/1000.0)*M_N;
}

static void update_torque_NM_vectors_no_tv(
        const struct Giei_t* const restrict self,
        const float throttle, float posTorquesNM[NUM_OF_EGINES], const float actual_max_pos_torque)
{
    const float power_repartition = driving_map_get_parameter(&self->driving_maps, TORQUE_REPARTITION);
    const float probability_of_success = power_repartition/ (1 - power_repartition);
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

int8_t giei_init(Giei_h* const restrict self, const DriverInput_h* const p_driver)
{
    int8_t err=0;
    GIEI_H_T_CONV(self, p_self);
    p_self->driver_input = p_driver;
    if(inverter_module_init(&p_self->inverter, p_self->driver_input) <0)
    {
        goto amk_fail;
    }
    p_self->giei_emergency = EmergencyNode_new(); //TODO: set the proper number
    if (p_self->giei_emergency)
    {
        goto emergency_node_fail;
    }
    
    if (driving_maps_init(&p_self->driving_maps) < 0)
    {
        goto driving_maps_fail;
    }

    if (imu_init(&p_self->imu) < 0) { //TODO: set the proper mailbox
        goto imu_fail;
    }

    if (giei_hv_init(&p_self->hv) < 0) {
        goto giei_hv_fail;
    }

    regen_alg_init();
    tv_alg_init();

    return 0;

giei_hv_fail:
    err--;
imu_fail:
    err--;
driving_maps_fail:
    err--;
emergency_node_fail:
    engine_destroy(&p_self->inverter);
    err--;
amk_fail:
    err--;
    
    return err;
}
enum RUNNING_STATUS GIEI_check_running_condition(struct Giei_h* const restrict self)
{
    GIEI_H_T_CONV(self, p_self);
    enum RUNNING_STATUS rt = SYSTEM_OFF;

    if ((timer_time_now() - p_self->rtd_sound_start) > 3 SECONDS)
    {
        gpio_set_high(GPIO_RTD_SOUND);
        gpio_set_high(GPIO_RTD_BUTTON);
    }
    rt = engine_rtd_procedure(&p_self->inverter);
    if (rt == RUNNING && !p_self->entered_rtd)
    {
        p_self->entered_rtd =1;
        gpio_set_low(GPIO_RTD_SOUND);
        gpio_set_low(GPIO_RTD_BUTTON);
        p_self->rtd_sound_start = timer_time_now();
    }
    else if (rt != RUNNING)
    {
        p_self->entered_rtd =0;
        gpio_set_high(GPIO_RTD_SOUND);
        gpio_set_high(GPIO_RTD_BUTTON);
    }
    p_self->running_status = rt;
    return rt;

}

int8_t GIEI_input(struct Giei_h* const restrict self, const float throttle, const float regen)
{
    GIEI_H_T_CONV(self, p_self);

    const float limit_power = driving_map_get_parameter(&p_self->driving_maps, POWER_KW);
    const float driver_regen = driver_get_amount(p_self->driver_input, REGEN);
    const float driver_throttle = driver_get_amount(p_self->driver_input, THROTTLE);
    const float driver_sterring_angle = driver_get_amount(p_self->driver_input, STEERING_ANGLE);
    const float imu_acc_x = imu_get_data(&p_self->imu, IMU_ACCELERATION, AXES_X);
    const float imu_acc_y = imu_get_data(&p_self->imu, IMU_ACCELERATION, AXES_Y);
    const float imu_acc_z = imu_get_data(&p_self->imu, IMU_ACCELERATION, AXES_Z);
    const float giei_max_pos_torque =
        driving_map_get_parameter(&p_self->driving_maps, MAX_POS_TORQUE);

    const float giei_max_neg_torque =
        driving_map_get_parameter(&p_self->driving_maps, MAX_POS_TORQUE);

    const float actual_max_pos_torque =
        engine_max_pos_torque(&p_self->inverter, giei_max_pos_torque);

    const float actual_max_neg_torque =
        engine_max_neg_torque(&p_self->inverter, giei_max_neg_torque);

    float posTorquesNM[NUM_OF_EGINES];
    float negTorquesNM[NUM_OF_EGINES];
    float engines_voltages[NUM_OF_EGINES];
    memset(posTorquesNM, 0, sizeof(posTorquesNM));
    memset(negTorquesNM, 0, sizeof(negTorquesNM));
    memset(engines_voltages, 0, sizeof(engines_voltages));

    engines_voltages[FRONT_LEFT] = engine_get_info(&p_self->inverter, FRONT_LEFT,ENGINE_VOLTAGE);
    engines_voltages[FRONT_RIGHT] = engine_get_info(&p_self->inverter, FRONT_RIGHT,ENGINE_VOLTAGE);
    engines_voltages[REAR_LEFT] = engine_get_info(&p_self->inverter, REAR_LEFT,ENGINE_VOLTAGE);
    engines_voltages[REAR_RIGHT] = engine_get_info(&p_self->inverter, REAR_RIGHT,ENGINE_VOLTAGE);


    if (driving_map_get_parameter(&p_self->driving_maps, TV_ON))
    {
        struct TVInputArgs tv_input =
        {
            .ax = imu_acc_x,
            .ay = imu_acc_y,
            .yaw_r = imu_acc_z,
            .throttle = driver_throttle,
            .regenpaddle = driver_regen,
            .steering = driver_sterring_angle,
            .brakepressurefront = 0, //TODO: not yet implemented in the code
            .brakepressurerear = 0, //TODO: not yet implemented in the code
            .voltage = giei_hv_get_info(&p_self->hv, HV_BATTERY_PACK_TENSION),
            .rpm[0] = engine_get_info(&p_self->inverter, FRONT_LEFT, ENGINE_RPM),
            .rpm[1] = engine_get_info(&p_self->inverter, FRONT_RIGHT, ENGINE_RPM),
            .rpm[2] = engine_get_info(&p_self->inverter, REAR_LEFT, ENGINE_RPM),
            .rpm[3] = engine_get_info(&p_self->inverter, REAR_RIGHT, ENGINE_RPM),
        };
        tv_alg_compute(&tv_input, posTorquesNM);
    }
    else
    {
        update_torque_NM_vectors_no_tv(p_self,throttle, posTorquesNM, actual_max_pos_torque);
    }

    giei_hv_computeBatteryPackTension(&p_self->hv, engines_voltages, NUM_OF_EGINES);

    if (throttle > 0 && regen >= 0)
    {
        const float total_power = giei_hv_get_info(&p_self->hv, HV_TOTAL_POWER);
        powerControl(total_power, limit_power, posTorquesNM);
    }

    const struct RegenAlgInput input =
    {
        .rear_left_velocity = engine_get_info(&p_self->inverter, REAR_LEFT, ENGINE_VOLTAGE),
        .rear_right_velocity = engine_get_info(&p_self->inverter, REAR_RIGHT, ENGINE_VOLTAGE),
        .front_left_velocity = engine_get_info(&p_self->inverter, FRONT_LEFT, ENGINE_VOLTAGE),
        .front_right_velocity = engine_get_info(&p_self->inverter, FRONT_RIGHT, ENGINE_VOLTAGE),
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
            engine_send_torque(&p_self->inverter, i, 0, negTorque);
        }
        else
        {
            engine_send_torque(&p_self->inverter, i, posTorque, negTorque);
        }
    }

    return 0;
}
