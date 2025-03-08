#include "core_x_log_variable.h"
#include "../feature/log/log.h"
#include "../../core_utility/core_status/core_status.h"

//private



//public

int8_t init_log_var_core_x(const enum CORES core, Log_h* const restrict log __attribute__((__unused__)))
{
  switch (core)
  {
    case CORE_0:
      break;
    case CORE_1:
      break;
    case CORE_2:
      break;
    default:
      return -1;
  }
  return 0;
}
