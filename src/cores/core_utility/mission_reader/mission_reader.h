#ifndef __CAR_MISSION_READER__
#define __CAR_MISSION_READER__

#include <stdint.h>
#include "mission_locker/mission_locker.h"

typedef struct __attribute__((aligned(4))) CarMissionReader_h{
  const uint8_t private_data[20];
  MissionLockerRead_h o_mission_locker_read;
}CarMissionReader_h;

/*
FROM can2.dbc

0 "none";
1 "manualy" 
2 "dv_acceleration" 
3 "dv_skidpad" 
4 "dv_autocross" 
5 "dv_trackdrive" 
6 "dv_ebs_test" 
7 "dv_inspection" 
*/

enum CAR_MISSIONS
{
  CAR_MISSIONS_NONE=0,
  CAR_MISSIONS_HUMAN,
  CAR_MISSIONS_DV_SKIDPAD,
  CAR_MISSIONS_DV_AUTOCROSS,
  CAR_MISSIONS_DV_TRACKDRIVE,
  CAR_MISSIONS_DV_EBS_TEST,
  CAR_MISSIONS_DV_INSPECTION,

  __NUM_OF_CAR_MISSIONS__
};

int8_t
car_mission_reader_init(CarMissionReader_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t
car_mission_reader_update(CarMissionReader_h* const restrict self)__attribute__((__nonnull__(1)));

enum CAR_MISSIONS
car_mission_reader_get_current_mission(CarMissionReader_h* const restrict self)__attribute__((__nonnull__(1)));

void
car_mission_reader_destroy(CarMissionReader_h* const restrict self)__attribute__((__nonnull__(1)));

#endif // !__CAR_MISSION_READER__
