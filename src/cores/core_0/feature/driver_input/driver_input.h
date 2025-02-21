#ifndef __CORE_0_DRIVER_INPUT__
#define __CORE_0_DRIVER_INPUT__

#include <stdint.h>
#include "../../../core_utility/mission_reader/mission_reader.h"
#include "../../../core_utility/driver_input_reader/driver_input_reader.h"

typedef struct __attribute__((aligned(8))) DriverInput_h{
    uint8_t private_data[64];
}DriverInput_h;

int8_t 
driver_input_init(DriverInput_h* const restrict self,
    CarMissionReader_h* const restrict p_car_mission)__attribute__((__nonnull__(1)));

int8_t
giei_driver_input_get(const DriverInput_h* const restrict self,
    const enum INPUT_TYPES input_type)__attribute__((__nonnull__(1)));

int8_t
giei_driver_input_rtd_request(const DriverInput_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t
giei_driver_input_update(DriverInput_h* const restrict self )__attribute__((__nonnull__(1)));

void 
giei_driver_input_destroy(DriverInput_h* const restrict self )__attribute__((__nonnull__(1)));


#endif // !__CORE_0_DRIVER_INPUT__
