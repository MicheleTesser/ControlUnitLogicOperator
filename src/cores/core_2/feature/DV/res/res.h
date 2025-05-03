#ifndef __DV_RES__
#define __DV_RES__

#include <stdint.h>

typedef struct __attribute__((aligned(4))) {
  const uint8_t private_data[8];
}DvRes_h;

int8_t res_class_init(DvRes_h* const restrict self )__attribute__((__nonnull__(1)));
int8_t res_check_go(const DvRes_h* self )__attribute__((__nonnull__(1)));
int8_t res_start_time_go(DvRes_h* self )__attribute__((__nonnull__(1)));

#endif // !__DV_RES__
