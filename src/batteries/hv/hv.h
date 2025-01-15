#ifndef __HV_BATTERIE__
#define __HV_BATTERIE__

#include "../../lib/raceup_board/components/can.h"
#include <stdint.h>

enum HV_INFO{
    HV_TOTAL_POWER,
    HV_LEM_CURRENT,
    HV_BATTERY_PACK_TENSION,
};

struct Hv;

int8_t hv_init(void);
const struct Hv* hv_get(void);
struct Hv* hv_get_mut(void);
int8_t hv_update_status(struct Hv* const restrict self, const CanMessage* const restrict mex);
int8_t hv_computeBatteryPackTension(struct Hv* const restrict self, 
        const float* const engines_voltages, const uint8_t num_of_voltages);
int8_t hv_get_info(const struct Hv* const restrict self, 
        const enum HV_INFO info, void* const buffer, const uint8_t buffer_size);

void hv_free_read_ptr(void);
void hv_free_mut_ptr(void);

#define HV_READ_ONLY_ACTION(exp)\
{\
    const struct Hv* hv_read_ptr =  hv_get();\
    exp;\
    hv_free_read_ptr();\
}

#define HV_MUT_ACTION(exp)\
{\
    struct Hv* hv_mut_ptr =  hv_get_mut();\
    exp;\
    hv_free_mut_ptr();\
}

#endif // !__LV_BATTERIE__
