#include "colling.h"
#include "cooling_device/cooling_device.h"
#include "../../../lib/raceup_board/components/can.h"
#include <stdint.h>
#include <string.h>

#define MAX_MAILBOX_FOR_DEVICE 4

typedef struct Cooling_t{
    const GeneralCan_h* can;
    struct CoolingDeviceData{
        CoolingDevice_h raw_device;
        uint16_t set_mex_id;
        uint8_t mailbox_num;
        struct CanMailbox* can_mailbox[MAX_MAILBOX_FOR_DEVICE]; 
    }devices[__NUM_OF_COOLING_DEVICES__];
}Cooling_t;

union Cooling_h_t_conv{
    Cooling_h* hidden;
    struct Cooling_t* clear;
};


//public

int8_t cooling_init(Cooling_h* const restrict self, const GeneralCan_h* const restrict can_bus)
{
    memset(self, 0, sizeof(*self));
    union Cooling_h_t_conv conv = {self};
    struct Cooling_t* const p_self = conv.clear;
    p_self->can = can_bus;
    p_self->devices[FANS_RADIATOR].set_mex_id =; //TODO: not yet defined
    p_self->devices[PUMPS].set_mex_id =; //TODO: not yet defined

    return 0;
}


int8_t cooling_switch_device(Cooling_h* const restrict self, const enum COOLING_DEVICES dev_id)
{
#ifdef DEBUG
    if (dev_id==__NUM_OF_COOLING_DEVICES__)
    {
        return -1;
    }
#endif /* ifdef DEBUG */
    union Cooling_h_t_conv conv = {self};
    struct Cooling_t* const p_self = conv.clear;
    return cooling_device_toggle(&p_self->devices[dev_id].raw_device);
}
