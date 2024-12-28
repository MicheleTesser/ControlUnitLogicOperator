#include "../../lib/raceup_board/raceup_board.h"
#include "../board_conf/id_conf.h"
#include "input_rtd/input_rtd.h"
#include "./driver_input.h"
#include <stdint.h>

//private
static struct{
    struct PercentageValue{
        float value;
        time_var_microseconds timestamp;
    }percentages[NUM_OF_INPUT_TYPES_USED_ONLY_FOR_INDEX];
    union{
        struct{
            uint8_t thr_brk : 1;
            uint8_t thr_pdl : 1;
            uint8_t thr_pot : 1;
        }impls;
        uint8_t impls_raw;
    };
}driver_info;

static void input_rtd_physical_button_toggled(void) INTERRUP_ATTRIBUTE
{
    input_rtd_check();
}

//public

int8_t driver_input_init(void)
{
    input_rtd_class_init();
    return hardware_interrupt_attach_fun(INTERRUPT_RTD_BUTTON, input_rtd_physical_button_toggled);
}

float driver_get_amount(enum INPUT_TYPES driver_input)
{
    if (driver_input < NUM_OF_INPUT_TYPES_USED_ONLY_FOR_INDEX) {
        return driver_info.percentages[driver_input].value;
    }
    return -1;
}

uint8_t driver_set_amount(enum INPUT_TYPES driver_input, 
        const float percentage, const time_var_microseconds timestamp)
{
    struct PercentageValue* val = &driver_info.percentages[driver_input];

    if (percentage > 100 || driver_input >= NUM_OF_INPUT_TYPES_USED_ONLY_FOR_INDEX) {
        return -1;
    }

    //INFO: respecting rule T 15.1.4
    if (driver_input == BRAKE && val->timestamp == timestamp && val->value > percentage) {
        return -2;
    }

    driver_info.percentages[driver_input].value = percentage;
    driver_info.percentages[driver_input].timestamp = timestamp;
    return 0;
}

void set_implausibility(const enum IMPL impl, const uint8_t value)
{
    switch (impl) {
        case THROTTLE_BRAKE:
            driver_info.impls.thr_brk = value;
            break;
        case THROTTLE_PADEL:
            driver_info.impls.thr_pdl = value;
            break;
        case THROTTLE_POT:
            driver_info.impls.thr_pot = value;
            break;
        default:
    }
}

void clear_implausibility(void)
{
    driver_info.impls_raw = 0;
}

uint8_t check_impls(const uint8_t impls)
{
    if ((impls & THROTTLE_BRAKE) && driver_info.impls.thr_brk) {
        return 1;
    }

    if ((impls & THROTTLE_PADEL) && driver_info.impls.thr_pdl) {
        return 1;
    }

    if ((impls & THROTTLE_POT) && driver_info.impls.thr_pot) {
        return  1;
    }

    return 0;
}
