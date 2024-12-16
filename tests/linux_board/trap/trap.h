#ifndef __LINUX_BOARD_TRAP__
#define __LINUX_BOARD_TRAP__

#include "../components/component.h"
#include <stdint.h>

void raise_trap(const uint8_t trap_number);

#endif // !__LINUX_BOARD_TRAP__
