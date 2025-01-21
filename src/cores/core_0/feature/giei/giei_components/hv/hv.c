#include "hv.h"
#include "../../../../../../lib/raceup_board/components/can.h"
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


int8_t
hv_init(
        struct Hv_h* const restrict self __attribute__((__nonnull__)))
{
    union GieiHv_conv conv = {self};
    struct GieiHv_t* p_self = conv.clear;

    memset(p_self, 0, sizeof(*p_self));
    p_self->lem_mailbox = hardware_get_mailbox(CORE_0_HV);
    if (!p_self->lem_mailbox) {
        return -1;
    }

    p_self->send_mailbox_bms_hv = hardware_get_mailbox_send(CORE_0_BMS_HV);
    if (!p_self->send_mailbox_bms_hv) {
        hardware_free_mailbox_can(&p_self->lem_mailbox);
    }
    return 0;
}

int8_t
hv_update(Hv_h* const restrict self __attribute__((__nonnull__)))
{
    union GieiHv_conv conv = {self};
    struct GieiHv_t* p_self = conv.clear;
    const uint64_t lem_data = hardware_mailbox_read(p_self->lem_mailbox);
    if (lem_data<0) {
        return -1;
    }
    p_self->lem_current = lem_data;
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
        struct Hv_h* const restrict self __attribute__((__nonnull__)),
        const float* const engines_voltages __attribute__((__nonnull__)),
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
giei_hv_get_info(const struct Hv_h* const restrict self __attribute__((__nonnull__)),
        const enum GIEI_HV_INFO info)
{
    union GieiHv_const_conv conv = {self};
    const struct GieiHv_t* const p_self = conv.clear;

    if (info!=__NUM_OF_GIEI_HV_INFO__) {
        return p_self->hv_public_data[info];
    }
    return -1;
}
