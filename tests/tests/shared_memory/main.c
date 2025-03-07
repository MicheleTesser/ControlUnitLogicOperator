#include "score_lib/test_lib.h"
#include "linux_board/linux_board.h"
#include "src/cores/core_utility/shared_memory/shared_memory.h"

#include <stdint.h>
#include <stdio.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <unistd.h>

//public

int main(void)
{
  int a =5;
  int b = 10;
  int err=0;
  const void* temp_pointer=NULL;

  SharedDataId id_a = 10;
  SharedDataId id_b = 54;

  if (shared_memory_init(1)<0)
  {
    FAILED("failed init shared memory");
  }
  else
  {
    PASSED("init shared memory ok");
  }

  if((err = shared_memory_store_pointer(&a, id_a))<0)
  {
    FAILED("storing shared pointer failed for a");
    printf("error: %d\n",err);
  }
  else
  {
  PASSED("pointer var a stored correctly");
  }

  if((err = shared_memory_store_pointer(&b, id_b))<0)
  {
    FAILED("storing shared pointer failed for b");
    printf("error: %d\n",err);
  }
  else
  {
  PASSED("pointer var b stored correctly");
  }

  temp_pointer = shared_memory_fetch_pointer(id_a);
  if (temp_pointer && *(int *)temp_pointer == a)
  {
    PASSED("pointer a fetched correctly");
  }
  else
  {
    FAILED("pointer a fetch failed with err: ");
    printf("%p\n", temp_pointer);
  }

  temp_pointer = NULL;
  temp_pointer = shared_memory_fetch_pointer(id_b);
  if (temp_pointer && *(int *)temp_pointer == b)
  {
    PASSED("pointer b fetched correctly");
  }
  else
  {
    FAILED("pointer b fetch failed with err: ");
    printf("%p\n", temp_pointer);
  }

  temp_pointer = NULL;
  temp_pointer = shared_memory_fetch_pointer(107);
  if (!temp_pointer)
  {
    PASSED("invalid fetch pointer recognized");
  }
  else
  {
    FAILED("invalid fetch pointer not recognized");
  }

  print_SCORE();
  return 0;
}
