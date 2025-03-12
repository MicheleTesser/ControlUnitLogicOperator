#include "serial.h"
#include "IfxPort.h"
#include "Bsp.h"
#include "../raceup_board/components/serial.h"
#include <stdint.h>

int8_t hardware_init_serial(void)
{
}

int8_t serial_setup(const uint32_t freq)
{
    return 0;
}

int8_t serial_read(uint8_t* restrict const o_buffer,
        const uint32_t buffer_size)
{
    return 0;
}

int8_t serial_write_str(const char* const restrict buffer)
{
    return 0;
}
