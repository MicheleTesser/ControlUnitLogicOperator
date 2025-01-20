#ifndef __COOLING__
#define __COOLING__

#include <stdint.h>
#include "../general_can/general_can.h"
#include "../log/log.h"

typedef struct Cooling_h{
    const uint8_t private_data[1];
}Cooling_h;

enum COOLING_DEVICES{
    PUMPS=0,
    FANS_RADIATOR,

    __NUM_OF_COOLING_DEVICES__
};

int8_t cooling_init(Cooling_h* const restrict self __attribute__((__nonnull__)),
        const GeneralCan_h* const restrict can_bus __attribute__((__nonnull__)),
        Log_h* const restrict log __attribute__((__nonnull__)));
int8_t cooling_update_all(Cooling_h* const restrict self __attribute__((__nonnull__)));
int8_t cooling_switch_device(Cooling_h* const restrict self __attribute__((__nonnull__)),
        const enum COOLING_DEVICES dev_id);
int8_t cooling_set_speed_device(Cooling_h* const restrict self __attribute__((__nonnull__)),
        const enum COOLING_DEVICES dev_id, const float speed);

#endif // !__COOLING__
