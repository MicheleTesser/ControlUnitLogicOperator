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


//public

int main(void)
{
  thrd_t core_0;
  thrd_t core_1;
  thrd_t core_2;

  if(create_virtual_chip()<0)
  {
    printf("failed init virtual chip\n");
    return -1;
  }

  thrd_create(&core_0, _core_0, NULL);
  thrd_create(&core_1, _core_1, NULL);
  thrd_create(&core_2, _core_2, NULL);

  sleep(1);

  hardware_can_node_debug_print_status();

  printf("print any key to stop\n");
  getchar();

  return 0;
}
