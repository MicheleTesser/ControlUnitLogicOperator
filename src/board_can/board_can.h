#ifndef __BOARD_CAN__
#define __BOARD_CAN__ 

#include <stdint.h>

#include "../lib/raceup_board/raceup_board.h"

enum CAN_FREQUENCY{
    _1_MBYTE_S_ = 1000000,
    _500_KBYTE_S_ = 500000,
    _250_KBYTE_S_ = 250000,
    _125_KBYTE_S_ = 125000,
    _50_KBYTE_S_ = 50000,
    _20_KBYTE_S_ = 20000,
    _10_KBYTE_S_ = 10000,
    _5_KBYTE_S_ = 5000,
};

int8_t board_can_init(uint8_t can_id, enum CAN_FREQUENCY freq);
int8_t board_can_read(uint8_t can_id, CanMessage* o_mex);
int8_t board_can_write(uint8_t can_id, CanMessage* o_mex);

#endif // !__BOARD_CAN__
