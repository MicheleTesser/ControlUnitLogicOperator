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

  DvDriverInput_h* dv_driver_input;
  volatile const uint8_t* const core_run;
}CoreInput;

typedef struct{
  DvDriverInput_h* dv_driver_input;
  ExternalBoards_t* external_boards;
}TestInput;

static int _core_thread_fun(void* arg)
{
  CoreInput* core_input = arg;
  time_var_microseconds t =0;

  while (*core_input->core_run)
  {
    ACTION_ON_FREQUENCY(t, 1 MILLIS)
    {
      dv_driver_input_update(core_input->dv_driver_input);
    }
  }
  return 0;
}

void _status_check(TestInput* input, const uint8_t brake, char * check_operation)
{
  printf("%s with expected brake %d:\t",check_operation, brake);
  const uint8_t curr_brake = dv_driver_input_get_brake(input->dv_driver_input);
  if (curr_brake == brake)
  {
    PASSED("consistent state brake");
  }
  else
  {
    FAILED("inconsistent state brake");
    printf("given %d, expected: %d\n", curr_brake, brake);
  }

}

//public

#define UPDATE_AND_TEST(brake_value)\
{\
}

void test_dv_driver_input(TestInput* t_input __attribute_maybe_unused__)
{
  _status_check(t_input, 0, "initial state");

  for (uint8_t i=0; i<=100; i++)
  {
    embedded_system_set_dv_input(&t_input->external_boards->embedded_system, DV_INPUT_BRAKE, i);
    wait_milliseconds(65 MILLIS);
    _status_check(t_input, i, "update brake %");
  }

    embedded_system_set_dv_input(&t_input->external_boards->embedded_system, DV_INPUT_BRAKE, 110);
    wait_milliseconds(65 MILLIS);
    _status_check(t_input, 100, "overflow update brake");

    embedded_system_set_dv_input(&t_input->external_boards->embedded_system, DV_INPUT_BRAKE, 24);
    atc_pedals_steering_wheel(&t_input->external_boards->atc, ATC_BRAKE, 25);
    wait_milliseconds(100 MILLIS);
    _status_check(t_input, 25, "atc and embedded at the same time with atc bigger");

    embedded_system_set_dv_input(&t_input->external_boards->embedded_system, DV_INPUT_BRAKE, 30);
    atc_pedals_steering_wheel(&t_input->external_boards->atc, ATC_BRAKE, 25);
    wait_milliseconds(100 MILLIS);
    _status_check(t_input, 30, "atc and embedded at the same time with embedded bigger");
}

int main(void)
{
  ExternalBoards_t external_boards = {0};

  DvDriverInput_h driver_input = {0};

  CoreThread core_thread={.run=1};
  CoreInput input =
  {
    .dv_driver_input = &driver_input,

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

  pcu_start_embedded(&external_boards.pcu);

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
