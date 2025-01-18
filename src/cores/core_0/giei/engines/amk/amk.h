#ifndef __AMK_POWER_SYSTEM__
#define __AMK_POWER_SYSTEM__

#include <stdint.h>
#include "../../../../../driver_input/driver_input.h"
#include "../engine_common.h"

//INFO: doc/amk_datasheet.pdf page 61

struct AmkInverter_h{
    uint8_t private_data[112];
};

int8_t amk_module_init(struct AmkInverter_h* const restrict self,
        const struct DriverInput_h* const p_driver_input);
int8_t amk_update_status(struct AmkInverter_h* const restrict self);
enum RUNNING_STATUS amk_rtd_procedure(struct AmkInverter_h* const restrict self);

#define EngineType struct AmkInverter_h

#define inverter_module_init(inverter,driver) amk_module_init(inverter,driver)
#define engine_rtd_procedure(inverter) amk_rtd_procedure(inverter);


#endif // !__AMK_POWER_SYSTEM__

