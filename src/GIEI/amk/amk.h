#ifndef __AMK_POWER_SYSTEM__
#define __AMK_POWER_SYSTEM__

#include <stdint.h>
#include "../../lib/raceup_board/raceup_board.h"
#include "../engine_common.h"

int8_t amk_module_init(void);
int8_t amk_set_max_speed(const uint32_t speed);
int8_t amk_send_torque(const enum ENGINES engine, const float pos_torque, const float neg_torque);
void amk_update_status(const CanMessage* const restrict mex);
uint8_t amk_inverter_hv_status(void);
enum RUNNING_STATUS amk_rtd_procedure(void);
void amk_shut_down_power(void);
float amk_get_info(const enum ENGINES engine, const enum ENGINE_INFO info);
float amk_max_pos_torque(const float limit_max_pos_torque);
float amk_max_neg_torque(const float limit_max_neg_torque);

#define engine_module_init() amk_module_init()

#define engine_set_max_speed(speed) amk_set_max_speed(speed)

#define engine_send_torque(engine, pos_torque, neg_torque) \
    amk_send_torque(engine, pos_torque, neg_torque)

#define engine_update_status(mex) amk_update_status(mex)

#define engine_inverter_hv_status() amk_inverter_hv_status()

#define engine_rtd_procedure() amk_rtd_procedure()

#define engine_shut_down_power() amk_shut_down_power()

#define engine_get_info(engine, info) amk_get_info(engine, info)

#define engine_max_pos_torque(limit_max_pos_torque) amk_max_pos_torque(limit_max_pos_torque)

#define engine_max_neg_torque(limit_max_neg_torque) amk_max_neg_torque(limit_max_neg_torque)


#endif // !__AMK_POWER_SYSTEM__
