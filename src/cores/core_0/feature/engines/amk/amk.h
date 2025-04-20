#ifndef __AMK_POWER_SYSTEM__
#define __AMK_POWER_SYSTEM__

#include <stdint.h>
#include "../../../feature/driver_input/driver_input.h"
#include "../../../../../lib/raceup_board/raceup_board.h"
#include "../engines.h"

typedef struct __attribute__((aligned(4))){
  const uint8_t private_data[156];
}AmkInverter_h;

int8_t 
amk_module_init(AmkInverter_h* const restrict self,
        const DriverInput_h* const p_driver_input,
        struct EngineType* const restrict general_inverter)__attribute__((__nonnull__(1,2,3)));

#endif // !__AMK_POWER_SYSTEM__
