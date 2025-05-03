#ifndef __CAR_DV__
#define __CAR_DV__

#include <stdint.h>
#include "../../../core_utility/mission_reader/mission_reader.h"
#include "../../../core_utility/driver_input_reader/driver_input_reader.h"
#include "../../../../lib/raceup_board/raceup_board.h"

typedef struct __attribute__((aligned(4))){
  const uint8_t private_data[304];
}Dv_h;

int8_t
dv_class_init(Dv_h* const restrict self ,
    CarMissionReader_h* const restrict p_mission_reader)__attribute__((__nonnull__(1,2)));

int8_t
dv_update(Dv_h* const restrict self)__attribute__((__nonnull__(1)));

#endif // !__CAR_DV__
