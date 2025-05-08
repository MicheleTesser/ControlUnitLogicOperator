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
serial_write_uint8_t(const uint8_t __uint8_t);

extern int8_t
serial_write_uint16_t(const uint16_t __uint16_t);

extern int8_t
serial_write_uint32_t(const uint32_t __uint32_t);

extern int8_t
serial_write_int8_t(const int8_t __int8_t);

extern int8_t
serial_write_int16_t(const int16_t __int16_t);

extern int8_t
serial_write_int32_t(const int32_t __int32_t);

extern int8_t
serial_write_raw(const char* const restrict str)__attribute__((__nonnull__));

#endif // !__VIRTUA__SERIAL__
