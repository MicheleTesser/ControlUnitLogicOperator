#include "./driver_input.h"
#include <stdint.h>

static struct{
    float percentages[REGEN + 1];
    union{
        struct{
            uint8_t thr_brk : 1;
            uint8_t thr_pdl : 1;
            uint8_t thr_pot : 1;
        }impls;
        uint8_t impls_raw;
    };
}driver_info;

float get_amount(enum INPUT_TYPES driver_input)
{
    return driver_info.percentages[driver_input];
}
float set_amount(enum INPUT_TYPES driver_input, float percentage)
{
    if (percentage > 100) {
        return -1;
    }
    driver_info.percentages[driver_input] = percentage;
    return 0;
}

void save_implausibility(const enum IMPL impl)
{
    if (impl & THROTTLE_BRAKE) {
        driver_info.impls.thr_brk = 1;
    }

    if (impl & THROTTLE_PADEL) {
        driver_info.impls.thr_pdl = 1;
    }

    if (impl & THROTTLE_POT) {
        driver_info.impls.thr_pot = 1;
    }
}

void clear_implausibility(void)
{
    driver_info.impls_raw = 0;
}

uint8_t check_imp(const enum IMPL impl)
{
    if ((impl & THROTTLE_BRAKE) && driver_info.impls.thr_brk) {
        return 1;
    }

    if ((impl & THROTTLE_PADEL) && driver_info.impls.thr_pdl) {
        return 1;
    }

    if ((impl & THROTTLE_POT) && driver_info.impls.thr_pot) {
        return  1;
    }

    return 0;
}

float driver_get_amount(const enum INPUT_TYPES driver_input)
{
    return driver_info.percentages[driver_input];
}

float driver_set_amount(const enum INPUT_TYPES driver_input, const float percentage)
{
    return driver_info.percentages[driver_input] = percentage;
}
