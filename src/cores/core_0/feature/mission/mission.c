#include "mission.h"
#include <stdint.h>
#include <string.h>
#include "../driver_input/driver_input.h"
#include "../../../../lib/raceup_board/components/can.h"

struct Mission_t{
    DriverInput_h* p_driver;
    struct CanMailbox* mission_mailbox;
    enum DRIVER m_type:2;
    uint8_t lock_mission:1;

};

union Mission_h_t_conv
{
    Mission_h* const restrict hidden;
    struct Mission_t* const restrict clear;
};

int8_t mission_init(Mission_h* const restrict self ,
        DriverInput_h* const driver )
{
    union Mission_h_t_conv conv = {self};
    struct Mission_t* const restrict p_self = conv.clear;
    memset(p_self, 0, sizeof(*p_self));

    p_self->p_driver = driver;
    p_self->m_type = DRIVER_NONE;
    p_self->mission_mailbox = hardware_get_mailbox(); //TODO: message not yet defined
    if (!p_self->mission_mailbox)
    {
        return -1;
    }


    return 0;
}

int8_t mission_update(Mission_h* const restrict self )
{
    union Mission_h_t_conv conv = {self};
    struct Mission_t* const restrict p_self = conv.clear;
    CanMessage mex;

    if (!p_self->lock_mission 
            && hardware_mailbox_read(p_self->mission_mailbox, &mex)>0
            && p_self->m_type != mex.full_word)
    {
        //TODO: unpack data
        p_self->m_type = mex.full_word;
        driver_input_change_driver(p_self->p_driver, p_self->m_type);
        return 0;
    }

    return -1;

}
void mission_lock(Mission_h* const restrict self )
{
    union Mission_h_t_conv conv = {self};
    struct Mission_t* const restrict p_self = conv.clear;
    p_self->lock_mission = 1;

}
void mission_unlock(Mission_h* const restrict self )
{
    union Mission_h_t_conv conv = {self};
    struct Mission_t* const restrict p_self = conv.clear;
    p_self->lock_mission = 0;
}
