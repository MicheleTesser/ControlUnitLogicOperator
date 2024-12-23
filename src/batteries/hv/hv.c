#include "hv.h"
#include "../../../lib/board_dbc/can2.h"

#include <stddef.h>
#include <stdint.h>

static struct{
    uint16_t min_voltage;
    uint16_t avg_voltage;
    uint16_t max_voltage;
    uint16_t soc;
}HV;

int8_t hv_init(void)
{
    return 0;
}

int8_t hv_update_status(const CanMessage* const restrict mex)
{
    if (mex->id == CAN_ID_BMSHV1) {
        can_obj_can2_h_t o;
        unpack_message_can2(&o, mex->id, mex->full_word, mex->message_size, 0);
        HV.min_voltage = o.can_0x057_BmsHv1.min_volt;
        HV.max_voltage = o.can_0x057_BmsHv1.max_volt;
        HV.avg_voltage = o.can_0x057_BmsHv1.avg_volt;
        HV.soc= o.can_0x057_BmsHv1.soc;
        return 0;
    }
    return -1;
}
