#ifndef __AMK_POWER_SYSTEM__
#define __AMK_POWER_SYSTEM__

#include <stdint.h>
#include "../engine_common.h"

//INFO: doc/amk_datasheet.pdf page 61

struct AmkInverter_h{
    uint8_t private_data[14];
};

int8_t amk_module_init(struct AmkInverter_h* const restrict self);
int8_t amk_update_status(struct AmkInverter_h* const restrict self);

#define EngineType struct AmkInverter_h

#define inverter_module_init(inverter) amk_module_init(inverter)


#endif // !__AMK_POWER_SYSTEM__

