#ifndef __CAR_MISSIONS_DV__
#define __CAR_MISSIONS_DV__

#include <stdint.h>
enum MISSIONS{
    MANUALY,
    DV_ACCELERATION,
    DV_SKIDPAD,
    DV_AUTOCROSS,
    DV_TRACKDRIVE,
    DV_EBS_TEST,
    DV_INSPECTION,
};

uint8_t update_current_mission(const enum MISSIONS mission);
enum MISSIONS get_current_mission(void);

#endif // !__CAR_MISSIONS_DV__
