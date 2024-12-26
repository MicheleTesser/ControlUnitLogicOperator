#include "./board_can.h"
#include "../board_conf/can/can.h"
#include "../board_conf/interrupt/interrupt.h"
#include "../board_conf/trap/trap.h"
#include "../lib/board_dbc/dbc/out_lib/can1/can1.h"
#include "../lib/board_dbc/dbc/out_lib/can2/can2.h"
#include "../lib/board_dbc/dbc/out_lib/can3/can3.h"
#include "../lib/DPS/dps_slave.h"
#include "../GIEI/giei.h"
#include "../IMU/imu.h"
#include "../driver_input/driver_input.h"
#include "../cooling/temperatures/temperatures.h"
#include "../cooling/fans/fans.h"
#include "../suspensions/suspensions.h"
#include "../batteries/batteries.h"
#include <stdint.h>
#include <string.h>

//private

//FIX: add an abstract atomic operation
static uint8_t mex_to_read[3];


static inline void inverter_can_interrupt(void) INTERRUP_ATTRIBUTE 
{
    mex_to_read[0]++;
}

static inline void general_can_interrupt(void) INTERRUP_ATTRIBUTE 
{
    mex_to_read[1]++;
}

static inline void dv_can_interrupt(void) INTERRUP_ATTRIBUTE 
{
    mex_to_read[2]++;
}

static int dps_send(DPSCanMessage* dps_mex){
    CanMessage mex;
    memset(&mex, 0, sizeof(mex));
    mex.full_word = dps_mex->GenericPayload.rawMex.full_buffer[0];
    mex.id = dps_mex->id;
    mex.message_size = dps_mex->dlc;

    if(board_can_write(CAN_MODULE_GENERAL, &mex) < 0){
        return -1;
    }
    return 0;
}

static int8_t manage_can_1_message(const CanMessage* const restrict mex){
    switch (mex->id) {
        case CAN_ID_INVERTERFL1:
        case CAN_ID_INVERTERFL2:
        case CAN_ID_INVERTERFR1:
        case CAN_ID_INVERTERFR2:
        case CAN_ID_INVERTERRL1:
        case CAN_ID_INVERTERRL2:
        case CAN_ID_INVERTERRR1:
        case CAN_ID_INVERTERRR2:
            return GIEI_recv_data(mex);
        default:
            return -1;
    }
}

static int8_t manage_can_2_message(const CanMessage* const restrict mex)
{
    union u32_flot{
        uint32_t u_data;
        float f_data;
    };
    can_obj_can2_h_t m;
    unpack_message_can2(&m, mex->id, mex->full_word, mex->message_size, 0);
    switch (mex->id) {
        case CAN_ID_PADDLE: //INFO: SW
            driver_set_amount(REGEN, m.can_0x052_Paddle.regen);
            break;
        case CAN_ID_DRIVER: //INFO: STW
            driver_set_amount(THROTTLE, m.can_0x053_Driver.throttle);
            driver_set_amount(BRAKE, m.can_0x053_Driver.brake);
            driver_set_amount(STEERING_ANGLE, m.can_0x053_Driver.steering);
            if (!m.can_0x053_Driver.no_implausibility) {
                set_implausibility(THROTTLE_BRAKE,m.can_0x053_Driver.bre_implausibility);
                set_implausibility(THROTTLE_PADEL,m.can_0x053_Driver.pad_implausibility);
                set_implausibility(THROTTLE_POT,m.can_0x053_Driver.pot_implausibility);
            }else{
                clear_implausibility();
            }
            break;
        case CAN_ID_BMSLV1:
            lv_update_status(mex);
            break;
        case CAN_ID_BMSLV2:
            lv_update_status(mex);
            save_temperature(BMS_LV_1, m.can_0x055_BmsLv2.temp1);
            save_temperature(BMS_LV_2, m.can_0x055_BmsLv2.temp2);
            break;
        case CAN_ID_BMSHV1:
            hv_update_status(mex);
            break;
        case CAN_ID_BMSHV2:
            hv_update_status(mex);
            save_temperature(BMS_HV_MIN, m.can_0x058_BmsHv2.min_temp);
            save_temperature(BMS_HV_MAX, m.can_0x058_BmsHv2.max_temp);
            save_temperature(BMS_HV_AVG, m.can_0x058_BmsHv2.avg_temp);
            fan_set_value(FAN_BMS_HV, m.can_0x058_BmsHv2.fan_speed);
            break;
        case CAN_ID_IMU1:
            {
                union u32_flot conv ={
                    .u_data = m.can_0x060_Imu1.acc_x,
                };
                conv.u_data = m.can_0x060_Imu1.acc_x;
                imu_update_info(IMU_accelerations, axis_X, conv.f_data);
                conv.u_data = m.can_0x060_Imu1.acc_y;
                imu_update_info(IMU_accelerations, axis_Y, conv.f_data);
            }
            break;
        case CAN_ID_IMU2:
            {
                union u32_flot conv ={
                    .u_data = m.can_0x061_Imu2.acc_z,
                };
                imu_update_info(IMU_accelerations, axis_Z, conv.f_data);
                imu_calibrate();
                conv.u_data = m.can_0x061_Imu2.omega_x;
                imu_update_info(IMU_angles, axis_X, conv.f_data);
            }
            break;
        case CAN_ID_IMU3:
            {
                union u32_flot conv ={
                    .u_data = m.can_0x062_Imu3.omega_y,
                };
                imu_update_info(IMU_angles, axis_Y, conv.f_data);
                conv.u_data = m.can_0x062_Imu3.omega_z;
                imu_update_info(IMU_angles, axis_Z, conv.f_data);
            }
            break;
        case CAN_ID_MAP: //INFO: STW
            giei_set_run_map(MAP_POWER, m.can_0x064_Map.power);
            giei_set_run_map(MAP_REGEN, m.can_0x064_Map.regen);
            giei_set_run_map(MAP_POWER_REPARTITION, m.can_0x064_Map.torque_rep);
            hardware_raise_trap(TRAP_CHANGE_MAP);
            break;
        case CAN_ID_LAPSTART:
            //TODO: not yet implemented
            break;
        case CAN_ID_TEMP1: //INFO: smu
            save_temperature(ENGINE_PRE_L, m.can_0x100_Temp1.temp_motor_pre_L);
            save_temperature(ENGINE_POST_L, m.can_0x100_Temp1.temp_motor_post_L);
            save_temperature(ENGINE_PRE_R, m.can_0x100_Temp1.temp_motor_pre_R);
            save_temperature(COLDPLATE_PRE_R, m.can_0x100_Temp1.temp_coldplate_pre_R);
            break;
        case CAN_ID_TEMP2: //INFO: smu
            save_temperature(COLDPLATE_PRE_L, m.can_0x101_Temp2.temp_cold_pre_L);
            save_temperature(COLDPLATE_POST_R, m.can_0x101_Temp2.temp_mot_post_R);
            save_temperature(COLDPLATE_POST_L, m.can_0x101_Temp2.temp_cold_post_L);
            save_temperature(COLDPLATE_POST_R, m.can_0x101_Temp2.temp_cold_post_R);
            break;
        case CAN_ID_SUSPREAR: //INFO: smu
            suspensions_save(SUSP_REAR_LEFT,m.can_0x102_SuspRear.susp_rl);
            suspensions_save(SUSP_REAR_RIGHT,m.can_0x102_SuspRear.susp_rr);
            break;
        case CAN_ID_SUSPFRONT: //INFO: smu
            suspensions_save(SUSP_FRONT_LEFT, m.can_0x104_SuspFront.susp_fl);
            suspensions_save(SUSP_FRONT_RIGHT, m.can_0x104_SuspFront.susp_fr);
            break;
        case CAN_ID_TEMPFRONTR: //INFO: atc
            save_temperature(ENGINE_POT_FRONT_RIGHT, m.can_0x105_TempFrontR.temp_mot_pot_FR);
            save_temperature(ENGINE_PRE_FRONT_RIGHT, m.can_0x105_TempFrontR.temp_mot_pre_FR);
            break;
        case CAN_ID_LEM: //INFO: lem
            return hv_update_status(mex);
            break;
        default:
            return -1;
    }
    

    return 0;
}

