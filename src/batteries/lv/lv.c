#include "lv.h"
#include "../../lib/raceup_board/components/can.h"
#include "../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#include <stdint.h>

#define NUM_OF_LV_CELLS 6
static struct Lv{
    uint16_t lv_cells[NUM_OF_LV_CELLS];
    uint8_t init_done: 1;
    uint8_t mut_ptr: 1;
}LV;

int8_t lv_init(void)
{
    if (!LV.init_done) {
        LV.init_done =1;
        return 0;
    }
    return -1;
}

struct Lv* lv_get_mut(void)
{
    while (!LV.init_done && LV.mut_ptr) {}
    LV.mut_ptr++;

    return &LV;
}

int8_t lv_update_status(struct Lv* const restrict self, const CanMessage * const restrict mex)
{
    can_obj_can2_h_t o;
    switch (mex->id) {
        case CAN_ID_BMSLV1:
            unpack_message_can2(&o, mex->id, mex->full_word, mex->message_size, 0);
            self->lv_cells[0] = o.can_0x054_BmsLv1.volt1;
            self->lv_cells[1] = o.can_0x054_BmsLv1.volt2;
            self->lv_cells[2] = o.can_0x054_BmsLv1.volt3;
            self->lv_cells[3] = o.can_0x054_BmsLv1.volt4;
            break;
        case CAN_ID_BMSLV2:
            unpack_message_can2(&o, mex->id, mex->full_word, mex->message_size, 0);
            self->lv_cells[4] = o.can_0x055_BmsLv2.volt5;
            self->lv_cells[5] = o.can_0x055_BmsLv2.volt6;
            break;
        default:
            return -1;
    }
    return 0;
}

void lv_free_mut_ptr(void)
{
    LV.mut_ptr--;
}
