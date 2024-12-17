#ifndef __AMK_POWER_SYSTEM__
#define __AMK_POWER_SYSTEM__

#include <stdint.h>
#include "../../lib/raceup_board/raceup_board.h"
#include "../engine_common.h"

int8_t stop_engine(const enum ENGINES engine);
int8_t set_regen_brake_engine(const enum ENGINES engine, const int16_t brake);
int8_t set_throttle_engine(const enum ENGINES engine, const int16_t throttle);
uint8_t inverter_hv_status(void);
void update_status(const CanMessage* const restrict mex);

#endif // !__AMK_POWER_SYSTEM__
