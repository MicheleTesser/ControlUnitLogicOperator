#ifndef __GIEI_ENGINES__
#define __GIEI_ENGINES__

#include "amk/amk.h"
#include "engine_common.h"

#if !defined(InverterType)
#define InverterType struct InvalidInverterType
#endif

#if !defined (inverter_module_init)
#define inverter_module_init(inverter,driver) engine_module_init_not_defined(inverter,driver)
#endif

#if !defined (engine_rtd_procedure)
#define engine_rtd_procedure(inverter) engine_rtd_procedure_not_defined(inverter)
#endif

#if !defined (engine_get_info)
#define engine_get_info(inverter, engine, info) engine_get_info_not_defined(inverter, engine,info)
#endif

#if !defined (engine_max_pos_torque)
#define engine_max_pos_torque(engine, limit_max_pos_torque) \
    engine_max_pos_torque_not_defined(engine, limit_max_pos_torque)
#endif

#if !defined (engine_max_neg_torque)
#define engine_max_neg_torque(engine, limit_max_neg_torque) \
    engine_max_neg_torque_not_defined(engine, limit_max_neg_torque)
#endif

#if !defined (engine_send_torque)
#define engine_send_torque(engine, pos_torque, neg_torque) \
    engine_send_torque_not_defined(engine, pos_torque, neg_torque)
#endif

#if !defined (engine_destroy)
#define engine_destroy(inverter) engine_destroy(inverter)
#endif



#endif // !__GIEI_ENGINES__
