#include "temperatures.h"

static float temps[TEMPS_AMOUNT];

int8_t save_temperature(const enum TEMPS_CATEGORY category, const float value)
{
    switch (category) {
        case BMS_LV_1:
        case BMS_LV_2:
        case BMS_HV_MAX:
        case BMS_HV_AVG:
        case BMS_HV_MIN:
        case ENGINE_POST_L:
        case ENGINE_PRE_L:
        case ENGINE_POST_R:
        case ENGINE_PRE_R:
        case COLDPLATE_POST_L:
        case COLDPLATE_PRE_L:
        case COLDPLATE_POST_R:
        case COLDPLATE_PRE_R:
            temps[category] = value;
            break;
        default:
            return -1;
    }
    return 0;
}
