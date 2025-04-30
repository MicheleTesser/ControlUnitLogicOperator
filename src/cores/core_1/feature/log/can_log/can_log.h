#ifndef __CAN_LOG__
#define __CAN_LOG__

#include <stdint.h>

typedef struct __attribute__((aligned(4))){
  const uint8_t private_data[44];
}CanLog_h;

int8_t can_log_init(CanLog_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t can_log_update(CanLog_h* const restrict p_self)__attribute__((__nonnull__(1)));

#endif // !__CAN_LOG__
