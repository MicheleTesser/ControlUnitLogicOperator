#ifndef __EMERGENCY_FAUL__
#define __EMERGENCY_FAUL__

#include <stdint.h>

enum EMERGENCY_FAULT{
    FAILED_RTD_SEQ = 0,
    ENGINE_FAULT = (1 << 0),
};

int8_t one_emergency_raised(const enum EMERGENCY_FAULT id);
int8_t one_emergency_solved(const enum EMERGENCY_FAULT id);
int8_t is_emergency_state(void);

#endif // !__EMERGENCY_FAUL__
