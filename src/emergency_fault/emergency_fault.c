#include "emergency_fault.h"
#include "../../lib/raceup_board/raceup_board.h"
#include "../board_conf/id_conf.h"
#include "../missions/missons.h"
#include <stdint.h>

//private

#define EMERGENCY_BUFFER_SIZE_MACRO __NUM_OF_EMERGENCY_FAULTS/8 + !!(__NUM_OF_EMERGENCY_FAULTS % 8)
const uint8_t EMERGENCY_BUFFER_SIZE = EMERGENCY_BUFFER_SIZE_MACRO;
static struct EmergencyController{
    uint8_t num_of_emergency[EMERGENCY_BUFFER_SIZE_MACRO];
    uint8_t mut_ptr :1;
    uint8_t read_ptr: 7;
}EMERGENCYS;

struct ErrorIndexArray {
    uint8_t emergency_buffer;
    uint8_t emergencY_bit;
};

static void 
get_error_index(const enum EMERGENCY_FAULT fault, struct ErrorIndexArray* const restrict o_buffer)
{
    o_buffer->emergency_buffer = fault/8;
    o_buffer->emergencY_bit = fault%8;
}

//public

const struct EmergencyController* emergency_get(void)
{
    while (EMERGENCYS.mut_ptr) {}
    EMERGENCYS.read_ptr++;
    return &EMERGENCYS;
}
struct EmergencyController* emergency_get_mut(void)
{
    while (EMERGENCYS.mut_ptr || EMERGENCYS.read_ptr) {}
    EMERGENCYS.mut_ptr++;
    return &EMERGENCYS;
}

int8_t one_emergency_raised(struct EmergencyController* const restrict self,
        const enum EMERGENCY_FAULT id)
{
    if (id == __NUM_OF_EMERGENCY_FAULTS) {
        return  -1;
    }
    struct ErrorIndexArray index;
    get_error_index(id, &index);
    self->num_of_emergency[index.emergency_buffer] |= (1 << index.emergencY_bit);
    gpio_set_low(SCS);

    return 0;
}

int8_t one_emergency_solved(struct EmergencyController* const restrict self,
        const enum EMERGENCY_FAULT id)
{
    if (id == __NUM_OF_EMERGENCY_FAULTS) {
        return  -1;
    }
    struct ErrorIndexArray index;
    get_error_index(id, &index);
    const uint8_t error_bit = (1 << index.emergencY_bit);
    if (self->num_of_emergency[index.emergency_buffer] & error_bit) {
        self->num_of_emergency[index.emergency_buffer]^= error_bit;
    }

    for (uint8_t i =0; i<EMERGENCY_BUFFER_SIZE; i++) {
        if (self->num_of_emergency[i]) {
            return 0;
        }
    }

    gpio_set_high(SCS);
    return 0;
}

int8_t is_emergency_state(const struct EmergencyController* const restrict self)
{
    for (uint8_t i=0;i<EMERGENCY_BUFFER_SIZE; i++) {
        const uint8_t f= self->num_of_emergency[i];
        if (f)
        {
            return f;
        }
    }
    return  0;
}

void emergency_free_ptr_read(void)
{
    EMERGENCYS.read_ptr--;
}

void emergency_free_ptr_mut(void)
{
    EMERGENCYS.mut_ptr--;
}
