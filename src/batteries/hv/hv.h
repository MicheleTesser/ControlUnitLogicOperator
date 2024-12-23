#ifndef __HV_BATTERIE__
#define __HV_BATTERIE__

#include "../../lib/raceup_board/components/can.h"
#include <stdint.h>

int8_t hv_init(void);
int8_t hv_update_status(const CanMessage* const restrict mex);

#endif // !__LV_BATTERIE__
