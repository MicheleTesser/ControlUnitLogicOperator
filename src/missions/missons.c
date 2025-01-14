#include "./missons.h"
#include <stdint.h>

//private

static struct{
    enum MISSION_STATUS missions_status;
    enum MISSIONS current_mission;
    uint8_t lock_mission :1;
}CAR_MISSION;

//public

int8_t mission_class_init(void)
{
    CAR_MISSION.missions_status = MISSION_NOT_RUNNING;
    CAR_MISSION.lock_mission =0;
    CAR_MISSION.current_mission = NONE;
    return 0;
}

int8_t update_current_mission(const enum MISSIONS mission)
{
    if (!CAR_MISSION.lock_mission) {
        CAR_MISSION.current_mission = mission;
        return 0;
    }
    return -1;
}
enum MISSIONS get_current_mission(void)
{
    return CAR_MISSION.current_mission;
}

enum MISSION_STATUS mission_status(void)
{
    return CAR_MISSION.missions_status;
}

int8_t mission_lock_mission(void)
{
    CAR_MISSION.lock_mission =1;
    return 0;
}

int8_t mission_unlock_mission(void)
{
    CAR_MISSION.lock_mission =0;
    return 0;
}
