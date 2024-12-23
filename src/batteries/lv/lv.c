#include "lv.h"
#include "../../lib/raceup_board/components/can.h"
#include "../../lib/board_dbc/can2.h"
#include <stdint.h>

#define NUM_OF_LV_CELLS 6
static uint16_t lv_cells[NUM_OF_LV_CELLS];

int8_t lv_init(void)
{
    return 0;
}

int8_t lv_update_status(const CanMessage * const restrict mex)
{
    can_obj_can2_h_t o;
    switch (mex->id) {
        case CAN_ID_BMSLV1:
            unpack_message_can2(&o, mex->id, mex->full_word, mex->message_size, 0);
            lv_cells[0] = o.can_0x054_BmsLv1.volt1;
            lv_cells[1] = o.can_0x054_BmsLv1.volt2;
            lv_cells[2] = o.can_0x054_BmsLv1.volt3;
            lv_cells[3] = o.can_0x054_BmsLv1.volt4;
            break;
        case CAN_ID_BMSLV2:
            unpack_message_can2(&o, mex->id, mex->full_word, mex->message_size, 0);
            lv_cells[4] = o.can_0x055_BmsLv2.volt5;
            lv_cells[5] = o.can_0x055_BmsLv2.volt6;
            break;
        default:
            return -1;
    }
    return 0;
}
