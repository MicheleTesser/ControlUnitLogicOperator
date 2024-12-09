#include "serial.h"
#include "IfxPort.h"
#include "Bsp.h"
#include "../../Libraries/ControlUnitLogicOperator/lib/raceup_board/components/serial.h"
#include <stdint.h>


int8_t hardware_init_serial(const BoardComponentId id)
{
    return 0;
}

int8_t serial_setup(const BoardComponentId id, const uint32_t freq)
{
    return 0;
}

int8_t serial_read(const BoardComponentId id, uint8_t* restrict const o_buffer,
        const uint32_t buffer_size)
{
    return 0;
}

int8_t serial_write(const BoardComponentId id, uint8_t* const restrict o_buffer,
       const uint32_t buffer_size)
{
    return 0;
}
