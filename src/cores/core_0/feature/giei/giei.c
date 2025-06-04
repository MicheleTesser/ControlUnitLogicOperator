#include "giei.h"
#include "../../../core_utility/core_utility.h"
#include "../../../../lib/raceup_board/raceup_board.h"
#include "../driver_input/driver_input.h"
#include "../maps/maps.h"
#include "../../../core_utility/imu/imu.h"
#include "giei_components/giei_components.h"
#include "giei_components/hv/hv.h"
#include "giei_components/speed_alg/speed_alg.h"
#include "../math_saturated/saturated.h"

#include <stdint.h>

//private

#define M_N                                 9.8f

struct Giei_t{
    Hv_h hv;
    time_var_microseconds rtd_sound_start;
    RtdAssiSound_h o_rtd_sound;
    GpioRead_h gpio_rtd_button;
    Gpio_h m_gpio_rtd_button_led;
    MissionLocker_h o_mission_locker;
    float engines_voltages[__NUM_OF_ENGINES__];
    enum RUNNING_STATUS running_status;
    AsNodeRead_h m_as_node_read;
    EngineType* inverter;
    const DriverInput_h* driver_input;
    const DrivingMaps_h* driving_maps;
    const Imu_h* imu;
    uint8_t entered_rtd : 1;
};

union Giei_conv{
    Giei_h* const hidden;
    struct Giei_t* const clear;
};

union Giei_conv_const{
    const Giei_h* const hidden;
    const struct Giei_t* const clear;
};

#define GIEI_H_T_CONV(h_ptr, t_ptr_name)\
    union Giei_conv __g_conv_##t_ptr_name##__ = {h_ptr};\
    struct Giei_t* t_ptr_name = __g_conv_##t_ptr_name##__.clear;

#ifdef DEBUG
char __giei_size_check[(sizeof(Giei_h) == sizeof(struct Giei_t))? 1 : -1];
char __giei_align_check[(_Alignof(Giei_h) == _Alignof(struct Giei_t))? 1 : -1];
#endif /* ifdef DEBUG */

static inline float NMtoTorqueSetpoint(const float torqueNM)
{
    return (torqueNM/M_N)*1000;
}

static inline float torqueSetpointToNM(const float setpoint)
{
    return (setpoint/1000.0f)*M_N;
}

