#include "score_lib/test_lib.h"
#include "linux_board/linux_board.h"
#include "car_component/car_component.h"
#include "src/cores/core_2/feature/dv_driver_input/dv_driver_input.h"

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
  volatile const uint8_t* const core_run;
}CoreInput;

typedef struct{
  DvDriverInput_h* dv_driver_input;
  ExternalBoards_t* external_boards;
}TestInput;

static int _core_thread_fun(void* arg)
{
  CoreInput* core_input = arg;
  while (*core_input->core_run)
  {
  }
  return 0;
}

//public

void test_dv_driver_input(TestInput* t_input __attribute_maybe_unused__)
{
}

int main(void)
{
  ExternalBoards_t external_boards = {0};

  DvDriverInput_h driver_input = {0};

  CoreThread core_thread={.run=1};
  CoreInput input =
  {
    .core_run = &core_thread.run,
  };

  TestInput t_input = {
    .external_boards = &external_boards,
    .dv_driver_input = &driver_input,
  };

  INIT_PH(hardware_init_can(CAN_INVERTER, _1_MBYTE_S_), "can inverter");
  INIT_PH(hardware_init_can(CAN_GENERAL, _500_KBYTE_S_), "can general");
  INIT_PH(hardware_init_can(CAN_DV, _500_KBYTE_S_), "can dv");
  INIT_PH(create_virtual_chip(), "virtual chip gpio");

  INIT_PH(start_external_boards(&external_boards), "external_boards");

  INIT_PH(dv_driver_input_init(&driver_input), "dv driver_input");

  thrd_create(&core_thread.thread_id, _core_thread_fun, &input);

  test_dv_driver_input(&t_input);

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
