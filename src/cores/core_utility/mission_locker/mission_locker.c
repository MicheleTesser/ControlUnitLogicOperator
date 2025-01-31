#include "mission_locker.h"
#include <stdatomic.h>
#include <string.h>

static struct{
  atomic_bool mission_lock;
  atomic_bool taken;
}MISSION_LOCKER;

struct MissionLockerRead_t{
  const atomic_bool* mission_lock;
};

struct MissionLocker_t{
  atomic_bool* mission_lock;
};


union MissionLocker_h_t_conv{
  MissionLocker_h* const restrict hidden;
  struct MissionLocker_t* const restrict clear;
};

union MissionLockerRead_h_t_conv{
  MissionLockerRead_h* const restrict hidden;
  struct MissionLockerRead_t* const restrict clear;
};

union MissionLockerRead_h_t_conv_const{
  const MissionLockerRead_h* const restrict hidden;
  const struct MissionLockerRead_t* const restrict clear;
};

int8_t lock_mission_ref_get(MissionLockerRead_h* const restrict self)
{
  union MissionLockerRead_h_t_conv conv = {self};
  struct MissionLockerRead_t* p_self = conv.clear;
  p_self->mission_lock = &MISSION_LOCKER.mission_lock;
  return -1;
}
int8_t lock_mission_ref_get_mut(MissionLocker_h* const restrict self)
{
  union MissionLocker_h_t_conv conv = {self};
  struct MissionLocker_t* const restrict p_self = conv.clear;
  if (!atomic_exchange(&MISSION_LOCKER.taken, 1)) {
  
    p_self->mission_lock = &MISSION_LOCKER.mission_lock;
    return 0;
  }
  atomic_store(&MISSION_LOCKER.taken,0);
  return -1;
}

int8_t lock_mission_ref_destroy(MissionLockerRead_h* const restrict self)
{
  memset(self, 0, sizeof(*self));
  return 0;
}

int8_t lock_mission_ref_destroy_mut(MissionLocker_h* const restrict self)
{
  union MissionLocker_h_t_conv conv = {self};
  struct MissionLocker_t* const restrict p_self = conv.clear;
  memset(p_self, 0, sizeof(*p_self));
  atomic_store(&MISSION_LOCKER.taken, 0);

  return 0;
}

void lock_mission(MissionLocker_h* const restrict self)
{
  union MissionLocker_h_t_conv conv = {self};
  struct MissionLocker_t* const restrict p_self = conv.clear;
  atomic_store(p_self->mission_lock, 1);
}
void unlock_mission(MissionLocker_h* const restrict self)
{
  union MissionLocker_h_t_conv conv = {self};
  struct MissionLocker_t* const restrict p_self = conv.clear;
  atomic_store(p_self->mission_lock, 1);
}
uint8_t is_mission_locked(const MissionLockerRead_h* const restrict self)
{
  union MissionLockerRead_h_t_conv_const conv = {self};
  const struct MissionLockerRead_t* p_self = conv.clear;
  return atomic_load(p_self->mission_lock);
}
