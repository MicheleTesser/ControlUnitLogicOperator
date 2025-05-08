#ifndef __VIRTUA__SERIAL__
#define __VIRTUA__SERIAL__

#include <stdint.h>
#include "./common_idx/common_idx.h"

extern int8_t
hardware_init_serial(void);

extern int8_t
serial_setup(const uint32_t freq);

extern int8_t
serial_read(uint8_t* restrict const o_buffer, const uint32_t buffer_size)
  __attribute__((__nonnull__));

extern int8_t
serial_write_str(const char* const restrict str)__attribute__((__nonnull__));

extern int8_t
serial_write_raw(const char* const restrict str)__attribute__((__nonnull__));

#endif // !__VIRTUA__SERIAL__
