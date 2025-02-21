#ifndef __DUMMY__
#define __DUMMY__

#include <stdint.h>

typedef struct __attribute__((aligned(8))) Dummy_h{
  const uint8_t private_data[16];
}Dummy_h;

int8_t
dummy_start(struct Dummy_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t
dummy_stop(struct Dummy_h* const restrict self)__attribute__((__nonnull__(1)));

#endif // !__DUMMY__
