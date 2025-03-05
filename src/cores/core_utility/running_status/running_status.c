#include "running_status.h"
#include <stdatomic.h>
#include <stdint.h>

static atomic_uchar global_car_status = SYSTEM_OFF;
static atomic_bool mut_requested=0;

struct GlobalRunningStatus_t{
  uint8_t global_car_status_copy;
  enum RUNNING_STATUS_OBJ_PERMISSIONS permission;
};

#ifdef DEBUG
char __assert_size_global_running_status[(sizeof(GlobalRunningStatus_h)==sizeof(struct GlobalRunningStatus_t))?1:-1];
char __assert_align_global_running_status[(_Alignof(GlobalRunningStatus_h)==_Alignof(struct GlobalRunningStatus_t))?1:-1];
#endif /* ifdef DEBUG */

union GlobalRunningStatus_h_t_conv{
  GlobalRunningStatus_h* const hidden;
  struct GlobalRunningStatus_t* const clear;
};

//private
enum RUNNING_STATUS _global_running_status_read_raw(void)
{
  return atomic_load(&global_car_status);
}

//public

int8_t global_running_status_init(GlobalRunningStatus_h* const restrict self,
    const enum RUNNING_STATUS_OBJ_PERMISSIONS permission)
{
  const union GlobalRunningStatus_h_t_conv conv = {self};
  struct GlobalRunningStatus_t* const p_self = conv.clear;

  if (permission >= __NUM_OF_RUNNING_STATUS_OBJ_PERMISSIONS__)
  {
    return -1;
  }

  if (permission == WRITE)
  {
    if(!atomic_exchange(&mut_requested, 1))
    {
      p_self->permission = permission;
      p_self->global_car_status_copy = _global_running_status_read_raw();
    }
    else
    {
      return -2;
    }
  
  }

  p_self->permission = permission;
  p_self->global_car_status_copy = _global_running_status_read_raw();


  return 0;
}

int8_t global_running_status_set(GlobalRunningStatus_h* const restrict self,
    const enum RUNNING_STATUS status)
{
  const union GlobalRunningStatus_h_t_conv conv = {self};
  struct GlobalRunningStatus_t* const p_self = conv.clear;

  if (p_self->permission != WRITE)
  {
    return -1;
  }

  p_self->global_car_status_copy = status;
  atomic_store(&global_car_status, status);
  return 0;
}

enum RUNNING_STATUS global_running_status_get(GlobalRunningStatus_h* const restrict self)
{
  const union GlobalRunningStatus_h_t_conv conv = {self};
  struct GlobalRunningStatus_t* const p_self = conv.clear;
  if (p_self->permission == WRITE)
  {
    return p_self->global_car_status_copy;
  }
  return _global_running_status_read_raw();
}
