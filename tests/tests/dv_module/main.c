#include "score_lib/test_lib.h"
#include "linux_board/linux_board.h"
#include "car_component/car_component.h"
#include "src/cores/core_2/feature/DV/dv.h"
#include "src/cores/core_2/feature/dv_driver_input/dv_driver_input.h"
#include "src/cores/core_utility/mission_reader/mission_reader.h"

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

  CarMissionReader_h* p_car_mission;
  DvDriverInput_h* p_dv_driver_input;
  Dv_h* p_dv;

}CoreInput;

typedef struct{
  ExternalBoards_t* external_boards;

  Dv_h* dv;
}TestInput;

static int _core_thread_fun(void* arg)
{
  CoreInput* core_input = arg;
  while (*core_input->core_run)
  {
    car_mission_reader_update(core_input->p_car_mission);
    dv_driver_input_update(core_input->p_dv_driver_input);
    dv_update(core_input->p_dv);
  }
  return 0;
}

//public

void test_dv_module_update(TestInput* t_input)
{
  steering_wheel_select_mission(&t_input->external_boards->steering_wheel, CAR_MISSIONS_DV_TRACKDRIVE);
  wait_milliseconds(100 MILLIS);
}

int main(void)
{
  ExternalBoards_t external_boards = {0};
  CarMissionReader_h mission_reader = {0};
  DvDriverInput_h dv_driver_input = {0};
  Dv_h dv={0};

  GpioRead_h gpio_dv_yellow_light = {0};
  GpioRead_h gpio_dv_blue_light = {0};
  GpioRead_h gpio_dv_emergency_sound = {0};

  CoreThread core_thread={.run=1};
  CoreInput input =
  {
    .core_run = &core_thread.run,

    .p_car_mission = &mission_reader,
    .p_dv_driver_input = &dv_driver_input,
    .p_dv = &dv,
  };

  TestInput t_input = {
    .external_boards = &external_boards,

    .dv = &dv,
  };

  INIT_PH(hardware_init_can(CAN_INVERTER, _1_MBYTE_S_), "can inverter");
  INIT_PH(hardware_init_can(CAN_GENERAL, _500_KBYTE_S_), "can general");
  INIT_PH(hardware_init_can(CAN_DV, _500_KBYTE_S_), "can dv");
  INIT_PH(create_virtual_chip(), "virtual chip gpio");

  INIT_PH(start_external_boards(&external_boards), "external_boards");

  INIT_PH(hardware_init_read_permission_gpio(&gpio_dv_blue_light, GPIO_ASSI_LIGHT_BLU), "dv light blue");
  INIT_PH(hardware_init_read_permission_gpio(&gpio_dv_yellow_light, GPIO_ASSI_LIGHT_YELLOW), "dv light yellow");
  INIT_PH(hardware_init_read_permission_gpio(&gpio_dv_emergency_sound, GPIO_RTD_ASSI_SOUND), "dv emergency sound");
  INIT_PH(car_mission_reader_init(&mission_reader), "mission_reader");
  INIT_PH(dv_driver_input_init(&dv_driver_input), "dv driver input");
  INIT_PH(dv_class_init(&dv, &mission_reader, &dv_driver_input), "dv module");

  thrd_create(&core_thread.thread_id, _core_thread_fun, &input);

  test_dv_module_update(&t_input);

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
