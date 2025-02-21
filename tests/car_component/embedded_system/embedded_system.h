#ifndef __EMBEDDED_SYSTEM__
#define __EMBEDDED_SYSTEM__

#include <stdint.h>

typedef struct __attribute__((aligned(8))) EmbeddedSystem_h{
  const uint8_t private_data[16];
}EmbeddedSystem_h;

int8_t
embedded_system_start(EmbeddedSystem_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t
embedded_system_stop(EmbeddedSystem_h* const restrict self)__attribute__((__nonnull__(1)));

#endif // !__EMBEDDED_SYSTEM__
