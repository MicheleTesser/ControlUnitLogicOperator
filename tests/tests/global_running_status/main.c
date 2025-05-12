#include "score_lib/test_lib.h"
#include "src/cores/core_utility/running_status/running_status.h"

#include <stdint.h>
#include <stdio.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <unistd.h>

#define _check_condition(bool_exp, cond_str)\
{\
  (bool_exp)?PASSED("passed: "cond_str):FAILED("failed: "cond_str);\
}

//public


void test_normal_context(void)
{
  GlobalRunningStatusMut_h setter;
  GlobalRunningStatusMut_h invalid_setter;

  _check_condition(!global_running_status_mut_init(&setter), "init tester");
  _check_condition(global_running_status_mut_init(&invalid_setter)==-1, "init invalid_setter failed");

  _check_condition(global_running_status_get()==SYSTEM_OFF, "initial read is SYSTEM_OFF");

  _check_condition(!global_running_status_set(&setter, RUNNING),"tester update");
  _check_condition(global_running_status_set(&invalid_setter,RUNNING)==-1, "reader try to set failed");
}

int main(void)
{
  printf("tests finished\n");

  test_normal_context();

  print_SCORE();
  return 0;
}
