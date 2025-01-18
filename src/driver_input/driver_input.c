#include "driver_input.h"
#include "../lib/raceup_board/raceup_board.h"
#include <stdint.h>
#include <string.h>

//private

typedef int16_t index_tye;
struct DriverInput_t{
    float driver_data[__NUM_OF_DRIVERS__ * __NUM_OF_INPUT_TYPES__];
    struct CanMailbox* drivers_mailboxes[__NUM_OF_DRIVERS__];
    enum DRIVER current_driver;
};

#ifdef DEBUG
const uint8_t __assert_driver_input_size[sizeof(struct DriverInput_h) == sizeof(struct DriverInput_t)? 1 : -1];
#endif /* ifdef DEBUG */

union DriverInputConv{
    struct DriverInput_h* const hidden;
    struct DriverInput_t* const clear;
};

static inline index_tye compute_data_index(const struct DriverInput_t* const restrict self,
        const enum INPUT_TYPES driver_input)
{
    const enum DRIVER driver = self->current_driver; 
    if (driver != __NUM_OF_DRIVERS__ && driver_input != __NUM_OF_INPUT_TYPES__)
    {
        return driver * sizeof(self->driver_data[0]) + driver_input;   
    }
    return -1;
}

//public

int8_t driver_input_init(struct DriverInput_h* const restrict self,
        const uint16_t human_mailbox_number,
        const uint16_t dv_mailbox_number)
{
    union Conv{
        struct DriverInput_h* const restrict hidden;
        struct DriverInput_t* const restrict clear;
    };
    union Conv d_conv = {self};
    struct DriverInput_t* const p_self = d_conv.clear;
    memset(p_self, 0, sizeof(*p_self));
    p_self->drivers_mailboxes[DRIVER_HUMAN] = hardware_get_mailbox(human_mailbox_number);
    p_self->drivers_mailboxes[DRIVER_EMBEDDED] = hardware_get_mailbox(dv_mailbox_number);
    p_self->current_driver = DRIVER_HUMAN;

    return 0;
}

int8_t driver_input_change_driver(struct DriverInput_h* const restrict self,
        const enum DRIVER driver)
{
    union Conv{
        struct DriverInput_h* const restrict hidden;
        struct DriverInput_t* const restrict clear;
    };
    union Conv d_conv = {self};
    struct DriverInput_t* const p_self = d_conv.clear;
    if (driver != __NUM_OF_DRIVERS__) {
        p_self->current_driver = driver;
        return 0;
    }
    return -1;
}

float driver_get_amount(const struct DriverInput_h* const restrict self,
        const enum INPUT_TYPES driver_input)
{
    union Conv{
        const struct DriverInput_h* const restrict hidden;
        const struct DriverInput_t* const restrict clear;
    };
    union Conv d_conv = {self};
    const struct DriverInput_t* const p_self = d_conv.clear;
    const index_tye data_index = compute_data_index(p_self, driver_input);

    if (data_index != -1)
    {
        return p_self->driver_data[data_index];
    }
    return -1;
}

void driver_input_destroy(struct DriverInput_h* restrict self)
{
    union Conv{
        struct DriverInput_h* const restrict hidden;
        struct DriverInput_t* const restrict clear;
    };
    union Conv d_conv = {self};
    struct DriverInput_t* const p_self = d_conv.clear;
    for (uint8_t i=0; i < __NUM_OF_DRIVERS__; i++ ) {
        hardware_free_mailbox_can(&p_self->drivers_mailboxes[i]);
    }
}
