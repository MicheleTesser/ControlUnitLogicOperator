#ifndef __CAR_GIEI__
#define __CAR_GIEI__
/*
 * Generic Inverter Engine Interface (GIEI) is a generic interface for the engines used in the car
 * it contains all the function needed to:
 *  - initialize
 *  - control
 *  the engine used. 
 *
 *  It does not depend on the type of engine used and has to stay in this way.
 */

#include "../lib/raceup_board/raceup_board.h"
#include "./power_maps/power_maps.h"
#include "./engine_common.h"
#include <stdint.h>

enum GIEI_LIMITS{
    POWER_LIMIT,
    MAX_POS_TORQUE_LIMIT,
    MAX_NEG_TORQUE_LIMIT,
    MOTOR_REPARTIION, // [0,1]: 0 rear, 1 front
    TORQUE_VECTORING_ACTIVATION,
};

enum GIEI_INFO{
    GIEI_INFO_LIMIT_POWER,
    GIEI_INFO_MAX_SPEED,
    GIEI_INFO_MAX_POS_TORQUE,
    GIEI_INFO_MAX_NEG_TORQUE,
    GIEI_INFO_LIMIT_REGEN,
    GIEI_INFO_TV,
    GIEI_INFO_REAR_REPARTITION,
    GIEI_INFO_FRONT_REPARTITION,
    GIEI_INFO_STATUS_HV,
    GIEI_INFO_STATUS_RF,
    GIEI_INFO_CURRENT_SPEED,
};

struct GIEI;

int8_t GIEI_init(void);
const struct GIEI* GIEI_get(void);
struct GIEI* GIEI_get_mut(void);
int8_t GIEI_recv_data(const CanMessage* const restrict mex);
enum RUNNING_STATUS GIEI_check_running_condition(void);
int8_t GIEI_set_limits(const enum GIEI_LIMITS category, const float value);
int8_t GIEI_input(const float throttle, const float regen);
float GIEI_get_info(const struct GIEI* const restrict self, const enum GIEI_INFO info);

void GIEI_free_read_ptr(void);
void GIEI_free_write_ptr(void);

#define GIEI_READ_ONLY_ACTION(exp) \
{\
    const struct GIEI* giei_read_ptr = GIEI_get();\
    exp;\
    GIEI_free_read_ptr();\
}

#define GIEI_MUT_ACTION(exp) \
{\
    struct GIEI* giei_read_ptr = GIEI_get_mut();\
    exp;\
    GIEI_free_write_ptr();\
}

//debug
uint8_t DEBUG_GIEI_check_limits(float power_limit, float pos_torque, 
        float neg_torque, float repartition, float tv_on);

#endif // !__CAR_GIEI__
