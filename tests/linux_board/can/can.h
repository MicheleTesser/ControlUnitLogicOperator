#ifndef __LINUX_BOARD_CAN__
#define __LINUX_BOARD_CAN__

#include "./raceup_board/components/can.h"

void
stop_thread_can(void);

struct CanNode*
hardware_init_new_external_node(const enum CAN_MODULES mod);

void
hardware_init_new_external_node_destroy(struct CanNode* const restrict self)
  __attribute__((__nonnull__(1)));

void
hardware_can_debug_print_status(struct CanMailbox* const restrict self)
  __attribute__((__nonnull__(1)));


#define ACTION_ON_CAN_NODE_EXTERNAL(node, p_node)\
  for (p_node =NULL;!p_node;p_node = hardware_init_new_external_node(node)){}\
  for (uint8_t node_used=0;!node_used;(hardware_init_new_external_node_destroy(p_node), node_used=1))

#endif // !__LINUX_BOARD_CAN__
