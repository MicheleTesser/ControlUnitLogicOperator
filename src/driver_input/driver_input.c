#include "../../lib/raceup_board/raceup_board.h"
#include "../board_conf/id_conf.h"
#include "input_rtd/input_rtd.h"
#include "./driver_input.h"
#include <stdint.h>

//private
static struct DriverInput{
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
    uint8_t init_done: 1;
    uint8_t mut_ptr: 1;
    uint8_t read_ptr :6;
}DRIVER_INFO;

static void input_rtd_physical_button_toggled(void) INTERRUP_ATTRIBUTE
{
    input_rtd_check();
}

//public

int8_t driver_input_init(void)
{
    if (DRIVER_INFO.init_done) {
        return -1;
    }
    input_rtd_class_init();
    hardware_interrupt_attach_fun(INTERRUPT_RTD_BUTTON, input_rtd_physical_button_toggled);
    DRIVER_INFO.init_done=1;
    return 0;
}

const struct DriverInput* driver_input_get(void)
{
    while (!DRIVER_INFO.init_done || DRIVER_INFO.mut_ptr) {}

    DRIVER_INFO.read_ptr++;
    return &DRIVER_INFO;
}
struct DriverInput* driver_input_get_mut(void)
{
    while (!DRIVER_INFO.init_done || DRIVER_INFO.read_ptr || DRIVER_INFO.mut_ptr) {}

    DRIVER_INFO.mut_ptr++;
    return &DRIVER_INFO;
}

float driver_get_amount(const struct DriverInput* const restrict self,
        const enum INPUT_TYPES driver_input)
{
    if (driver_input < NUM_OF_INPUT_TYPES_USED_ONLY_FOR_INDEX) {
        return self->percentages[driver_input].value;
    }
    return -1;
}

uint8_t driver_set_amount(struct DriverInput* const restrict self,
        const enum INPUT_TYPES driver_input, 
        const float percentage, const time_var_microseconds timestamp)
{
    struct PercentageValue* val = &self->percentages[driver_input];

    if (percentage > 100 || driver_input >= NUM_OF_INPUT_TYPES_USED_ONLY_FOR_INDEX) {
        return -1;
    }

    //INFO: respecting rule T 15.1.4
    if (driver_input == BRAKE && val->timestamp == timestamp && val->value > percentage) {
        return -2;
    }

    self->percentages[driver_input].value = percentage;
    self->percentages[driver_input].timestamp = timestamp;
    return 0;
}

void set_implausibility(struct DriverInput* const restrict self,
        const enum IMPL impl,const uint8_t value)
{
    switch (impl) {
        case THROTTLE_BRAKE:
            self->impls.thr_brk = value;
            break;
        case THROTTLE_PADEL:
            self->impls.thr_pdl = value;
            break;
        case THROTTLE_POT:
            self->impls.thr_pot = value;
            break;
        default:
    }
}

void clear_implausibility(struct DriverInput* const restrict self)
{
    self->impls_raw = 0;
}

uint8_t check_impls(const struct DriverInput* const restrict self, const uint8_t impls)
{
    if ((impls & THROTTLE_BRAKE) && self->impls.thr_brk) {
        return 1;
    }

    if ((impls & THROTTLE_PADEL) && self->impls.thr_pdl) {
        return 1;
    }

    if ((impls & THROTTLE_POT) && self->impls.thr_pot) {
        return  1;
    }

    return 0;
}

void driver_input_free_read_ptr(void)
{
    DRIVER_INFO.read_ptr--;
}

void driver_input_free_mut_ptr(void)
{
    DRIVER_INFO.mut_ptr--;
}
