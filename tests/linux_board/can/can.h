#ifndef __LINUX_BOARD_CAN__
#define __LINUX_BOARD_CAN__

#include "./raceup_board/components/can.h"
void stop_thread_can(void);

struct CanNode*
hardware_init_can_get_ref_node_new(const enum CAN_MODULES mod);

void
hardware_init_can_get_ref_node_destroy(struct CanNode* const restrict self)
  __attribute__((__nonnull__(1)));


#endif // !__LINUX_BOARD_CAN__
