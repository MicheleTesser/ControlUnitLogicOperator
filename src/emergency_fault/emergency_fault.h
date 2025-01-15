#ifndef __EMERGENCY_FAUL__
#define __EMERGENCY_FAUL__

#include <stdint.h>


enum EMERGENCY_FAULT{
    FAILED_RTD_SEQ = 0,
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

struct EmergencyController;

const struct EmergencyController* emergency_get(void);
struct EmergencyController* emergency_get_mut(void);

int8_t one_emergency_raised(struct EmergencyController* const restrict self,
        const enum EMERGENCY_FAULT id);
int8_t one_emergency_solved(struct EmergencyController* const restrict self,
        const enum EMERGENCY_FAULT id);
int8_t is_emergency_state(const struct EmergencyController* const restrict self);

void emergency_free_ptr_read(void);
void emergency_free_ptr_mut(void);

#define EMERGENCY_FAULT_READ_ONLY_ACTION(exp)\
{\
    const struct EmergencyController* const emergency_read_ptr = emergency_get();\
    exp;\
    emergency_free_ptr_read();\
}

#define EMERGENCY_FAULT_MUT_ACTION(exp)\
{\
    struct EmergencyController* const emergency_mut_ptr = emergency_get_mut();\
    exp;\
    emergency_free_ptr_mut();\
}

#endif // !__EMERGENCY_FAUL__
