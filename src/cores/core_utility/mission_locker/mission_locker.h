#ifndef __MISSION_LOCKER__
#define __MISSION_LOCKER__

#include <stdint.h>

typedef struct MissionLocker_h{
  const uint8_t private_data[1];
}MissionLocker_h;

typedef struct MissionLockerRead_h{
  const uint8_t private_data[1];
}MissionLockerRead_h;

int8_t
lock_mission_ref_get(MissionLockerRead_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t
lock_mission_ref_get_mut(MissionLocker_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t
lock_mission_ref_destroy(MissionLockerRead_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t
lock_mission_ref_destroy_mut(MissionLocker_h* const restrict self)__attribute__((__nonnull__(1)));

void
lock_mission(MissionLocker_h* const restrict self)__attribute__((__nonnull__(1)));

void
unlock_mission(MissionLocker_h* const restrict self)__attribute__((__nonnull__(1)));

uint8_t
is_mission_locked(const MissionLockerRead_h* const restrict self)__attribute__((__nonnull__(1)));

#endif // !__MISSION_LOCKER__
