#include "./board_can.h"
#include "../board_conf/id_conf.h"
#include "../lib/board_dbc/can1.h"
#include "../lib/board_dbc/can2.h"
#include <stdint.h>

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
            //TODO: implement manager for can inverter messages
            break;
        case CAN_MODULE_GENERAL:
            //TODO: implement manager for can general messages
            break;
        case CAN_MODULE_DV:
            //TODO: implement manager for can dv messages
            break;
        default:
            return -1;
    
    }
}
