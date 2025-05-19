#include <stdio.h>

#include "src/cores/core_utility/core_utility.h"

//public

void test_empty_trace(void)
{
  errno_trace_print(CORE_0);
  errno_trace_print(CORE_1);
  errno_trace_print(CORE_2);

  errno_trace_clear(CORE_0);
  errno_trace_clear(CORE_1);
  errno_trace_clear(CORE_2);
}

void test_level_trace(void)
{
  //trace core 0
  errno_trace_push_trace(CORE_0, -1, "first layer");
  errno_trace_push_trace(CORE_0, -4, "second layer");
  errno_trace_push_trace(CORE_0, -25, "third layer");
  errno_trace_print(CORE_0);
  //trace core 1
  errno_trace_push_trace(CORE_1, -1, "first layer");
  errno_trace_push_trace(CORE_1, -4, "second layer");
  errno_trace_print(CORE_1);
  //trace core 2
  errno_trace_push_trace(CORE_2, -6, "first layer");
  errno_trace_print(CORE_2);

  printf("clearing all\n");
  errno_trace_clear(CORE_0);
  errno_trace_clear(CORE_1);
  errno_trace_clear(CORE_2);

  errno_trace_print(CORE_0);
  errno_trace_print(CORE_1);
  errno_trace_print(CORE_2);
}

int main(void)
{
  test_empty_trace();
  test_level_trace();
  return 0;
}
