#include "score_lib/test_lib.h"
#include "linux_board/linux_board.h"
#include "src/cores/core_utility/shared_memory/shared_memory.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <unistd.h>

//public

int main(void)
{
  int err=0;
  const void* temp_pointer=NULL;
  const unsigned long vars_number = 100;
  SharedDataId vars_id[vars_number];
  short vars[vars_number];

  for(unsigned long i=0;i < vars_number;++i)
  {
    vars_id[i] = i;
    vars[i] = i * 20;
  }


  if (shared_memory_init(1)<0)
  {
    FAILED("failed init shared memory");
  }
  else
  {
    PASSED("init shared memory ok");
  }

  for(unsigned long i=0;i < (sizeof(vars)/sizeof(vars[0]));++i)
  {
    if((err = shared_memory_store_pointer(&vars[i], vars_id[i]))<0)
    {
      FAILED("storing shared pointer: ");
    }
    else
    {
      PASSED("storing shared pointer: ");
    }
    printf("var: %lu, id: %d, value %d\n",i, vars_id[i], vars[i]);
  }

  for(unsigned long i=0;i < (sizeof(vars)/sizeof(vars[0]));++i)
  {
    vars[i] %= vars_number;
    temp_pointer = shared_memory_fetch_pointer(vars_id[i]);
    if (temp_pointer && vars[i] == *(short *) temp_pointer)
    {
      PASSED("var fetched correctly: ");
    }
    else
    {
      FAILED("var not fetched correctly: ");
    }
    printf("\
        var: %lu, var_id: %d.\n\
        given: [var_value: %d], expected: [var_value: %d]\n",
        i, vars_id[i], *(short *)temp_pointer, vars[i]);
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
