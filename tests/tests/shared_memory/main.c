#include "score_lib/test_lib.h"
#include "linux_board/linux_board.h"
#include "src/cores/core_1/feature/log/external_log_variables/external_log_variables.h"

#include <stdint.h>
#include <stdio.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <unistd.h>

//public

int main(void)
{
  int err=0;
  const unsigned long vars_number = __NUM_OF_SHARED_ID__;
  SharedDataId vars_id[vars_number];
  short vars[vars_number];

  for(unsigned long i=0;i < vars_number;++i)
  {
    vars_id[i] = i;
    vars[i] = i * 20;
  }


  for(unsigned long i=0;i < (sizeof(vars)/sizeof(vars[0]));++i)
  {
    if((err = external_log_variables_store_pointer(&vars[i], vars_id[i]))<0)
    {
      FAILED("storing shared pointer: ");
    }
    else
    {
      PASSED("storing shared pointer: ");
    }
    printf("var: %lu, id: %d, value %d\n",i, vars_id[i], vars[i]);
  }

  print_SCORE();
  return 0;
}
