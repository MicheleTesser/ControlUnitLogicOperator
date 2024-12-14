#ifndef __AMK_POWER_SYSTEM__
#define __AMK_POWER_SYSTEM__

#include <stdint.h>
#include "../../lib/raceup_board/raceup_board.h"

void stop_engines(void);
uint8_t inverter_hv_status(void);
void update_status(const CanMessage* const restrict mex);

#endif // !__AMK_POWER_SYSTEM__
