#include "score_lib/test_lib.h"
#include "linux_board/linux_board.h"
#include "src/cores/core_utility/mission_locker/mission_locker.h"
#include "src/cores/core_2/feature/mission/mission.h"
#include <stdint.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <unistd.h>

static int run=1;
static int mission_loop(void *args)
{
  DvMission_h* mission = args;

  while (dv_mission_init(mission)<0);

  while (run)
  {
    dv_mission_update(mission);
  }


  return 0;
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

  while (lock_mission_ref_get_mut(&locker)<0);
  thrd_create(&mission_thread, mission_loop, &mission);


end:
  print_SCORE();
  return 0;
}
