#include "score_lib/test_lib.h"
#include "linux_board/linux_board.h"
#include "src/cores/core_0/feature/mission/mission.h"
#include <sys/cdefs.h>
#include <threads.h>
#include <stdint.h>
#include <unistd.h>

static int mission_loop(void* args __attribute_maybe_unused__)
{
  DriverInput_h driver={0};
  while(driver_input_init(&driver)<0);
  while (mission_init(args, &driver)<0);

  for(;;)
  {
    mission_update(args);
  }

  return 0;
}

int main(void)
{
  Mission_h mission={0};
  int8_t err=0;
  if(create_virtual_chip() <0){
    err--;
    goto end;
  }

  if (virtual_can_manager_init()<0) {
    goto end;
  }

  thrd_t miss;
  thrd_create(&miss, mission_loop, &mission);
  sleep(1);

end:
  print_SCORE();
  return err;
}
