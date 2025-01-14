#include "hv.h"
#include "../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#include "../../board_conf/id_conf.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

//private

static struct{
    uint16_t min_voltage;
    uint16_t avg_voltage;
    uint16_t max_voltage;
    uint16_t soc;
    uint32_t battery_pack_tension;
    float lem_current;
    float total_power;
}HV;

static int8_t send_tension_bms(const uint64_t tension){
    can_obj_can2_h_t o;
    CanMessage mex;
    memset(&mex, 0, sizeof(mex));

    o.can_0x120_InvVolt.car_voltage = tension;
    mex.id = CAN_ID_INVVOLT;
    mex.message_size = pack_message_can2(&o, CAN_ID_INVVOLT, &mex.full_word);

    return hardware_write_can(CAN_MODULE_GENERAL, &mex);
}

//public

int8_t hv_init(void)
{
    return 0;
}

int8_t hv_update_status(const CanMessage* const restrict mex)
{
    can_obj_can2_h_t o;
    switch (mex->id) {
        case CAN_ID_LEM:
            unpack_message_can2(&o, mex->id, mex->full_word, mex->message_size, 0);
            HV.lem_current = o.can_0x3c2_Lem.current;
            break;
        case CAN_ID_BMSHV1:
            unpack_message_can2(&o, mex->id, mex->full_word, mex->message_size, 0);
            HV.min_voltage = o.can_0x057_BmsHv1.min_volt;
            HV.max_voltage = o.can_0x057_BmsHv1.max_volt;
            HV.avg_voltage = o.can_0x057_BmsHv1.avg_volt;
            HV.soc= o.can_0x057_BmsHv1.soc;
            break;
    }
    return -1;
}

/*
 * Battery pack tension is given indipendently by every motor.
 * The function seems complex because takes in consideration the case
 * that one or more motor are inactive.
 *
 * BMS precharge needs a message with the tot voltage
 */
int8_t hv_computeBatteryPackTension(const float engines_voltages[NUM_OF_EGINES])
{
    uint8_t active_motors = 0;
    float sum = 0.0f;
    uint8_t max = 0;

    // find max voltage
    for (uint8_t i = 0; i < NUM_OF_EGINES; i++)
    {
        if (engines_voltages[i] > max)
        {
            max = engines_voltages[i];
        }
    }

    // Compute sum of voltages, exclude if it is below 50 V than the maximum reading
    for (uint8_t i = 0; i < NUM_OF_EGINES; i++)
    {
        if (engines_voltages[i] > (max - 50))
        {
            active_motors++;
            sum += engines_voltages[i];
        }
    }

    if (!active_motors) {
        HV.battery_pack_tension= 0;
        HV.total_power = 0.0f;
    }
    else {
        HV.battery_pack_tension = (sum / active_motors);
        HV.total_power = HV.battery_pack_tension * HV.lem_current;
    }
    return send_tension_bms(HV.battery_pack_tension);
}

int8_t hv_get_info(const enum HV_INFO info, void* const buffer, const uint8_t buffer_size)
{
    void* src = NULL;
    uint8_t src_size=0;
    switch (info) {
        case HV_TOTAL_POWER:
            src_size = sizeof(HV.total_power);
            src = &HV.total_power;
            break;
        case HV_LEM_CURRENT:
            src_size = sizeof(HV.lem_current);
            src = &HV.lem_current;
            break;
        case HV_BATTERY_PACK_TENSION:
            src_size = sizeof(HV.battery_pack_tension);
            src = &HV.battery_pack_tension;
            break;
        default:
            return -1;
    }
    if (src_size > buffer_size) {
        return -2;
    }
    memcpy(buffer, src, src_size);
    return 0;
}
