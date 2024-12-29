#ifndef __CAR_MISSIONS_DV__
#define __CAR_MISSIONS_DV__

#include <stdint.h>

//INFO: maintain the order of the enums or at least leave NONE and MANUALLY in that order and on 
//top of the other enums
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
    MISSION_RUNNING,
    MISSION_FINISHED,
};

int8_t mission_class_init(void);
uint8_t update_current_mission(const enum MISSIONS mission);
enum MISSIONS get_current_mission(void);
int8_t mission_status(void);
int8_t mission_lock_mission(void);
int8_t mission_unlock_mission(void);

#endif // !__CAR_MISSIONS_DV__
