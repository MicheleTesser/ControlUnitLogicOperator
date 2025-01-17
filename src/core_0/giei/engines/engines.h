#ifndef __GIEI_ENGINES__
#define __GIEI_ENGINES__

#include "amk/amk.h"
#include "engine_common.h"

#if !defined(InverterType)
#define InverterType struct InvalidInverterType
#endif

#if !defined (inverter_module_init)
#define inverter_module_init(inverter) engine_module_init_not_defined(inverter)
#endif


#endif // !__GIEI_ENGINES__
