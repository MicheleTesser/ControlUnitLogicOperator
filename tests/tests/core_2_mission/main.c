#include "score_lib/test_lib.h"
#include "linux_board/linux_board.h"
#include "./car_component/car_component.h"
#include "src/cores/core_utility/mission_locker/mission_locker.h"
#include "src/cores/core_2/feature/mission/mission.h"
#include "lib/board_dbc/dbc/out_lib/can2/can2.h"
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

typedef struct{
  DvMission_h* mission;

  int8_t runnig;
}ThInput ;

typedef struct{
  ExternalBoards_t* external_boards;
  DvMission_h* dv_mission;
  MissionLocker_h* mission_locker;
}TestInput;

static int _mission_loop(void *args)
{
  ThInput* input = args;

  while (dv_mission_init(input->mission)<0);

  while(input->runnig)
  {
    dv_mission_update(input->mission);
  }


  return 0;
}


static void _test_update_mission(const DvMission_h* const restrict self,
    const enum MISSIONS update_req,
    const enum MISSIONS expected)
{
  can_obj_can2_h_t o2={0};
  CanMessage mex={0};
  enum MISSIONS mission=0;
  struct CanNode* can_node = NULL;

  o2.can_0x067_CarMission.Mission = update_req;
  mex.id = CAN_ID_CARMISSION;
  mex.message_size = pack_message_can2(&o2, mex.id, &mex.full_word);
  ACTION_ON_CAN_NODE(CAN_GENERAL,can_node)
  {
    hardware_write_can(can_node, &mex);
  }

  wait_milliseconds(1 MILLIS);

  mission = dv_mission_get_current(self);
  if (mission == expected)
  {
    PASSED("test prevent update of mission with lock passed: ");
  }
  else{
    FAILED("test prevent update of mission with lock failed: ");
  }
  printf("updated request: %d, expected: %d, given :%d\n",update_req, expected, mission);
}

//public

static void test_default_mission(const TestInput* const restrict input)
{
  enum MISSIONS m = dv_mission_get_current(input->dv_mission);
  if (m == NONE)
  {
    PASSED("default mission is NONE");
  }
  else
  {
    FAILED("default mission is not NONE");
    printf("given: %d\n",m);
  }
}

void test_update_mission(const TestInput* const restrict input, const enum MISSIONS expected)
{
  can_obj_can2_h_t o2={0};
  CanMessage mex={0};
  enum MISSIONS mission=0;
  struct CanNode* can_node = NULL;

  o2.can_0x067_CarMission.Mission = expected;
  mex.id = CAN_ID_CARMISSION;
  mex.message_size = pack_message_can2(&o2, mex.id, &mex.full_word);
  ACTION_ON_CAN_NODE(CAN_GENERAL,can_node)
  {
    hardware_write_can(can_node, &mex);
  }
  wait_milliseconds(1 MILLIS);

  mission = dv_mission_get_current(input->dv_mission);
  if (mission == expected)
  {
    PASSED("test update of mission passed: ");
  }
  else{
    FAILED("test update of mission failed: ");
  }
  printf("expected: %d, given :%d\n", expected, mission);
}

void test_full_update_mission_unlocked(const TestInput* input)
{
  for (uint8_t i=0; i<__NUM_OF_MISSIONS__; i++)
  {
    _test_update_mission(input->dv_mission, i, i);
  }
}

void test_full_update_mission_locked(const TestInput* input)
{
  lock_mission(input->mission_locker);
  for (uint8_t i=0; i<__NUM_OF_MISSIONS__; i++)
  {
    _test_update_mission(input->dv_mission, i, __NUM_OF_MISSIONS__ -1);
  }
  unlock_mission(input->mission_locker);
}

int main(void)
{
  ExternalBoards_t external_boards = {0};

  MissionLocker_h locker={0};
  DvMission_h mission={0};
  thrd_t mission_thread=0;

  ThInput input =
  {
    .mission = &mission,
    .runnig = 1,
  };

  TestInput t_input ={
    .dv_mission = &mission,
    .external_boards = &external_boards,
    .mission_locker = &locker,
  };

  INIT_PH(hardware_init_can(CAN_INVERTER, _1_MBYTE_S_), "can inverter");
  INIT_PH(hardware_init_can(CAN_GENERAL, _500_KBYTE_S_), "can general");
  INIT_PH(hardware_init_can(CAN_DV, _500_KBYTE_S_), "can dv");
  INIT_PH(create_virtual_chip(), "virtual chip gpio");

  INIT_PH(start_external_boards(&external_boards), "external_boards");

  INIT_PH(dv_mission_init(&mission), "dv mission");
  while (lock_mission_ref_get_mut(&locker)<0);

  thrd_create(&mission_thread, _mission_loop, &input);

  test_default_mission(&t_input);
  test_full_update_mission_unlocked(&t_input);
  test_full_update_mission_locked(&t_input);


  printf("test finished\n");

  printf("stopping core update\n");
  input.runnig=0;
  thrd_join(mission_thread, NULL);
  lock_mission_ref_destroy_mut(&locker);

  stop_external_boards(&external_boards);
  stop_thread_can();
end:
  print_SCORE();
  return 0;
}
