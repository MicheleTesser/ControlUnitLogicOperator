#ifndef __DUMMY__
#define __DUMMY__

#include <stdint.h>

typedef struct __attribute__((aligned(4))){
  const uint8_t private_data[12];
}res_node_h;

int8_t
res_node_start(res_node_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t
res_node_stop(res_node_h* const restrict self)__attribute__((__nonnull__(1)));

#endif // !__DUMMY__
