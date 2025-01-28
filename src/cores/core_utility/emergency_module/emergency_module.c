#include "./emergency_module.h"
#include "../../../lib/raceup_board/raceup_board.h"
#include <stdlib.h>
#include <stdatomic.h>
#include <stdint.h>

//private

struct EmergencyNode{
    uint8_t emergency_state : 1;
    uint8_t emergency_amount;
    uint8_t emergency_buffer[];
};

static struct{
    atomic_flag lock;
    uint8_t excepion_counter;
    Gpio_h gpio_scs;
}EXCEPTION_COUNTER;

static void raise_module_exception_state(void) TRAP_ATTRIBUTE
{
    while (atomic_flag_test_and_set(&EXCEPTION_COUNTER.lock)){}
    EXCEPTION_COUNTER.excepion_counter++;
    gpio_set_low(&EXCEPTION_COUNTER.gpio_scs);
    atomic_flag_clear(&EXCEPTION_COUNTER.lock);
}

static void solved_module_exception_state(void) TRAP_ATTRIBUTE
{
    while (atomic_flag_test_and_set(&EXCEPTION_COUNTER.lock)){}
    EXCEPTION_COUNTER.excepion_counter--;
    if (!EXCEPTION_COUNTER.excepion_counter) {
        gpio_set_high(&EXCEPTION_COUNTER.gpio_scs);
    }
    atomic_flag_clear(&EXCEPTION_COUNTER.lock);
}


//public


struct EmergencyNode* EmergencyNode_new(const uint8_t num_exception)
{
    if (!num_exception)
    {
        return NULL;
    }

    if (hardware_init_gpio(&EXCEPTION_COUNTER.gpio_scs, GPIO_SCS)<0)
    {
        return NULL;   
    }

    const uint8_t exception_buffer_size = (num_exception/8) + !!(num_exception%8);
    struct EmergencyNode* const self = calloc(1, sizeof(*self) + exception_buffer_size);
    self->emergency_amount = num_exception;

    hardware_trap_attach_fun(EMERGENCY_RAISED_TRAP, raise_module_exception_state);
    hardware_trap_attach_fun(EMERGENCY_SOLVED_TRAP, solved_module_exception_state);

    return self;
}

void EmergencyNode_raise(struct EmergencyNode* const restrict self, const uint8_t exeception)
{
    const uint8_t exception_byte = exeception/8;
    const uint8_t exception_bit = exeception % 8;
    self->emergency_buffer[exception_byte] |= (1 << exception_bit);
    if (!self->emergency_state) {
        self->emergency_state = 1;
        hardware_raise_trap(EMERGENCY_RAISED_TRAP);
    }
}

void EmergencyNode_solve(struct EmergencyNode* const restrict self, const uint8_t exeception)
{
    const uint8_t exception_byte = exeception/8;
    const uint8_t exception_bit = 1 << (exeception % 8);
    if (self->emergency_buffer[exception_byte] | exception_bit) {
        self->emergency_buffer[exception_byte] ^= exception_bit;
    }

    if (self->emergency_state) {
        for (uint8_t i=0; i<self->emergency_amount; i++)
        {
            if (self->emergency_buffer[i])
            {
                return;
            }
        }
        self->emergency_state = 0;
        hardware_raise_trap(EMERGENCY_SOLVED_TRAP);
    }

}

uint8_t EmergencyNode_is_emergency_state(struct EmergencyNode* const restrict self)
{
    return self->emergency_state;
}

void EmergencyNode_free(struct EmergencyNode* const restrict self)
{
    free(self);
}
