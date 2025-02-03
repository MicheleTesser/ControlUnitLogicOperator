#include "score_lib/test_lib.h"
#include "linux_board/linux_board.h"
#include "src/cores/core_utility/emergency_module/emergency_module.h"
#include <stdio.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <stdint.h>
#include <unistd.h>

static inline int8_t check_nodes(EmergencyNode_h** nodes, uint8_t node_num)
{
  uint8_t status = EmergencyNode_is_emergency_state(*nodes);
  for (uint8_t i=0; i<node_num; i++) {
    if (EmergencyNode_is_emergency_state(nodes[i]) != status) {
      return -1;
    }
  }
  return status;
}

static void test_initial_status(EmergencyNode_h* const restrict node_1, EmergencyNode_h* const restrict node_2)
{
  EmergencyNode_h* nodes[] = {node_1, node_2};
  if (!check_nodes(nodes, sizeof(nodes)/sizeof(nodes[0]))) {
    PASSED("initial state is emergency off");
  } else {
    FAILED("initial state is emergency on");
  }
}

static void test_raise_one_emergency(EmergencyNode_h* const restrict node_1, EmergencyNode_h* const restrict node_2)
{
  EmergencyNode_h* nodes[] = {node_1, node_2};
  if (EmergencyNode_raise(node_1, 1) < 0) {
    FAILED("emergency node raise failed on test raise one emergency");
    return;
  }

  if (check_nodes(nodes, sizeof(nodes)/sizeof(nodes[0]))) {
    PASSED("emergency raised recognized");
  }

  if (EmergencyNode_solve(node_1, 1) < 0) {
    FAILED("emergency node solve failed on test raise one emergency");
    return;
  }

  if (!check_nodes(nodes, sizeof(nodes)/sizeof(nodes[0]))) {
    PASSED("emergency clear recognized");
  }
}

static void test_multiple_emergencies(EmergencyNode_h* const restrict node_1, EmergencyNode_h* const restrict node_2)
{
  EmergencyNode_h* nodes[] = {node_1, node_2};
  EmergencyNode_raise(node_1, 1);
  EmergencyNode_raise(node_2, 2);
  
  if (check_nodes(nodes, sizeof(nodes)/sizeof(nodes[0]))) {
    PASSED("multiple emergencies recognized");
  }

  EmergencyNode_solve(node_1, 1);
  if (check_nodes(nodes, sizeof(nodes)/sizeof(nodes[0]))) {
    PASSED("emergency partially cleared");
  }

  EmergencyNode_solve(node_2, 2);
  if (!check_nodes(nodes, sizeof(nodes)/sizeof(nodes[0]))) {
    PASSED("all emergencies cleared");
  }
}

static void test_invalid_emergency(EmergencyNode_h* const restrict node)
{
  if (EmergencyNode_solve(node, -1) < 0) {
    PASSED("invalid emergency solve failed as expected");
  }

  if (EmergencyNode_raise(node, -1) < 0) {
    PASSED("invalid emergency raise failed as expected");
  }
}

int main(void)
{
  EmergencyNode_h emergency_node_1 = {0};
  EmergencyNode_h emergency_node_2 = {0};
  GpioRead_h gpio_scs = {0};

  if (create_virtual_chip() < 0) {
    goto end;
  }

  if (hardware_init_read_permission_gpio(&gpio_scs, GPIO_SCS) < 0) {
    goto end;
  }

  if (hardware_init_trap()<0) {
    goto end;
  }

  if (EmergencyNode_class_init() < 0) {
    goto end;
  }

  if (EmergencyNode_init(&emergency_node_1) < 0) {
    goto end;
  }

  if (EmergencyNode_init(&emergency_node_2) < 0) {
    goto end;
  }


  test_initial_status(&emergency_node_1, &emergency_node_2);
  test_raise_one_emergency(&emergency_node_1, &emergency_node_2);
  test_multiple_emergencies(&emergency_node_1, &emergency_node_2);
  test_invalid_emergency(&emergency_node_1);

end:
  print_SCORE();
  return 0;
}

