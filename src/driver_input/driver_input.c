#include "./driver_input.h"
#include <stdint.h>

static float percentages[REGEN + 1];

float get_amount(enum INPUT_TYPES driver_input)
{
    return percentages[driver_input];
}
float set_amount(enum INPUT_TYPES driver_input, float percentage)
{
    if (percentage > 100) {
        return -1;
    }
    percentages[driver_input] = percentage;
    return 0;
}
