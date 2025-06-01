#include "score_lib/test_lib.h"
#include "linux_board/linux_board.h"
#include "car_component/car_component.h"
#include "src/cores/core_utility/core_utility.h"
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

#define _check_status(bool_exp, check_descr)\
  (bool_exp)?PASSED(check_descr):FAILED(check_descr);

typedef struct{
  thrd_t thread_id;
  uint8_t run;
}CoreThread;

typedef struct{
  AsNode_h* p_as_node;
  CarMissionReader_h* p_car_mission_reader;
  SharedMessageOwner_h* p_shared_messages;
  SytemSettingOwner_h* system_settings;

  volatile const uint8_t* const core_run;
}CoreInput;

typedef struct{
  ExternalBoards_t* external_boards;
  AsNode_h* p_as_node;
  AsNodeRead_h* p_as_node_read;
  GpioRead_h* p_gpio_as_node;
}TestInput;

static int _core_thread_fun(void* arg)
{
  CoreInput* core_input = arg;
  while (*core_input->core_run)
  {
    shared_message_owner_update(core_input->p_shared_messages);
    car_mission_reader_update(core_input->p_car_mission_reader);
    as_node_update(core_input->p_as_node);
    system_settings_update(core_input->system_settings);
  }
  return 0;
}

//public

void test_as_node_mission_none(TestInput* t_input)
{
  steering_wheel_select_mission(&t_input->external_boards->steering_wheel, CAR_MISSIONS_NONE);
  wait_milliseconds(get_tick_from_millis(200));

  _check_status(!as_node_get_status(t_input->p_as_node) &&
      !gpio_read_state(t_input->p_gpio_as_node),
      "as node not closed in mission none");
  _check_status(!as_node_read_get_status(t_input->p_as_node_read)
      && !gpio_read_state(t_input->p_gpio_as_node)
      ,"as node read status in mission none");
}

void test_as_node_mission_human(TestInput* t_input)
{
  steering_wheel_select_mission(&t_input->external_boards->steering_wheel, CAR_MISSIONS_HUMAN);
  wait_milliseconds(get_tick_from_millis(200));

  _check_status(as_node_get_status(t_input->p_as_node) &&
      gpio_read_state(t_input->p_gpio_as_node),
      "as node not closed in mission human at initial state");
  _check_status(as_node_read_get_status(t_input->p_as_node_read)
      && gpio_read_state(t_input->p_gpio_as_node)
      ,"as node read status in mission human at initial state");
}

void test_as_node_mission_dv(TestInput* t_input)
{
  steering_wheel_select_mission(&t_input->external_boards->steering_wheel, CAR_MISSIONS_DV_SKIDPAD);
  wait_milliseconds(get_tick_from_millis(200));

  _check_status(!as_node_get_status(t_input->p_as_node) &&
      !gpio_read_state(t_input->p_gpio_as_node),
      "as node not closed in mission dv at initial state");
  _check_status(!as_node_read_get_status(t_input->p_as_node_read)
      && !gpio_read_state(t_input->p_gpio_as_node)
      ,"as node read status in mission dv at initial state");
}

int main(void)
{
  GpioRead_h gpio_as_node = {0};
  CarMissionReader_h car_mission_reader = {0};
  ExternalBoards_t external_boards = {0};
  AsNode_h as_node = {0};
  AsNodeRead_h as_node_read = {0};
  SharedMessageOwner_h shared_messages = {0};
  SytemSettingOwner_h system_settings = {0};

  CoreThread core_thread={.run=1};
  CoreInput input =
  {
    .core_run = &core_thread.run,
    .p_as_node = &as_node,
    .p_car_mission_reader = &car_mission_reader,
    .p_shared_messages = &shared_messages,
    .system_settings = &system_settings,
  };
  TestInput test_input = 
  {
    .external_boards = &external_boards,
    .p_as_node = &as_node,
    .p_as_node_read = &as_node_read,
    .p_gpio_as_node = &gpio_as_node,
  };

  INIT_PH(hardware_init_can(CAN_INVERTER, _1_MBYTE_S_), "can inverter");
  INIT_PH(hardware_init_can(CAN_GENERAL, _500_KBYTE_S_), "can general");
  INIT_PH(hardware_init_can(CAN_DV, _500_KBYTE_S_), "can dv");
  INIT_PH(create_virtual_chip(), "virtual chip gpio");

  INIT_PH(start_external_boards(&external_boards), "external_boards");

  INIT_PH(system_settings_init(&system_settings), "system_settings");
  INIT_PH(shared_message_owner_init(&shared_messages), "shared_messages");
  INIT_PH(hardware_init_read_permission_gpio(&gpio_as_node, GPIO_AS_NODE), "gpio read Ts button");
  INIT_PH(car_mission_reader_init(&car_mission_reader), "car mission reader");
  INIT_PH(as_node_init(&as_node, &car_mission_reader), "as node mutable");
  INIT_PH(as_node_read_init(&as_node_read), "as node read");

  thrd_create(&core_thread.thread_id, _core_thread_fun, &input);

  test_as_node_mission_none(&test_input);
  test_as_node_mission_human(&test_input);
  test_as_node_mission_dv(&test_input);

  printf("tests finished\n");

  printf("stopping debug core\n");
  core_thread.run=0;
  thrd_join(core_thread.thread_id, NULL);

  hardware_can_node_debug_print_status();
  stop_external_boards(&external_boards);
  stop_thread_can();
end:
  print_SCORE();
  return 0;
}
