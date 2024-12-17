#ifndef __CAR_TEMPERATURES__
#define __CAR_TEMPERATURES__ 

#include <stdint.h>
enum TEMPS_CATEGORY{
    BMS_LV_1,
    BMS_LV_2,
    BMS_HV_MAX,
    BMS_HV_AVG,
    BMS_HV_MIN,

    ENGINE_POST_L,
    ENGINE_PRE_L,
    ENGINE_POST_R,
    ENGINE_PRE_R,

    COLDPLATE_POST_L,
    COLDPLATE_PRE_L,
    COLDPLATE_POST_R,
    COLDPLATE_PRE_R,
};

int8_t save_temperature(const enum TEMPS_CATEGORY category, const float value);
float get_temperature(const enum TEMPS_CATEGORY category);

#endif // !__CAR_TEMPERATURES__
