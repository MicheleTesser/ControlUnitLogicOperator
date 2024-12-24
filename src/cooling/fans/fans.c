#include "./fans.h"
#include "../../lib/board_dbc/can2.h"
#include "../../board_can/board_can.h"
#include "../../board_conf/id_conf.h"
#include <stdint.h>
#include <string.h>

//private

static float fans[NUMBER_OF_FAN_TYPES];

static inline int8_t fan_init_done(void)
{
    return 0;
}

static int8_t setup_fan_radiator(const uint8_t enable)
{
    can_obj_can2_h_t o;
    CanMessage mex;
    memset(&mex, 0, sizeof(mex));
    o.can_0x130_Pcu.fan_enable = enable;
    mex.id = CAN_ID_PCU;
    mex.message_size = pack_message_can2(&o, CAN_ID_PCU, &mex.full_word);
    return board_can_write(CAN_MODULE_GENERAL, &mex);
}

//public
int8_t fan_init(void)
{
    if (!fan_init_done()) {
       
    }
    return 0;
}


int8_t fan_enable(const uint8_t fans)
{
    if (fans & FANS_RADIATOR) {
        return setup_fan_radiator(1);
    }
    return 0;
}

int8_t fan_disable(const uint8_t fans)
{
    if (fans & FANS_RADIATOR) {
        return setup_fan_radiator(0);
    }
    return 0;
}

int8_t fan_set_value(const enum FAN_TYPES fan, const float value)
{
    switch (fan) {
        case FAN_BMS_HV:
        case FANS_RADIATOR:
            fans[fan] = value;
            return 0;
        default:
            return -1;
    }
}
