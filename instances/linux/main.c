#include "linux_board/linux_board.h"
#include "src/cores/cores.h"

#include <stdint.h>
#include <stdio.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <unistd.h>

#define TEMPLACE_CORE(num)\
static inline int _core_##num (void* arg __attribute__((__unused__)))\
{\
  main_##num ();\
  return 0;\
}\

TEMPLACE_CORE(0)
TEMPLACE_CORE(1)
TEMPLACE_CORE(2)

static int _read_as_node(void* arg)
{
  int8_t new_status =0, old_status =0;
  GpioRead_h* p_as_node = arg;

  while((new_status = gpio_read_state(p_as_node))!=old_status)
  {
    old_status = new_status;
    printf("as node: %d\n", old_status);
  }

  return 0;
}

//public

int main(void)
{
  thrd_t core_0;
  thrd_t core_1;
  thrd_t core_2;

  thrd_t read_as_node;

  GpioRead_h as_node = {0};

  if(create_virtual_chip()<0)
  {
    printf("failed init virtual chip\n");
    return -1;
  }

  if (hardware_init_read_permission_gpio(&as_node, GPIO_AS_NODE)<0)
  {
    printf("failed init as node gpio\n");
    return -2;
  }

  thrd_create(&core_0, _core_0, NULL);
  thrd_create(&core_1, _core_1, NULL);
  thrd_create(&core_2, _core_2, NULL);

  thrd_create(&read_as_node, _read_as_node, &as_node);

  sleep(1);

  hardware_can_node_debug_print_status();

  printf("print any key to stop\n");
  getchar();

  return 0;
}
