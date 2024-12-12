#ifndef __BOARD_CAN__
#define __BOARD_CAN__ 

#include <stdint.h>

#include "../lib/raceup_board/raceup_board.h"

int8_t board_can_init(uint8_t can_id, uint32_t freq);
int8_t board_can_read(uint8_t can_id, CanMessage* o_mex);
int8_t board_can_write(uint8_t can_id, CanMessage* o_mex);

#endif // !__BOARD_CAN__
