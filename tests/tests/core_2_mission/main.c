#include "score_lib/test_lib.h"
#include "linux_board/linux_board.h"
#include "src/cores/core_utility/mission_locker/mission_locker.h"
#include "src/cores/core_2/feature/mission/mission.h"
#include "lib/board_dbc/dbc/out_lib/can2/can2.h"
#include <stdint.h>
#include <stdio.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <unistd.h>

char run=1;

static int mission_loop(void *args)
{
  DvMission_h* mission = args;

  while (dv_mission_init(mission)<0);

  for(;run;)
  {
    dv_mission_update(mission);
  }


  return 0;
}

static void test_default_mission(const DvMission_h* const restrict self)
{
  enum MISSIONS m = dv_mission_get_current(self);
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

static void test_update_mission(const DvMission_h* const restrict self, const enum MISSIONS expected)
{
  can_obj_can2_h_t o2={0};
  CanMessage mex={0};
  enum MISSIONS mission=0;

  o2.can_0x067_CarMission.Mission = expected;
  mex.id = CAN_ID_CARMISSION;
  mex.message_size = pack_message_can2(&o2, mex.id, &mex.full_word);
  ACTION_ON_CAN_NODE(CAN_GENERAL,{
    hardware_write_can(can_node, &mex);
  })
  wait_milliseconds(1 MILLIS);

  mission = dv_mission_get_current(self);
  if (mission == expected)
  {
    PASSED("test update of mission passed: ");
  }
  else{
    FAILED("test update of mission failed: ");
  }
  printf("expected: %d, given :%d\n", expected, mission);
}


static void test_update_mission_with_lock(const DvMission_h* const restrict self, const enum MISSIONS expected)
{
  can_obj_can2_h_t o2={0};
  CanMessage mex={0};
  enum MISSIONS mission=0;

  o2.can_0x067_CarMission.Mission = expected;
  mex.id = CAN_ID_CARMISSION;
  mex.message_size = pack_message_can2(&o2, mex.id, &mex.full_word);
  ACTION_ON_CAN_NODE(CAN_GENERAL,{
    hardware_write_can(can_node, &mex);
  })
  wait_milliseconds(1 MILLIS);

  mission = dv_mission_get_current(self);
  if (mission == __NUM_OF_MISSIONS__ -1)
  {
    PASSED("test prevent update of mission with lock passed: ");
  }
  else{
    FAILED("test prevent update of mission with lock failed: ");
  }
  printf("updated request: %d, expected: %d, given :%d\n",expected, __NUM_OF_MISSIONS__ -1, mission);
}

int main(void)
{
  MissionLocker_h locker={0};
  DvMission_h mission={0};
  thrd_t mission_thread=0;

  if(create_virtual_chip() <0){
    goto end;
  }

  if (virtual_can_manager_init()<0) {
    goto end;
  }

  if (hardware_init_can(CAN_GENERAL, _500_KBYTE_S_)<0)
  {
    goto end;
  }
  
  if (hardware_init_can(CAN_DV, _500_KBYTE_S_)<0)
  {
    goto end;
  }

  while (lock_mission_ref_get_mut(&locker)<0);
  thrd_create(&mission_thread, mission_loop, &mission);
  sleep(1);

  test_default_mission(&mission);
  for (uint8_t i=0; i<__NUM_OF_MISSIONS__; i++)
  {
    test_update_mission(&mission, i);
  }

  lock_mission(&locker);
  for (uint8_t i=0; i<__NUM_OF_MISSIONS__; i++)
  {
    test_update_mission_with_lock(&mission, i);
  }
  unlock_mission(&locker);

  run=0;
  thrd_join(mission_thread, NULL);
  lock_mission_ref_destroy_mut(&locker);
end:
  print_SCORE();
  return 0;
}