static void update_torque_NM_vectors_no_tv(
        const struct Giei_t* const restrict self,
        const float throttle, float posTorquesNM[__NUM_OF_ENGINES__], const float actual_max_pos_torque)
{
    const float power_repartition = driving_map_get_parameter(self->driving_maps, TORQUE_REPARTITION);
    const float probability_of_success = power_repartition/ (1 - power_repartition);
    for (uint8_t i = 0; i < __NUM_OF_ENGINES__; i++)
    {
        /*
         * 0 - 100 throttle and brake to Nm (setpoint is relative to nominal torque and goes in 0.1% that's why there is * 10)
         * Setpoints are scaled to obtain desired torque repartition that inverts during braking
         * Negative torques are computed in regBrake() 
         */
        float setpoint =  throttle* (actual_max_pos_torque/M_N) * 10;
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

int8_t giei_init(Giei_h* const restrict self,
        EngineType* const engine,
        const DriverInput_h* const p_driver,
        const DrivingMaps_h* const p_maps,
        const Imu_h* const p_imu)
{
    GIEI_H_T_CONV(self, p_self);

    if (hv_init(&p_self->hv)<0) {
        SET_TRACE(CORE_0);
        return -1;
    }

    if (hardware_init_read_permission_gpio(&p_self->gpio_rtd_button, GPIO_RTD_BUTTON)<0)
    {
        SET_TRACE(CORE_0);
        return -2;
    }

    while(hardware_init_gpio(&p_self->m_gpio_rtd_button_led, GPIO_RTD_BUTTON_LED)<0)
    {
        SET_TRACE(CORE_0);
        return -3;
    }

    rtd_assi_sound_init(&p_self->o_rtd_sound);

    if (lock_mission_ref_get_mut(&p_self->o_mission_locker)<0)
    {
      SET_TRACE(CORE_0);
      return -4;
    }

    if (as_node_read_init(&p_self->m_as_node_read)<0)
    {
      SET_TRACE(CORE_0);
      return -5;
    }

    speed_alg_init();
    regen_alg_init();
    tv_alg_init();

    p_self->inverter = engine;
    p_self->driver_input = p_driver;
    p_self->driving_maps = p_maps;
    p_self->imu = p_imu;

    //INFO: sharing var:
    //  - posTorquesNM
    //  - negTorquesNM.
    //  - engines_voltages
    //
    //ranges min, max included
    //pos: 0..3
    //neg: 4..7
    //engines engines_voltages: 8..11

    // for (uint8_t i=0; i<__NUM_OF_ENGINES__; i++)
    // {
    //   if(external_log_variables_store_pointer(p_self->engines_voltages, (2 * __NUM_OF_ENGINES__) + i))return -7;
    // }


    return 0;
}

int8_t giei_update(Giei_h* const restrict self)
{
  GIEI_H_T_CONV(self, p_self);

  if (hv_update(&p_self->hv)<0)
  {
    SET_TRACE(CORE_0);
    return -1;
  }

  if(inverter_update(p_self->inverter)<0)
  {
    SET_TRACE(CORE_0);
    return -2;
  }

  return 0;
}

enum RUNNING_STATUS GIEI_check_running_condition(Giei_h* const restrict self)
{
    GIEI_H_T_CONV(self, p_self);
    enum RUNNING_STATUS rt = SYSTEM_OFF;

    ACTION_ON_FREQUENCY(p_self->rtd_sound_start, get_tick_from_millis(3000))
    {
      rtd_assi_sound_stop(&p_self->o_rtd_sound);
    }
    if (as_node_read_get_status(&p_self->m_as_node_read))
    {
      rt = engine_rtd_procedure(p_self->inverter);
    }

    if (rt > SYSTEM_OFF)
    {
      lock_mission(&p_self->o_mission_locker);
    }
    else
    {
      unlock_mission(&p_self->o_mission_locker);
    }
    if (rt == RUNNING && !p_self->entered_rtd)
    {
        p_self->entered_rtd =1;
        gpio_set_high(&p_self->m_gpio_rtd_button_led);
        rtd_assi_sound_start(&p_self->o_rtd_sound);
        p_self->rtd_sound_start = timer_time_now();
    }
    else if (rt != RUNNING)
    {
        p_self->entered_rtd =0;
        p_self->rtd_sound_start=0;
        gpio_set_low(&p_self->m_gpio_rtd_button_led);
        rtd_assi_sound_stop(&p_self->o_rtd_sound);
    }
    p_self->running_status = rt;
    return rt;

}

int8_t GIEI_compute_power(Giei_h* const restrict self)
{
    const union Giei_conv conv = {self};
    struct Giei_t* const restrict p_self = conv.clear;

    const float regen = giei_driver_input_get(p_self->driver_input, BRAKE);
    const float limit_power = driving_map_get_parameter(p_self->driving_maps, POWER_KW);
    const float driver_throttle = giei_driver_input_get(p_self->driver_input, THROTTLE);
    const float driver_sterring_angle = giei_driver_input_get(p_self->driver_input, STEERING_ANGLE);
    const float imu_acc_x = imu_get_acc(p_self->imu, AXES_X);
    const float imu_acc_y = imu_get_acc(p_self->imu, AXES_Y);
    const float imu_acc_z = imu_get_acc(p_self->imu, AXES_Z);
    const float giei_max_pos_torque =
        driving_map_get_parameter(p_self->driving_maps, MAX_POS_TORQUE);

    const float giei_max_neg_torque =
        driving_map_get_parameter(p_self->driving_maps, MAX_POS_TORQUE);

    const float actual_max_pos_torque =
        engine_max_pos_torque(p_self->inverter, giei_max_pos_torque);

    const float actual_max_neg_torque =
        engine_max_neg_torque(p_self->inverter, giei_max_neg_torque);

    struct{
      float pos[__NUM_OF_ENGINES__];
      float neg[__NUM_OF_ENGINES__];
    }TorquesNM = {0};


    p_self->engines_voltages[FRONT_LEFT] = engine_get_info(p_self->inverter, FRONT_LEFT,ENGINE_VOLTAGE);
    p_self->engines_voltages[FRONT_RIGHT] = engine_get_info(p_self->inverter, FRONT_RIGHT,ENGINE_VOLTAGE);
    p_self->engines_voltages[REAR_LEFT] = engine_get_info(p_self->inverter, REAR_LEFT,ENGINE_VOLTAGE);
    p_self->engines_voltages[REAR_RIGHT] = engine_get_info(p_self->inverter, REAR_RIGHT,ENGINE_VOLTAGE);


    if ((uint8_t)driving_map_get_parameter(p_self->driving_maps, TV_ON))
    {
        struct TVInputArgs tv_input =
        {
            .ax = imu_acc_x,
            .ay = imu_acc_y,
            .yaw_r = imu_acc_z,
            .throttle = driver_throttle,
            .steering = driver_sterring_angle,
            .rpm[0] = engine_get_info(p_self->inverter, FRONT_LEFT, ENGINE_RPM),
            .rpm[1] = engine_get_info(p_self->inverter, FRONT_RIGHT, ENGINE_RPM),
            .rpm[2] = engine_get_info(p_self->inverter, REAR_LEFT, ENGINE_RPM),
            .rpm[3] = engine_get_info(p_self->inverter, REAR_RIGHT, ENGINE_RPM),
        };
        tv_alg_compute(&tv_input, TorquesNM.pos);
    }
    else
    {
        update_torque_NM_vectors_no_tv(p_self,driver_throttle, TorquesNM.pos, actual_max_pos_torque);
    }

    hv_computeBatteryPackTension(&p_self->hv, p_self->engines_voltages, __NUM_OF_ENGINES__);

    if (driver_throttle> 0 && regen >= 0)
    {
        const float total_power = hv_get_info(&p_self->hv, HV_TOTAL_POWER);
        powerControl(total_power, limit_power, TorquesNM.pos);
    }

    const struct RegenAlgInput input =
    {
        .rear_left_velocity = engine_get_info(p_self->inverter, REAR_LEFT, ENGINE_VOLTAGE),
        .rear_right_velocity = engine_get_info(p_self->inverter, REAR_RIGHT, ENGINE_VOLTAGE),
        .front_left_velocity = engine_get_info(p_self->inverter, FRONT_LEFT, ENGINE_VOLTAGE),
        .front_right_velocity = engine_get_info(p_self->inverter, FRONT_RIGHT, ENGINE_VOLTAGE),
    };
    regen_alg_compute(&input, TorquesNM.neg);


    for (uint8_t i = 0; i < __NUM_OF_ENGINES__; i++)
    {
        const float saturated_pos_torque_nm = 
            saturate_float(TorquesNM.pos[i], actual_max_pos_torque, 0.0f);
        const float saturated_neg_torque_nm = 
            saturate_float(TorquesNM.neg[i],0.0f, actual_max_neg_torque);
        const float posTorque = NMtoTorqueSetpoint(saturated_pos_torque_nm);
        const float negTorque = NMtoTorqueSetpoint(saturated_neg_torque_nm);

        if (posTorque > 0 && negTorque < 0)
        {
            engine_send_torque(p_self->inverter, i, 0, negTorque);
        }
        else
        {
            engine_send_torque(p_self->inverter, i, posTorque, negTorque);
        }
    }

    return 0;
}
