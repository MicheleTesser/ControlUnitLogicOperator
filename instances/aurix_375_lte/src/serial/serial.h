#ifndef __AURIX_SERIAL__
#define __AURIX_SERIAL__

#include <stdint.h>

typedef struct component_serial component_serial;

int8_t hardware_init_serial(const uint16_t id);
int8_t serial_setup(const component_serial* const restrict self, const uint32_t freq);

int8_t serial_read(const component_serial* const restrict self, uint8_t* restrict const o_buffer,
        const uint32_t buffer_size);

int8_t serial_write(const component_serial* const restrict self, uint8_t* const restrict o_buffer,
       const uint32_t buffer_size);

#endif // !__AURIX_SERIAL__
