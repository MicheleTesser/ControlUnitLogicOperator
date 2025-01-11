#ifndef __EMERGENCY_FAUL__
#define __EMERGENCY_FAUL__

#include <stdint.h>

enum EMERGENCY_FAULT{
    FAILED_RTD_SEQ = 1,
    ENGINE_FAULT,
    DV_EMERGENCY_STATE,
    RTD_IN_NONE_MISSION,
    BMS_LV_FAULT,
    BMS_HV_FAULT,
    IMU_FAULT,
    ATC_FRONT_FAULT,
    SMU_FAULT,
    LEM_FAULT,
    


    __NUM_OF_EMERGENCY_FAULTS,
};

int8_t one_emergency_raised(const enum EMERGENCY_FAULT id);
int8_t one_emergency_solved(const enum EMERGENCY_FAULT id);
int8_t is_emergency_state(void);

#endif // !__EMERGENCY_FAUL__
