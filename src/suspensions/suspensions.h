#ifndef __CAR_SUSPENSIONS__
#define __CAR_SUSPENSIONS__

#include <stdint.h>

//INFO: leave the values incremental starting from 0
enum SUSP_ID{
    SUSP_FRONT_LEFT = 0,
    SUSP_FRONT_RIGHT,
    SUSP_REAR_LEFT,
    SUSP_REAR_RIGHT,

    SUSP_NUM_OF_SUSP, //INFO: do not use this enum, leave it at the end
};

int8_t suspensions_init(void);
int8_t suspensions_save(const enum SUSP_ID category, const float value);

#endif // !__CAR_SUSPENSIONS__
