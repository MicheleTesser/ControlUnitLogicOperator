#ifndef __AMK_POWER_SYSTEM__
#define __AMK_POWER_SYSTEM__

#include <stdint.h>
#include "../../lib/raceup_board/raceup_board.h"
#include "../engine_common.h"

int8_t amk_module_init(void);
int8_t amk_set_max_speed(const uint32_t speed);
int8_t amk_send_torque(const enum ENGINES engine, const float pos_torque, const float neg_torque);
void amk_update_status(const CanMessage* const restrict mex);
enum RUNNING_STATUS amk_rtd_procedure(void);
void amk_shut_down_power(void);
float amk_get_info(const enum ENGINES engine, const enum ENGINE_INFO info);
float amk_max_pos_torque(void);
float amk_max_neg_torque(void);

#endif // !__AMK_POWER_SYSTEM__
