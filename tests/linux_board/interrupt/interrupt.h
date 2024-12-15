#ifndef __LINUX_BOARD_INTERRUPT__
#define __LINUX_BOARD_INTERRUPT__

#include "../components/component.h"
#include <stdint.h>

void raise_interrupt(uint64_t interrupt_number);

#endif // !__LINUX_BOARD_INTERRUPT__
