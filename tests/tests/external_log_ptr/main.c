#include "score_lib/test_lib.h"
#include "src/cores/core_1/feature/log/external_log_variables/external_log_variables.h"

#include <stdint.h>
#include <stdio.h>
//public

void test(void)
{
  uint8_t vars[__NUM_OF_SHARED_ID__];

  for (uint8_t i=0; i<__NUM_OF_SHARED_ID__; i++)
  {
    vars[i] = i+1;
    if(external_log_variables_store_pointer(&vars[i], i)<0)
    {
      FAILED("store ptr failed on var: ");
    }
    else
    {
      PASSED("store ptr passed on var: ");
    }
    printf("%d\n", i);
  }

  for (uint8_t i=0; i<__NUM_OF_SHARED_ID__; i++)
  {
    const uint8_t* ptr = external_log_extract_data_ptr_r_only(i);
    if (*ptr != vars[i])
    {
      FAILED("ptr extracted differs: ");
    }
    else
    {
      PASSED("ptr extracted ok: ");
    }
    printf("given: %d, expected: %d\n", *ptr, vars[i]);
  }
}

int main(void)
{
  test();
  printf("tests finished\n");
  print_SCORE();
  return 0;
}
