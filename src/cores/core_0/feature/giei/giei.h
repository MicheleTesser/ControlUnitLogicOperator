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
#include "../driver_input/driver_input.h"
#include "../maps/maps.h"
#include "../../../core_utility/imu/imu.h"
#include "../../../../lib/raceup_board/raceup_board.h"
#include "../engines/engines.h"

#if ARCH == 64
typedef struct __attribute__((aligned(8))){
  const uint8_t private_data[128];
}Giei_h;
#elif ARCH == 32
typedef struct __attribute__((aligned(4))){
  const uint8_t private_data[92];
}Giei_h;
#endif // ARCH == 64


int8_t 
giei_init(Giei_h* const restrict self,
    EngineType* const engine,
    const DriverInput_h* const p_driver,
    const DrivingMaps_h* const p_maps,
    const Imu_h* const p_imu)__attribute__((__nonnull__(1,2,3,4,5)));


int8_t
giei_update(Giei_h* const restrict self )__attribute__((__nonnull__(1)));

enum RUNNING_STATUS 
GIEI_check_running_condition(Giei_h* const restrict self )__attribute__((__nonnull__(1)));

int8_t
GIEI_compute_power(Giei_h* const restrict self )__attribute__((__nonnull__(1)));

#endif // !__CAR_GIEI__
