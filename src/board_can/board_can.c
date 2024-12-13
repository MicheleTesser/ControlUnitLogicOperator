#include "./board_can.h"
#include "../board_conf/id_conf.h"
#include "../lib/board_dbc/can1.h"
#include "../lib/board_dbc/can2.h"
#include <stdint.h>
#include <string.h>

//private

//FIX: add an abstract atomic operation
static uint8_t mex_to_read[3];


static void inverter_can_interrupt(void)
{
    mex_to_read[0] = 1;
}

static void general_can_interrupt(void)
{
    mex_to_read[1] = 1;
}

static void dv_can_interrupt(void)
{
    mex_to_read[2] = 1;
}

static int8_t manage_can_1_message(const CanMessage* const restrict mex){
    //TODO: implement manager for can inverter messages
    switch (mex->id) {
        case CAN_ID_VCUINVFL:
        case CAN_ID_VCUINVFR:    
        case CAN_ID_VCUINVRL: 
        case CAN_ID_VCUINVRR: 
        case CAN_ID_INVERTERFL1:
        case CAN_ID_INVERTERFR1:
        case CAN_ID_INVERTERFL2:
        case CAN_ID_INVERTERFR2:
        case CAN_ID_INVERTERRL1:
        case CAN_ID_INVERTERRR1:
        case CAN_ID_INVERTERRL2:
        case CAN_ID_INVERTERRR2:
        case CAN_ID_VECTOR__INDEPENDENT_SIG_MSG:
        default:
            return -1;
    }
    return 0;
}

static int8_t manage_can_2_message(const CanMessage* const restrict mex){
    //TODO: implement manager for can general messages
    
    switch (mex->id) {
        case CAN_ID_PADDLE:
        case CAN_ID_DRIVER:
        case CAN_ID_BMSLV1:
        case CAN_ID_BMSLV2:
        case CAN_ID_BMSHV1:
        case CAN_ID_BMSHV2:
        case CAN_ID_IMU1:
        case CAN_ID_IMU2:
        case CAN_ID_IMU3:
        case CAN_ID_IMUCALIB:
        case CAN_ID_MAP:
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
            return -1;
    
    }
    

    return 0;
}

static int8_t manage_can_3_message(const CanMessage* const restrict mex){
    //TODO: implement manager for can dv messages
    
    switch (mex->id) {
        default:
            return -1;
    }
    return 0;
}

//public
int8_t board_can_init(uint8_t can_id, enum CAN_FREQUENCY freq)
{
    if(hardware_init_can(can_id, freq) < 0){
        return -1;
    }

    switch (can_id) {
        case CAN_MODULE_INVERTER:
            if(hardware_interrupt_attach_fun(0, inverter_can_interrupt)){
                return -2;
            }
            break;
        case CAN_MODULE_GENERAL:
            if(hardware_interrupt_attach_fun(1, general_can_interrupt)){
                return -2;
            }
            break;
        case CAN_MODULE_DV:
            if(hardware_interrupt_attach_fun(2, dv_can_interrupt)){
                return -2;
            }
            break;
        default:
            return -1;
    }


    return 0;
}

int8_t board_can_read(const uint8_t can_id,CanMessage* const restrict o_mex)
{
    memset(o_mex, 0, sizeof(*o_mex));
    int8_t mex_to_read_t = -1;
    switch (can_id) {
        case CAN_MODULE_INVERTER:
            if (mex_to_read[0]) mex_to_read_t = 0;
            break;
        case CAN_MODULE_GENERAL:
            if (mex_to_read[1]) mex_to_read_t = 1;
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

    if(hardware_read_can(mex_to_read_t, o_mex) <0){
        return -1;
    }

    mex_to_read[mex_to_read_t] = 0;

    return 0;
}

int8_t board_can_write(const uint8_t can_id, const CanMessage* const restrict o_mex)
{
    return hardware_write_can(can_id, o_mex);
}

int8_t board_can_manage_message(const uint8_t can_id, const CanMessage* const restrict mex)
{
    switch (can_id) {
        case CAN_MODULE_INVERTER:
            return manage_can_1_message(mex);
        case CAN_MODULE_GENERAL:
            return manage_can_2_message(mex);
            break;
        case CAN_MODULE_DV:
            return manage_can_3_message(mex);
            break;
        default:
            return -1;
    
    }
}
