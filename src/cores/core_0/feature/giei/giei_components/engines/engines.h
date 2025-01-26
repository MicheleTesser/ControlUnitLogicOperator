#ifndef __GIEI_ENGINES__
#define __GIEI_ENGINES__

/*
 * This header defines the interface for the EngineType abstraction.
 * Users must define the following macros before including this file:
 * 
 * - EngineType: The structure representing the engine type.
 * - inverter_module_init: Function to initialize the engine module.
 * - inverter_update: Function to update the engine state.
 * - engine_rtd_procedure: Function to perform RTD procedures.
 * - engine_get_info: Function to retrieve engine information.
 * - engine_max_pos_torque: Function to retrieve maximum positive torque.
 * - engine_max_neg_torque: Function to retrieve maximum negative torque.
 * - engine_send_torque: Function to send torque values.
 * - engine_destroy: Function to clean up resources for the engine.
 * 
 * Failure to define these will result in a compile-time error.
 */


#include "amk/amk.h"
#include "engine_common.h"

#if !defined(EngineType)
#error "InverterType not found not found"
#endif

#if !defined (inverter_module_init)
#error "inveter_module_inif function not found"
#endif

#if !defined (inverter_update)
#error "inveter_update function not found"
#endif

#if !defined (engine_rtd_procedure)
#error "engine_rtd_procedure function not found"
#endif

#if !defined (engine_get_info)
#error "engine_get_info function not found"
#endif

#if !defined (engine_max_pos_torque)
#error "engine_mex_pos_torque function not found"
#endif

#if !defined (engine_max_neg_torque)
#error "engine_mex_neg_torque function not found"
#endif

#if !defined (engine_send_torque)
#error "engine_send_torque function not found"
#endif

#if !defined (engine_destroy)
#error "engine_destroy function not found"
#endif



#endif // !__GIEI_ENGINES__
