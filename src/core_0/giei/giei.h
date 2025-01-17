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

#include <stdint.h>
#include <string.h>

#include "./engines/engines.h"

struct Giei_h
{
    uint8_t private_data[136];
};

int8_t giei_init(struct Giei_h* const restrict self);
enum RUNNING_STATUS GIEI_check_running_condition(struct Giei_h* const restrict self);



#endif // !__CAR_GIEI__
