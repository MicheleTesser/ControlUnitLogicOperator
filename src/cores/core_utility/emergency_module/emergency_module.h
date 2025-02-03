#ifndef __EMERGENCY_MODULE__
#define __EMERGENCY_MODULE__

#include "../../../lib/raceup_board/components/trap.h"
#include <stdint.h>
#include <assert.h>


typedef struct EmergencyNode_h
{
  const uint8_t private_data[24];
}EmergencyNode_h;

int8_t EmergencyNode_class_init(void);

int8_t
EmergencyNode_init(EmergencyNode_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t
EmergencyNode_raise(
        struct EmergencyNode_h* const restrict self , 
        const uint8_t exeception)__attribute__((__nonnull__(1)));

int8_t
EmergencyNode_solve(
        struct EmergencyNode_h* const restrict self,
        const uint8_t exeception)__attribute__((__nonnull__(1)));

int8_t
EmergencyNode_is_emergency_state(const struct EmergencyNode_h* const restrict self)
    __attribute__((__nonnull__(1)));

#endif // !__EMERGENCY_MODULE__
