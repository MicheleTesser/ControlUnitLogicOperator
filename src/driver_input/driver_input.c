#include "../../lib/raceup_board/raceup_board.h"
#include "../board_conf/id_conf.h"
#include "./driver_input.h"
#include <stdint.h>

//private
static struct{
    float percentages[NUM_OF_INPUT_TYPES_USED_ONLY_FOR_INDEX];
    union{
        struct{
            uint8_t thr_brk : 1;
            uint8_t thr_pdl : 1;
            uint8_t thr_pot : 1;
        }impls;
        uint8_t impls_raw;
    };
}driver_info;

static void rtd_physical_button_toggled(void) INTERRUP_ATTRIBUTE
{
    driver_info.percentages[READY_TO_DRIVE_BUTTON] = gpio_read_state(READY_TO_DRIVE_INPUT_BUTTON);
}

//public

int8_t driver_input_init(void)
{
    return hardware_interrupt_attach_fun(INTERRUPT_RTD_BUTTON, rtd_physical_button_toggled);
}

float driver_get_amount(enum INPUT_TYPES driver_input)
{
    if (driver_input < NUM_OF_INPUT_TYPES_USED_ONLY_FOR_INDEX) {
        return driver_info.percentages[driver_input];
    }
    return -1;
}

uint8_t driver_set_amount(enum INPUT_TYPES driver_input, float percentage)
{
    if (percentage > 100 || driver_input >= NUM_OF_INPUT_TYPES_USED_ONLY_FOR_INDEX) {
        return -1;
    }
    driver_info.percentages[driver_input] = percentage;
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
