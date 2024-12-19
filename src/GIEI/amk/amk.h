#ifndef __AMK_POWER_SYSTEM__
#define __AMK_POWER_SYSTEM__

#include <stdint.h>
#include "../../lib/raceup_board/raceup_board.h"
#include "../engine_common.h"

int8_t amk_module_init(void);
int8_t amk_set_regen_brake_engine(const enum ENGINES engine, const int16_t brake);
int8_t amk_set_throttle_engine(const enum ENGINES engine, const int16_t throttle);
void amk_update_status(const CanMessage* const restrict mex);
enum RUNNING_STATUS amk_rtd_procedure(void);
void amk_shut_down_power(void);

#endif // !__AMK_POWER_SYSTEM__
