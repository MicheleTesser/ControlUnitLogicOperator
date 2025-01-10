#ifndef __EMERGENCY_FAUL__
#define __EMERGENCY_FAUL__

#include <stdint.h>

enum EMERGENCY_FAULT{
    FAILED_RTD_SEQ = (1 << 0),
    ENGINE_FAULT = (1 << 1),
    DV_EMERGENCY_STATE = (1 << 2),
    RTD_IN_NONE_MISSION = (1 << 3),
    BMS_LV_FAULT = (1 << 4),
    BMS_HV_FAULT = (1 << 5),
    IMU_FAULT = (1 << 6),
    ATC_FRONT_FAULT = (1 << 7),
    SMU_FAULT = (1 << 7),
    LEM_FAULT = (1 << 9),
};

int8_t one_emergency_raised(const enum EMERGENCY_FAULT id);
int8_t one_emergency_solved(const enum EMERGENCY_FAULT id);
int8_t is_emergency_state(void);

#endif // !__EMERGENCY_FAUL__
