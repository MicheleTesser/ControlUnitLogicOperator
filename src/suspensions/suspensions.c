#include "suspensions.h"


static float suspensions[SUSP_NUM_OF_SUSP];

int8_t suspensions_init(void)
{
    return 0;
}

int8_t suspensions_save(const enum SUSP_ID category, const float value)
{
    suspensions[category] = value;
    return 0;
}
