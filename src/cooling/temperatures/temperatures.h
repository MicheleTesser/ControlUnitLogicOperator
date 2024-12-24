#ifndef __CAR_TEMPERATURES__
#define __CAR_TEMPERATURES__ 

#include <stdint.h>
//INFO: leave the value of the enums sequentially
enum TEMPS_CATEGORY{
    BMS_LV_1 =0,
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

    ENGINE_POT_FRONT_RIGHT,
    ENGINE_PRE_FRONT_RIGHT,

    TEMPS_AMOUNT, //INFO: do not use this enum, leave it at the end
};

int8_t save_temperature(const enum TEMPS_CATEGORY category, const float value);

#endif // !__CAR_TEMPERATURES__
