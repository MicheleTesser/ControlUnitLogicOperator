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
  GlobalRunningStatus_h setter;
  GlobalRunningStatus_h invalid_setter;
  GlobalRunningStatus_h reader[5];

  _check_condition(!global_running_status_init(&setter, WRITE), "init tester");
  _check_condition(global_running_status_init(&invalid_setter, WRITE)==-2, "init invalid_setter failed");
  for (int i =0; i<5; i++)
  {
    _check_condition(!global_running_status_init(&reader[i], READ), "init reader");
  }

  _check_condition(global_running_status_get(&setter)==SYSTEM_OFF, "setter initial read is SYSTEM_OFF");
  for (int i =0; i<5; i++)
  {
  _check_condition(global_running_status_get(&reader[i])==SYSTEM_OFF, "reader initial read is SYSTEM_OFF");
  }

  _check_condition(!global_running_status_set(&setter, RUNNING),"tester update");
  for (int i =0; i<5; i++)
  {
  _check_condition(global_running_status_set(&reader[i],RUNNING)==-1, "reader try to set failed");
  }

  for (int i =0; i<5; i++)
  {
  _check_condition(global_running_status_get(&reader[i])==RUNNING, "reader read after update RUNNING");
  }

}

int main(void)
{
  printf("tests finished\n");

  test_normal_context();

  print_SCORE();
  return 0;
}
