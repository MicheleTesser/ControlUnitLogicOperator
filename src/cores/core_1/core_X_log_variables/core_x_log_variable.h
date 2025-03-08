#ifndef __CORE_0_LOG_VARIABLES__
#define __CORE_0_LOG_VARIABLES__

#include <stdint.h>
#include "../feature/log/log.h"
#include "../../core_utility/core_status/core_status.h"

int8_t
init_log_var_core_x(const enum CORES core, Log_h* const restrict log)__attribute__((__nonnull__(2)));

#endif // !__CORE_0_LOG_VARIABLES__
