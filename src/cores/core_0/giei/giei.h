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
#include "../../../driver_input/driver_input.h"
#include "./giei_components/engines/engine_common.h"

typedef struct Giei_h{
    uint8_t private_data[136];
}Giei_h;

int8_t 
giei_init(Giei_h* const restrict self __attribute__((__nonnull__)),
        const DriverInput_h* const driver);

enum RUNNING_STATUS 
GIEI_check_running_condition(Giei_h* const restrict self __attribute__((__nonnull__)));



#endif // !__CAR_GIEI__