static int8_t manage_can_3_message(const CanMessage* const restrict mex){
    switch (mex->id) {
        case CAN_ID_DV_DRIVING_DYNAMICS_1:
            //TODO: not yet implemented
            break;
        case CAN_ID_DV_DRIVING_DYNAMICS_2:
            //TODO: not yet implemented
            break;
        case CAN_ID_DV_SYSTEM_STATUS:
            //TODO: not yet implemented
            break;
        default:
            return -1;
    }
    return 0;
}

//public
int8_t board_can_init(uint8_t can_id, enum CAN_FREQUENCY freq)
{
    static uint8_t dps_init_flag = 0;
    int8_t err=0;

    if(hardware_init_can(can_id, freq) < 0){
        goto hardware_init_failed;
    }

    switch (can_id) {
        case CAN_MODULE_INVERTER:
            if(hardware_interrupt_attach_fun(INTERRUPT_CAN_1, inverter_can_interrupt)){
                goto hardware_interrupt_attach_fail;
            }
            break;
        case CAN_MODULE_GENERAL:
            if(hardware_interrupt_attach_fun(INTERRUPT_CAN_2, general_can_interrupt)){
                goto hardware_interrupt_attach_fail;
            }
            break;
        case CAN_MODULE_DV:
            if(hardware_interrupt_attach_fun(INTERRUPT_CAN_3, dv_can_interrupt)){
                goto hardware_interrupt_attach_fail;
            }
            break;
        default:
            goto invalid_can_module;
    }

    BoardName board_name = {
        .full_data.name = "CULO",
    };
    while(!dps_init_flag && dps_init(dps_send, &board_name)){}
    dps_init_flag=1;

    return 0;

hardware_interrupt_attach_fail:
    err--;
invalid_can_module:
    err--;
hardware_init_failed:
    err--;

    return err;
}

int8_t board_can_read(const uint8_t can_id, CanMessage* const restrict o_mex)
{
    int8_t mex_to_read_t = -1;
    memset(o_mex, 0, sizeof(*o_mex));
    switch (can_id) {
        case CAN_MODULE_INVERTER:
            if (mex_to_read[0]){
                mex_to_read_t = 0;
            }
            break;
        case CAN_MODULE_GENERAL:
            if (mex_to_read[1]){
                mex_to_read_t = 1;
            }
            break;
        case CAN_MODULE_DV:
            if (mex_to_read[2]) mex_to_read_t = 2;
            break;
        default:
            return -1;
    }
    if (mex_to_read_t == -1) {
        return -2;
    }

    mex_to_read[mex_to_read_t]--;
    return hardware_read_can(mex_to_read_t, o_mex);
}

int8_t board_can_write(const uint8_t can_id, const CanMessage* const restrict mex)
{
    return hardware_write_can(can_id, mex);
}

int8_t board_can_manage_message(const uint8_t can_id, const CanMessage* const restrict mex)
{
    switch (can_id) {
        case CAN_MODULE_INVERTER:
            return manage_can_1_message(mex);
        case CAN_MODULE_GENERAL:
            return manage_can_2_message(mex);
        case CAN_MODULE_DV:
            return manage_can_3_message(mex);
        default:
            return -1;
    }
}
