#ifndef __CAR_DV__
#define __CAR_DV__

#include <stdint.h>
#include "../dv_driver_input/dv_driver_input.h"
#include "../../../core_utility/mission_reader/mission_reader.h"

typedef struct __attribute__((aligned(8))) Dv_h{
  const uint8_t private_data[392];
}Dv_h;

int8_t
dv_class_init(Dv_h* const restrict self ,
    CarMissionReader_h* const restrict p_mission_reader,
    DvDriverInput_h* const restrict driver)__attribute__((__nonnull__(1,2)));

int8_t dv_update(Dv_h* const restrict self)__attribute__((__nonnull__(1)));

#endif // !__CAR_DV__
