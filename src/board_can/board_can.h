#ifndef __BOARD_CAN__
#define __BOARD_CAN__ 

#include <stdint.h>

#include "../lib/raceup_board/raceup_board.h"

enum CAN_FREQUENCY{
    _1_MBYTE_S_ = 1000000L,
    _500_KBYTE_S_ = 500000L,
    _250_KBYTE_S_ = 250000L,
    _125_KBYTE_S_ = 125000L,
    _50_KBYTE_S_ = 50000L,
    _20_KBYTE_S_ = 20000L,
    _10_KBYTE_S_ = 10000L,
    _5_KBYTE_S_ = 5000L,
};

int8_t board_can_init(const uint8_t can_id, const enum CAN_FREQUENCY freq);
int8_t board_can_read(const uint8_t can_id, CanMessage* const restrict o_mex);
int8_t board_can_write(const uint8_t can_id, const CanMessage* const restrict o_mex);
int8_t board_can_manage_message(const uint8_t can_id, const CanMessage* const restrict mex);

#endif // !__BOARD_CAN__
