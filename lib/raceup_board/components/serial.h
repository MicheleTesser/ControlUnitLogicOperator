#ifndef __VIRTUA__SERIAL__
#define __VIRTUA__SERIAL__

#include <stdint.h>
#include "./common_idx/common_idx.h"

extern int8_t hardware_init_serial(const BoardComponentId id);
extern int8_t serial_setup(const BoardComponentId id, const uint32_t freq);
extern int8_t serial_read(const BoardComponentId id, uint8_t* restrict const o_buffer,
        const uint32_t buffer_size);
extern int8_t serial_write(const BoardComponentId id, uint8_t* const restrict o_buffer,
       const uint32_t buffer_size);

#endif // !__VIRTUA__SERIAL__
