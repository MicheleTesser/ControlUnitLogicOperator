#ifndef __AMK_POWER_SYSTEM__
#define __AMK_POWER_SYSTEM__

#include <stdint.h>
#include "../../../../driver_input/driver_input.h"
#include "../engine_common.h"

//INFO: doc/amk_datasheet.pdf page 61

typedef struct AmkInverter_h{
    const uint8_t private_data[112];
}AmkInverter_h;

int8_t 
amk_module_init(AmkInverter_h* const restrict self  ,
        const struct DriverInput_h* const p_driver_input)__attribute__((__nonnull__(1,2)));

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


#define EngineType AmkInverter_h
#define inverter_module_init(inverter,driver) amk_module_init(inverter,driver)
#define inverter_update(inverter) amk_update(inverter)
#define engine_rtd_procedure(inverter) amk_rtd_procedure(inverter);
#define engine_get_info(inverter, engine, info) amk_get_info(inverter, engine, info)
#define engine_max_pos_torque(engine, limit_max_pos_torque) \
    amk_max_pos_torque(engine, limit_max_pos_torque)
#define engine_max_neg_torque(engine, limit_max_neg_torque) \
    amk_max_neg_torque(engine, limit_max_neg_torque)
#define engine_send_torque(self, engine, pos_torque, neg_torque) \
    amk_send_torque(self, engine, pos_torque, neg_torque)

#define engine_destroy(inverter) amk_destroy(inverter);


#endif // !__AMK_POWER_SYSTEM__
