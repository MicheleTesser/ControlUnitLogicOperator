#include "driver_input.h"
#include "../../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#include "../../../../lib/raceup_board/raceup_board.h"
#include <stdint.h>
#include <string.h>

//private

typedef int16_t index_tye;
struct DriverInput_t{
    float driver_data[__NUM_OF_DRIVERS__ * __NUM_OF_INPUT_TYPES__];
    struct CanMailbox* drivers_mailboxes[__NUM_OF_DRIVERS__ -1];
    enum DRIVER current_driver;
    GpioRead_h gpio_rtd_button;
};

union DriverInput_h_t_conv{
    DriverInput_h* const restrict hidden;
    struct DriverInput_t* const restrict clear;
};

union DriverInput_h_t_conv_const{
    const DriverInput_h* const restrict hidden;
    const struct DriverInput_t* const restrict clear;
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

int8_t driver_input_init(struct DriverInput_h* const restrict self)
{
    union Conv{
        struct DriverInput_h* const restrict hidden;
        struct DriverInput_t* const restrict clear;
    };
    union Conv d_conv = {self};
    struct DriverInput_t* const p_self = d_conv.clear;

    memset(p_self, 0, sizeof(*p_self));

    if (hardware_init_read_permission_gpio(&p_self->gpio_rtd_button, GPIO_RTD_BUTTON)<0)
    {
        return -1;
    }

    ACTION_ON_CAN_NODE(CAN_GENERAL,{
        p_self->drivers_mailboxes[DRIVER_HUMAN] = hardware_get_mailbox(can_node, CAN_ID_DRIVER,4);
        p_self->drivers_mailboxes[DRIVER_EMBEDDED] = hardware_get_mailbox(); //TODO: not yet defined
        p_self->current_driver = DRIVER_NONE;
    });

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

int8_t
driver_input_update(DriverInput_h* const restrict self )
{
    union DriverInput_h_t_conv conv = {self};
    struct DriverInput_t* const restrict p_self = conv.clear;
    can_obj_can2_h_t o2;
    CanMessage mex;
    const struct CanMailbox* mailbox = NULL;
    index_tye index_input = 0;

    switch (p_self->current_driver) {
        case DRIVER_HUMAN:
            mailbox = p_self->drivers_mailboxes[DRIVER_HUMAN];
            if (hardware_mailbox_read(mailbox, &mex)>0) {
                unpack_message_can2(&o2, CAN_ID_DRIVER, mex.full_word, mex.message_size, timer_time_now());
                index_input = compute_data_index(p_self, THROTTLE);
                p_self->driver_data[index_input] = o2.can_0x053_Driver.throttle;
                index_input = compute_data_index(p_self, BRAKE);
                p_self->driver_data[index_input] = o2.can_0x053_Driver.brake;
            }
            break;
        case DRIVER_EMBEDDED:
            //TODO: not yet implemented
            break;
        default:
           return -1; 
    }
    return 0;
}

float driver_input_get(const struct DriverInput_h* const restrict self,
        const enum INPUT_TYPES driver_input)
{
    const union DriverInput_h_t_conv_const conv = {self};
    const struct DriverInput_t* const p_self = conv.clear;
    const index_tye data_index = compute_data_index(p_self, driver_input);

    if (data_index != -1)
    {
        return p_self->driver_data[data_index];
    }
    return -1;
}

int8_t
driver_input_rtd_request(const DriverInput_h* const restrict self)
{
    const union DriverInput_h_t_conv_const conv = {self};
    const struct DriverInput_t* const p_self = conv.clear;
    switch (p_self->current_driver) {
        case DRIVER_HUMAN:
            return gpio_read_state(&p_self->gpio_rtd_button);
        case DRIVER_EMBEDDED:
            //TODO: not yet defined
        default:
            return -1;
    }
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
