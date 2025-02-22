#include "score_lib/test_lib.h"
#include "linux_board/linux_board.h"
#include "car_component/car_component.h"
#include "src/cores/core_2/feature/DV/ebs/ebs.h"

#include <stdint.h>
#include <stdio.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <unistd.h>

#define INIT_PH(init_exp, module_name)\
  if ((init_exp)<0)\
  {\
    FAILED("failed init "module_name);\
    goto end;\
  }

typedef struct CoreThread{
  thrd_t thread_id;
  uint8_t run;
}CoreThread;

typedef struct CoreInput{
  DvEbs_h* ebs;

  volatile const uint8_t* const core_run;
}CoreInput;

typedef struct{
  ExternalBoards_t* external_boards;

  DvEbs_h* ebs;
}TestInput;

static int _core_thread_fun(void* arg)
{
  CoreInput* core_input = arg;
  while (*core_input->core_run)
  {
    ebs_update(core_input->ebs);
  }
  return 0;
}

//public

void test_ebs(TestInput* t_input)
{
}

int main(void)
{
  ExternalBoards_t external_boards = {0};
  DvEbs_h ebs = {0};

  CoreThread core_thread={.run=1};
  CoreInput input =
  {
    .ebs = &ebs,

    .core_run = &core_thread.run,
  };

  TestInput t_input = {
    .external_boards = &external_boards,
    .ebs = &ebs,
  };

  INIT_PH(hardware_init_can(CAN_INVERTER, _1_MBYTE_S_), "can inverter");
  INIT_PH(hardware_init_can(CAN_GENERAL, _500_KBYTE_S_), "can general");
  INIT_PH(hardware_init_can(CAN_DV, _500_KBYTE_S_), "can dv");
  INIT_PH(create_virtual_chip(), "virtual chip gpio");

  INIT_PH(start_external_boards(&external_boards), "external_boards");

  INIT_PH(ebs_class_init(&ebs), "ebs")

  thrd_create(&core_thread.thread_id, _core_thread_fun, &input);

  test_ebs(&t_input);

  printf("tests finished\n");

  printf("stopping debug core\n");
  core_thread.run=0;
  thrd_join(core_thread.thread_id, NULL);

  stop_external_boards(&external_boards);
  stop_thread_can();
end:
  print_SCORE();
  return 0;
}
