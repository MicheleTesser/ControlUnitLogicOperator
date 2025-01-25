#ifndef __EMERGENCY_MODULE__
#define __EMERGENCY_MODULE__

#include "../../../lib/raceup_board/components/trap.h"
#include <stdint.h>
#include <assert.h>


typedef struct EmergencyNode EmergencyNode;

struct EmergencyNode*
EmergencyNode_new(const uint8_t num_exception);

void
EmergencyNode_raise(
        struct EmergencyNode* const restrict self , 
        const uint8_t exeception)__attribute__((__nonnull__(1)));

void
EmergencyNode_solve(
        struct EmergencyNode* const restrict self,
        const uint8_t exeception)__attribute__((__nonnull__(1)));

uint8_t
EmergencyNode_is_emergency_state(struct EmergencyNode* const restrict self)
    __attribute__((__nonnull__(1)));

void
EmergencyNode_free(struct EmergencyNode* const restrict self)__attribute__((__nonnull__(1)));

#endif // !__EMERGENCY_MODULE__
