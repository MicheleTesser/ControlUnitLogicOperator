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


int8_t GIEI_initialize(void);
int8_t GIEI_recv_data(const CanMessage* const restrict mex);
enum RUNNING_STATUS GIEI_check_running_condition(void);
int8_t GIEI_set_limits(const enum GIEI_LIMITS category, const float value);
int8_t GIEI_input(const float throttle, const float brake, const float regen);


#endif // !__CAR_GIEI__
