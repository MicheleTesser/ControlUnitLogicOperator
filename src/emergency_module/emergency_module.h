#ifndef __EMERGENCY_MODULE__
#define __EMERGENCY_MODULE__

#include "../lib/raceup_board/components/trap.h"
#include <stdint.h>
#include <assert.h>


struct EmergencyNode;

struct EmergencyNode* EmergencyNode_init(const uint8_t num_exception);
void EmergencyNode_raise(struct EmergencyNode* const restrict self __attribute__((__nonnull__)),
        const uint8_t exeception);
void EmergencyNode_solve(struct EmergencyNode* const restrict self __attribute__((__nonnull__)),
        const uint8_t exeception);
uint8_t EmergencyNode_is_emergency_state(struct EmergencyNode* const restrict self __attribute__((__nonnull__)));
void EmergencyNode_free(struct EmergencyNode* const restrict self __attribute__((__nonnull__)));

#endif // !__EMERGENCY_MODULE__
