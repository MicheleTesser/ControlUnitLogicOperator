#ifndef __COOLING__
#define __COOLING__

#include <stdint.h>
#include "../../../../lib/raceup_board/components/can.h"
#include "../log/log.h"

typedef struct __attribute__((aligned(8))) Cooling_h{
    const uint8_t private_data[16];
}Cooling_h;

enum COOLING_DEVICES{
    PUMPS=0,
    FANS_RADIATOR,

    __NUM_OF_COOLING_DEVICES__
};

int8_t cooling_init(Cooling_h* const restrict self ,
        Log_h* const restrict log )__attribute__((__nonnull__(1,2)));
int8_t cooling_switch_device(Cooling_h* const restrict self ,
        const enum COOLING_DEVICES dev_id)__attribute__((__nonnull__(1)));
int8_t cooling_set_speed_device(Cooling_h* const restrict self ,
        const enum COOLING_DEVICES dev_id, const float speed)__attribute__((__nonnull__(1)));

#endif // !__COOLING__
