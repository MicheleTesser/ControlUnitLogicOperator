#ifndef __EBS__
#define __EBS__

#include <stdint.h>

typedef struct __attribute__((aligned(8))){
  const uint8_t private_data[16];
}Ebs_h;

int8_t
ebs_start(Ebs_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t
ebs_stop(Ebs_h* const restrict self)__attribute__((__nonnull__(1)));

#endif // !__EBS__
