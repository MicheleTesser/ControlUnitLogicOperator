#ifndef __LV_BATTERIE__
#define __LV_BATTERIE__

#include "../../lib/raceup_board/components/can.h"
#include <stdint.h>

int8_t lv_init(void);
int8_t lv_update_status(const CanMessage * const restrict mex);

#endif // !__LV_BATTERIE__
