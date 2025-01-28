#include "hv.h"
#include "../../../../../../lib/raceup_board/components/can.h"
#include "../../../../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#include <stdint.h>
#include <string.h>

struct GieiHv_t{
    struct CanMailbox* lem_mailbox;
    struct CanMailbox* send_mailbox_bms_hv;
    float hv_public_data[__NUM_OF_GIEI_HV_INFO__];
    float lem_current;
};

union GieiHv_conv{
    struct Hv_h* const hidden;
    struct GieiHv_t* const clear;
};

union GieiHv_const_conv{
    const struct Hv_h* const  hidden;
    const struct GieiHv_t* const  clear;
};

#ifdef DEBUG
char __assert_size_GieiHv[(sizeof(Hv_h) == sizeof(struct GieiHv_t))? 1:-1];
#endif // DEBUG


int8_t
hv_init(
        struct Hv_h* const restrict self )
{
    union GieiHv_conv conv = {self};
    struct GieiHv_t* p_self = conv.clear;

    memset(p_self, 0, sizeof(*p_self));
    ACTION_ON_CAN_NODE(CAN_GENERAL,{
        p_self->lem_mailbox = hardware_get_mailbox(can_node, CAN_ID_BMSHV1,7);
        if (!p_self->lem_mailbox) {
        return -1;
        }

        p_self->send_mailbox_bms_hv = hardware_get_mailbox_send(can_node, CAN_ID_INVVOLT, 2);
        if (!p_self->send_mailbox_bms_hv) {
        hardware_free_mailbox_can(&p_self->lem_mailbox);
        }
    })
    return 0;
}

int8_t
hv_update(Hv_h* const restrict self )
{
    union GieiHv_conv conv = {self};
    struct GieiHv_t* p_self = conv.clear;
    CanMessage mex; 
    if(hardware_mailbox_read(p_self->lem_mailbox,&mex)>=0)
    {
        union {
            uint32_t u32;
            float f;
        }conv;
        conv .u32 = mex.full_word;
        if (conv.f<0) {
            return -1;
        }
        p_self->lem_current = conv.f;
    }

    return 0;
}

/*
 * Battery pack tension is given indipendently by every motor.
 * The function seems complex because takes in consideration the case
 * that one or more motor are inactive.
 *
 * BMS precharge needs a message with the tot voltage
 */
int8_t
giei_hv_computeBatteryPackTension(
        struct Hv_h* const restrict self ,
        const float* const engines_voltages ,
        const uint8_t num_of_voltages)
{
    union GieiHv_conv conv = {self};
    struct GieiHv_t* p_self = conv.clear;
    uint8_t active_motors = 0;
    float sum = 0.0f;
    uint8_t max = 0;

    // find max voltage
    for (uint8_t i = 0; i < num_of_voltages; i++)
    {
        if (engines_voltages[i] > max)
        {
            max = engines_voltages[i];
        }
    }

    // Compute sum of voltages, exclude if it is below 50 V than the maximum reading
    for (uint8_t i = 0; i < num_of_voltages; i++)
    {
        if (engines_voltages[i] > (max - 50))
        {
            active_motors++;
            sum += engines_voltages[i];
        }
    }

    if (!active_motors) {
        p_self->hv_public_data[HV_BATTERY_PACK_TENSION] =0;
        p_self->hv_public_data[HV_TOTAL_POWER] =0;
    }
    else {
        p_self->hv_public_data[HV_BATTERY_PACK_TENSION] =sum / active_motors;
        p_self->hv_public_data[HV_TOTAL_POWER] =
            p_self->hv_public_data[HV_BATTERY_PACK_TENSION] * p_self->lem_current;
    }
    return 
        hardware_mailbox_send(p_self->send_mailbox_bms_hv, 
                p_self->hv_public_data[HV_BATTERY_PACK_TENSION]);
}

float
giei_hv_get_info(const struct Hv_h* const restrict self ,
        const enum GIEI_HV_INFO info)
{
    union GieiHv_const_conv conv = {self};
    const struct GieiHv_t* const p_self = conv.clear;

    if (info!=__NUM_OF_GIEI_HV_INFO__) {
        return p_self->hv_public_data[info];
    }
    return -1;
}
