#include "serial.h"
#include "IfxPort.h"
#include "Bsp.h"
#include "../../Libraries/ControlUnitLogicOperator/lib/raceup_board/components/serial/serial.h"
#include <stdint.h>


int8_t hardware_init_serial(const uint16_t id)
{
    return 0;
}

int8_t serial_setup(const component_serial* const restrict self, const uint32_t freq)
{
    return 0;
}

int8_t serial_read(const component_serial* const restrict self, uint8_t* restrict const o_buffer,
        const uint32_t buffer_size)
{
    return 0;
}

int8_t serial_write(const component_serial* const restrict self, uint8_t* const restrict o_buffer,
       const uint32_t buffer_size)
{
    return 0;
}
