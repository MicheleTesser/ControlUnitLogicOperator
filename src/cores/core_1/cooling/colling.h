#ifndef __COOLING__
#define __COOLING__

#include <stdint.h>
#include "../general_can/general_can.h"

typedef struct Cooling_h{
    const uint8_t private_data[1];
}Cooling_h;

#define MAX_MAILBOX_FOR_DEVICE 4

enum COOLING_DEVICES{
    PUMPS=0,
    FANS_RADIATOR,

    __NUM_OF_COOLING_DEVICES__
};

int8_t cooling_init(Cooling_h* const restrict self, const GeneralCan_h* const restrict can_bus);
int8_t cooling_switch_update_all(Cooling_h* const restrict self);
int8_t cooling_switch_device(Cooling_h* const restrict self, const enum COOLING_DEVICES dev_id);
int8_t cooling_set_speed_device(Cooling_h* const restrict self,
        const enum COOLING_DEVICES dev_id, const float speed);

#endif // !__COOLING__
