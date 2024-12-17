#ifndef __AMK_POWER_SYSTEM__
#define __AMK_POWER_SYSTEM__

#include <stdint.h>
#include "../../lib/raceup_board/raceup_board.h"

enum ENGINES {
    FRONT_LEFT = 0, //Front Left: Status Values: [0x283,0x285] SetPoint: 0x184
    FRONT_RIGHT = 1, //Front Right: Status Values: [0x284,0x286] SetPoint: 0x185
    REAR_LEFT = 2, //Rear Left: Status Values: [0x287,0x289] SetPoint: 0x188
    REAR_RIGHT = 3, //Rear Right: Status Values: [0x288,0x28A] SetPoint: 0x189
};

int8_t stop_engine(const enum ENGINES engine);
int8_t set_regen_brake_engine(const enum ENGINES engine, int16_t brake);
int8_t set_throttle_engine(const enum ENGINES engine, int16_t throttle);
uint8_t inverter_hv_status(void);
void update_status(const CanMessage* const restrict mex);

#endif // !__AMK_POWER_SYSTEM__
