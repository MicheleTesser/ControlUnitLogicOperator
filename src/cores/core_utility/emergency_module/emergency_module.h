#ifndef __EMERGENCY_MODULE__
#define __EMERGENCY_MODULE__

#include "../../../lib/raceup_board/components/trap.h"
#include <stdint.h>

typedef struct{
  const uint8_t private_data[9];
}EmergencyNode_h;

int8_t EmergencyNode_class_init(void);

int8_t
EmergencyNode_init(EmergencyNode_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t
EmergencyNode_raise(EmergencyNode_h* const restrict self , 
        const uint8_t exeception)__attribute__((__nonnull__(1)));

int8_t
EmergencyNode_solve(EmergencyNode_h* const restrict self,
        const uint8_t exeception)__attribute__((__nonnull__(1)));

int8_t
EmergencyNode_is_emergency_state(const EmergencyNode_h* const restrict self)
    __attribute__((__nonnull__(1)));

#endif // !__EMERGENCY_MODULE__
