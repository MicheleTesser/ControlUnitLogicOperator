#include "score_lib/test_lib.h"
#include "linux_board/linux_board.h"
#include "car_component/car_component.h"
#include "src/cores/core_utility/core_utility.h"

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

typedef struct TestInput{
  CarMissionReader_h* mission_reader;
  SteeringWheel_h* stw;
  MissionLocker_h* locker;
  ExternalBoards_t* external_boards;

}TestInput;

typedef struct CoreInput{
  CarMissionReader_h* mission_reader;
  SharedMessageOwner_h* shared_messages;
  SytemSettingOwner_h* system_setting;

  volatile const uint8_t* const core_run;
}CoreInput;

static int _core_thread_fun(void* arg)
{
  CoreInput* core_input = arg;

  while (*core_input->core_run)
  {
    car_mission_reader_update(core_input->mission_reader);
    shared_message_owner_update(core_input->shared_messages);
    system_settings_update(core_input->system_setting);
  }
  return 0;
}

static void _test_status_mission(TestInput* input,
    enum CAR_MISSIONS expected, enum CAR_MISSIONS given)
{
  steering_wheel_select_mission(input->stw, given);
  wait_milliseconds(get_tick_from_millis(200));
  steering_wheel_select_mission(input->stw, given);
  wait_milliseconds(get_tick_from_millis(200));
  uint8_t m = car_mission_reader_get_current_mission(input->mission_reader);
  if (m==expected)
  {
    PASSED("mission status ok");
  }
  else
  {
    FAILED("mission status failed");
  }
  printf("expected: %d, given: %d\n", expected, m);
}

//public

void test_mission(TestInput* input)
{
  pcu_start_embedded(&input->external_boards->pcu);
  printf("testing update of all missions without lock\n");
  for (enum CAR_MISSIONS i=0; i<__NUM_OF_CAR_MISSIONS__; i++)
  {
    _test_status_mission(input, i, i);
  }

  steering_wheel_select_mission(input->stw, CAR_MISSIONS_NONE);
  wait_milliseconds(get_tick_from_millis(100));
  lock_mission(input->locker);

  printf("testing update of mission with lock\n");
  for (enum CAR_MISSIONS i=0; i<__NUM_OF_CAR_MISSIONS__; i++)
  {
    _test_status_mission(input, CAR_MISSIONS_NONE, i);
  }

  unlock_mission(input->locker);

  printf("testing update of mission after unlock\n");
  for (enum CAR_MISSIONS i=0; i<__NUM_OF_CAR_MISSIONS__; i++)
  {
    _test_status_mission(input, i, i);
  }

}

int main(void)
{
  ExternalBoards_t external_boards = {0};
  CarMissionReader_h mission_reader = {0};
  MissionLocker_h mission_locker = {0};
  SharedMessageOwner_h shared_messages = {0};
  SytemSettingOwner_h system_settings ={0};

  CoreThread core_thread={.run=1};
  CoreInput input =
  {
    .core_run = &core_thread.run,
    .mission_reader = &mission_reader,
    .shared_messages = &shared_messages,
    .system_setting = &system_settings,
  };

  TestInput t_input = {
    .mission_reader = &mission_reader,
    .locker = &mission_locker,
    .external_boards = &external_boards,
    .stw = &external_boards.steering_wheel,
  };

  INIT_PH(hardware_init_can(CAN_INVERTER, _1_MBYTE_S_), "can inverter");
  INIT_PH(hardware_init_can(CAN_GENERAL, _500_KBYTE_S_), "can general");
  INIT_PH(hardware_init_can(CAN_DV, _500_KBYTE_S_), "can dv");
  INIT_PH(create_virtual_chip(), "virtual chip gpio");

  INIT_PH(start_external_boards(&external_boards), "external_boards");

  INIT_PH(system_settings_init(&system_settings), "system_settings");
  INIT_PH(shared_message_owner_init(&shared_messages), "shared_messages");
  INIT_PH(car_mission_reader_init(&mission_reader), "mission_reader");
  INIT_PH(lock_mission_ref_get_mut(&mission_locker), "mission locker");

  thrd_create(&core_thread.thread_id, _core_thread_fun, &input);

  test_mission(&t_input);

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
