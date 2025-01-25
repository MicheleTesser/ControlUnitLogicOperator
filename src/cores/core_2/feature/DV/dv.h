#ifndef __CAR_DV__
#define __CAR_DV__

#include <stdint.h>
#include "../dv_driver_input/dv_driver_input.h"
#include "../mission/mission.h"

typedef struct Dv_h{
    const uint8_t private_data[1];
}Dv_h;

int8_t
dv_class_init(Dv_h* const restrict self ,
        DvMission_h* const restrict mission ,
        DvDriverInput_h* const restrict driver)__attribute__((__nonnull__(1,2,3)));

int8_t dv_update(Dv_h* const restrict self)__attribute__((__nonnull__(1)));

#endif // !__CAR_DV__
