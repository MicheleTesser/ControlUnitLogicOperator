#ifndef __HV_BATTERIE__
#define __HV_BATTERIE__

#include "../../lib/raceup_board/components/can.h"
#include "../../GIEI/giei.h"
#include <stdint.h>

enum HV_INFO{
    HV_TOTAL_POWER,
    HV_LEM_CURRENT,
    HV_BATTERY_PACK_TENSION,
};

int8_t hv_init(void);
int8_t hv_update_status(const CanMessage* const restrict mex);
int8_t hv_computeBatteryPackTension(const float engines_voltages[NUM_OF_EGINES]);
int8_t hv_get_info(const enum HV_INFO info, void* const buffer, const uint8_t buffer_size);

#endif // !__LV_BATTERIE__
