#include "giei_hv.h"
#include "../../../../../lib/raceup_board/components/can.h"
#include <stdint.h>
#include <string.h>

struct GieiHv_t{
    struct CanMailbox* lem_mailbox;
    uint32_t battery_pack_tension;
    float total_power;
    float lem_current;
};

union GieiHv_conv{
    struct GieiHv_h* const hidden;
    struct GieiHv_t* const clear;
};

union GieiHv_const_conv{
    const struct GieiHv_h* const  hidden;
    const struct GieiHv_t* const  clear;
};


int8_t
giei_hv_init(
        struct GieiHv_h* const restrict self __attribute__((__nonnull__)))
{
    memset(self, 0, sizeof(*self));
    union GieiHv_conv conv = {self};
    struct GieiHv_t* p_self = conv.clear;
    p_self->lem_mailbox = hardware_get_mailbox(CORE_0_HV);
    if (!p_self->lem_mailbox) {
        return -1;
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
        struct GieiHv_h* const restrict self __attribute__((__nonnull__)),
        const float* const engines_voltages,
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
        p_self->battery_pack_tension= 0;
        p_self->total_power = 0.0f;
    }
    else {
        p_self->battery_pack_tension = (sum / active_motors);
        p_self->total_power = p_self->battery_pack_tension * p_self->lem_current;
    }
    return send_tension_bms(p_self->battery_pack_tension);
}

float
giei_hv_get_info(const struct GieiHv_h* const restrict self __attribute__((__nonnull__)),
        const enum GIEI_HV_INFO info)
{
    union GieiHv_const_conv conv = {self};
    const struct GieiHv_t* const p_self = conv.clear;
    
    switch (info) {
        case HV_BATTERY_PACK_TENSION:
            return p_self->battery_pack_tension;
        case HV_TOTAL_POWER:
            return p_self->total_power;
        default:
            return -1;
    }
}
