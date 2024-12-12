#ifndef __BOARD_CAN__
#define __BOARD_CAN__ 

#include <stdint.h>

#include "../lib/raceup_board/raceup_board.h"

enum CAN_FREQUENCY{
    _1_MBYTE_S_,
    _500_KBYTE_S_,
    _250_KBYTE_S_,
    _125_KBYTE_S_,
    _50_KBYTE_S_,
    _20_KBYTE_S_,
    _10_KBYTE_S_,
    _5_KBYTE_S_,
};

int8_t board_can_init(uint8_t can_id, enum CAN_FREQUENCY freq);
int8_t board_can_read(uint8_t can_id, CanMessage* o_mex);
int8_t board_can_write(uint8_t can_id, CanMessage* o_mex);

#endif // !__BOARD_CAN__
