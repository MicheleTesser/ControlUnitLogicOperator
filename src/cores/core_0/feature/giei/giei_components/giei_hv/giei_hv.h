#ifndef __GIEI_HV__
#define __GIEI_HV__

#include <stdint.h>

typedef struct GieiHv_h{
    const uint8_t private_data[1];
}GieiHv_h;

enum GIEI_HV_INFO{
    HV_BATTERY_PACK_TENSION,
    HV_TOTAL_POWER,
};

int8_t
giei_hv_init(GieiHv_h* const restrict self __attribute__((__nonnull__)));

int8_t
giei_hv_computeBatteryPackTension(GieiHv_h* const restrict self __attribute__((__nonnull__)),
        const float* const engines_voltages,
        const uint8_t num_of_voltages);

float
giei_hv_get_info(const GieiHv_h* const restrict self __attribute__((__nonnull__)),
        const enum GIEI_HV_INFO info);


#endif // !__GIEI_HV__
