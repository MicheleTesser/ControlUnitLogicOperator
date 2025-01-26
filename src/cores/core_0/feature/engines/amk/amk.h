#ifndef __AMK_POWER_SYSTEM__
#define __AMK_POWER_SYSTEM__

#include <stdint.h>
#include "../../../feature/driver_input/driver_input.h"
#include "../engines.h"

//INFO: doc/amk_datasheet.pdf page 61

typedef struct Inverter{
    const uint8_t private_data[112];
}AmkInverter_h;


int8_t 
amk_module_init(AmkInverter_h* const restrict self,
        const struct DriverInput_h* const p_driver_input,
        struct EngineType* const restrict general_inverter)__attribute__((__nonnull__(1,2,3)));

int8_t
amk_update(AmkInverter_h* const restrict self )__attribute__((__nonnull__(1)));

enum RUNNING_STATUS
amk_rtd_procedure(AmkInverter_h* const restrict self )__attribute__((__nonnull__(1)));

float
amk_get_info(const AmkInverter_h* const restrict self ,
        const enum ENGINES engine, const enum ENGINE_INFO info)__attribute__((__nonnull__(1)));

float
amk_max_pos_torque(const AmkInverter_h* const restrict self ,
        const float limit_max_pos_torque)__attribute__((__nonnull__(1)));

float
amk_max_neg_torque(const AmkInverter_h* const restrict self ,
        const float limit_max_neg_torque)__attribute__((__nonnull__(1)));

int8_t
amk_send_torque(const AmkInverter_h* const restrict self ,
        const enum ENGINES engine, const float pos_torque, const float neg_torque)
    __attribute__((__nonnull__(1)));


void
amk_destroy(AmkInverter_h* const restrict self )__attribute__((__nonnull__(1)));

#endif // !__AMK_POWER_SYSTEM__
