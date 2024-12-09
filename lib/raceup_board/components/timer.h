#ifndef __VIRTUAL_TIMER__
#define __VIRTUAL_TIMER__

#include <stdint.h>
#include "./common_idx/common_idx.h"

extern int8_t hardware_init_timer(const BoardComponentId id);
extern int8_t wait_milliseconds(const BoardComponentId id, uint32_t time);

#endif // !__VIRTUAL_TIMER__
