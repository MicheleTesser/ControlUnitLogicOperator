#ifndef __GIEI_ENGINES__
#define __GIEI_ENGINES__

#include "engine_common.h"
#include "amk/amk.h"

#if !defined(EngineType)
#define EngineType struct InvalidEngineType
#endif

#if !defined (engine_module_init)
#define engine_module_init() engine_module_init_not_defined()
#endif


#endif // !__GIEI_ENGINES__
