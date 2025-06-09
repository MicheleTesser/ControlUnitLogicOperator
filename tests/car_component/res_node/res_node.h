#ifndef __RES_NODE__
#define __RES_NODE__

#include <stdint.h>

typedef struct __attribute__((aligned(4))){
  const uint8_t private_data[41];
}res_node_h;

int8_t
res_node_start(res_node_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t
res_node_stop(res_node_h* const restrict self)__attribute__((__nonnull__(1)));

#endif // !__RES_NODE__
