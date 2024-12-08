#include "gpio.h"
#include "IfxPort.h"
#include <stdint.h>
#include "../../Libraries/ControlUnitLogicOperator/lib/raceup_board/components/gpio/gpio.h"

int8_t hardware_init_gpio(const uint16_t id)
{
    switch (id) {
        case 0:
            IfxPort_setPinModeOutput(&MODULE_P00,5, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
            IfxPort_setPinLow(&MODULE_P00,5);
            break;
        default:
            return -1;
    }
    return 0;
}
int8_t gpio_set_pin_mode(const component_gpio* const restrict  comp,uint8_t mode)
{
    return 0;
}
int8_t gpio_toggle(const component_gpio* const restrict  comp)
{
    switch (comp->id) {
        case 0:
            IfxPort_togglePin(&MODULE_P00,5);
            break;
        default:
            return -1;
    }
    return 0;
}
int8_t gpio_read_state(const component_gpio* const restrict comp)
{
    return 0;
}
int8_t gpio_set_high(const component_gpio* const restrict comp)
{
    switch (comp->id) {
        case 0:
            IfxPort_setPinHigh(&MODULE_P00,5);
            break;
        default:
            return -1;
    }
    return 0;
}
int8_t gpio_set_low(const component_gpio* const restrict comp)
{
    switch (comp->id) {
        case 0:
            IfxPort_setPinLow(&MODULE_P00,5);
            break;
        default:
            return -1;
    }
    return 0;
}
