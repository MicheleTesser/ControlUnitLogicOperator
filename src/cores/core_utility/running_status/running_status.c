#include "running_status.h"
#include "../../../lib/raceup_board/raceup_board.h"

#include <stdatomic.h>
#include <stdint.h>



struct GlobalRunningStatusMut_t{
  uint8_t owner_id;
};

static struct{
  atomic_uchar global_car_status;
  atomic_uchar owner;
}RUNNING_STATUS_SHARED = 
{
  .global_car_status = SYSTEM_OFF,
};

#ifdef DEBUG
char __assert_size_global_running_status[(sizeof(GlobalRunningStatusMut_h)==sizeof(struct GlobalRunningStatusMut_t))?1:-1];
char __assert_align_global_running_status[(_Alignof(GlobalRunningStatusMut_h)==_Alignof(struct GlobalRunningStatusMut_t))?1:-1];
#endif /* ifdef DEBUG */

union GlobalRunningStatusMut_h_t_conv{
  GlobalRunningStatusMut_h* const hidden;
  struct GlobalRunningStatusMut_t* const clear;
};

//public

int8_t global_running_status_mut_init(
    GlobalRunningStatusMut_h* const restrict self __attribute__((__unused__)))
{
  const union GlobalRunningStatusMut_h_t_conv conv = {self};
  struct GlobalRunningStatusMut_t * const p_self = conv.clear;

  p_self->owner_id = (uint8_t) timer_time_now();
  if (!atomic_load(&RUNNING_STATUS_SHARED.owner))
  {
    atomic_store(&RUNNING_STATUS_SHARED.owner, p_self->owner_id);
    return 0;
  }

  return -1;
}
  

int8_t global_running_status_set(GlobalRunningStatusMut_h* const restrict self,
    const enum RUNNING_STATUS status)
{
  const union GlobalRunningStatusMut_h_t_conv conv = {self};
  struct GlobalRunningStatusMut_t* const p_self = conv.clear;

  if (p_self->owner_id == atomic_load(&RUNNING_STATUS_SHARED.owner))
  {
    atomic_store(&RUNNING_STATUS_SHARED.global_car_status, status);
    return 0;
  }

  return -1;
}

enum RUNNING_STATUS global_running_status_get(void)
{
  return atomic_load(&RUNNING_STATUS_SHARED.global_car_status);
}
