#ifndef __DV_MISSIONS__
#define __DV_MISSIONS__
#include <stdint.h>

typedef struct DvMission_h{
    const uint8_t private_data[32];
}DvMission_h;

enum MISSIONS{
    NONE=0,
    MANUALY,

    DV_ACCELERATION,
    DV_SKIDPAD,
    DV_AUTOCROSS,
    DV_TRACKDRIVE,
    DV_EBS_TEST,
    DV_INSPECTION,
};

enum MISSION_STATUS{
    MISSION_NOT_RUNNING=0,
    MISSION_RUNNING,
    MISSION_FINISHED,
};

int8_t
dv_mission_init(DvMission_h* const restrict self )__attribute__((__nonnull__(1)));

int8_t
dv_mission_update(DvMission_h* const restrict self )__attribute__((__nonnull__(1)));

enum MISSIONS
dv_mission_get_current(const DvMission_h* const restrict self )__attribute__((__nonnull__(1)));

enum MISSION_STATUS
dv_mission_get_status(const DvMission_h* const restrict self )__attribute__((__nonnull__(1)));

void dv_mission_lock(DvMission_h* const restrict self )__attribute__((__nonnull__(1)));

void dv_mission_unlock(DvMission_h* const restrict self )__attribute__((__nonnull__(1)));

#endif // !__DV_MISSIONS__
