#ifndef __GIEI_HV__
#define __GIEI_HV__

#include <stdint.h>

typedef struct __attribute__((aligned(4))){
  const uint8_t private_data[32];
}Hv_h;

enum GIEI_HV_INFO{
    HV_BATTERY_PACK_TENSION=0,
    HV_TOTAL_POWER,

    __NUM_OF_GIEI_HV_INFO__
};

int8_t
hv_init(Hv_h* const restrict self )__attribute__((__nonnull__(1)));

int8_t
hv_update(Hv_h* const restrict self )__attribute__((__nonnull__(1)));

int8_t
hv_computeBatteryPackTension(Hv_h* const restrict self ,
        const float* const engines_voltages,
        const uint8_t num_of_voltages)__attribute__((__nonnull__(1,2)));

float
hv_get_info(const Hv_h* const restrict self ,
        const enum GIEI_HV_INFO info)__attribute__((__nonnull__(1)));


#endif // !__GIEI_HV__
