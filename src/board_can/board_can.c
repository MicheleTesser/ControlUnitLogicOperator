#include "./board_can.h"
#include "../board_conf/id_conf.h"
#include "../lib/board_dbc/can1.h"
#include "../lib/board_dbc/can2.h"
#include "../lib/board_dbc/can3.h"
#include "../lib/DPS/dps_slave.h"
#include "../GIEI/giei.h"
#include "../driver_input/driver_input.h"
#include <stdint.h>
#include <string.h>

//private

//FIX: add an abstract atomic operation
static uint8_t mex_to_read[3];


static void inverter_can_interrupt(void) INTERRUP_ATTRIBUTE 
{
    mex_to_read[0]++;
}

static void general_can_interrupt(void) INTERRUP_ATTRIBUTE 
{
    mex_to_read[1]++;
}

static void dv_can_interrupt(void) INTERRUP_ATTRIBUTE 
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
    int8_t err=0;
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
            goto invalid_inverter_message;
    }
    return 0;

invalid_inverter_message:
    err--;

    return err;
}

static int8_t manage_can_2_message(const CanMessage* const restrict mex)
{
    int8_t err=0;
    can_obj_can2_h_t m;
    unpack_message_can2(&m, mex->id, mex->full_word, mex->message_size, 0);
    switch (mex->id) {
        case CAN_ID_PADDLE:
            driver_set_amount(REGEN, m.can_0x052_Paddle.regen);
            break;
        case CAN_ID_DRIVER:
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
        case CAN_ID_BMSLV2:
        case CAN_ID_BMSHV1:
        case CAN_ID_BMSHV2:
        case CAN_ID_IMU1:
        case CAN_ID_IMU2:
        case CAN_ID_IMU3:
        case CAN_ID_IMUCALIB:
        case CAN_ID_MAP:
            giei_set_run_map(MAP_POWER, m.can_0x064_Map.power);
            giei_set_run_map(MAP_REGEN, m.can_0x064_Map.regen);
            giei_set_run_map(MAP_POWER_REPARTITION, m.can_0x064_Map.torque_rep);
            hardware_raise_trap(TRAP_CHANGE_MAP);
            break;
        case CAN_ID_CARSTATUS:
        case CAN_ID_CARSETTINGS:
        case CAN_ID_LAPSTART:
        case CAN_ID_TEMP1:
        case CAN_ID_TEMP2:
        case CAN_ID_SUSPREAR:
        case CAN_ID_SUSPFRONT:
        case CAN_ID_TEMPFRONTR:
        case CAN_ID_INVVOLT:
        case CAN_ID_PCU:
        case CAN_ID_LEM:
        default:
            goto invalid_general_message;
    }
    

    return 0;

invalid_general_message:
    err--;

    return err;
}

static int8_t manage_can_3_message(const CanMessage* const restrict mex){
    int8_t err=0;
    switch (mex->id) {
        case CAN_ID_DV_DRIVING_DYNAMICS_1:
            break;
        case CAN_ID_DV_DRIVING_DYNAMICS_2:
            break;
        case CAN_ID_DV_SYSTEM_STATUS:
            break;
        default:
            goto invalid_dv_message;
    }
    return 0;

invalid_dv_message:
    err--;

    return err;
}

//public
int8_t board_can_init(uint8_t can_id, enum CAN_FREQUENCY freq)
{
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
    dps_init(dps_send, &board_name);

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
    int8_t err=0;
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
        return 0;
    }

    if(hardware_read_can(mex_to_read_t, o_mex) <0){
        goto hardware_failed_read_can;
    }


    mex_to_read[mex_to_read_t]--;

    return 0;

hardware_failed_read_can:
    err--;

    return err;
}

int8_t board_can_write(const uint8_t can_id, const CanMessage* const restrict mex)
{
    return hardware_write_can(can_id, mex);
}

int8_t board_can_manage_message(const uint8_t can_id, const CanMessage* const restrict mex)
{
    int8_t err=0;
    switch (can_id) {
        case CAN_MODULE_INVERTER:
            return manage_can_1_message(mex);
        case CAN_MODULE_GENERAL:
            return manage_can_2_message(mex);
        case CAN_MODULE_DV:
            return manage_can_3_message(mex);
        default:
            goto invalid_can_module_id;
            return -1;
    }

invalid_can_module_id:
    err--;

    return err;
}
