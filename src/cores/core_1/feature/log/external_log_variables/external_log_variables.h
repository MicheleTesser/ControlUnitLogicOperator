#ifndef __EXTERNAL_LOG_VARIABLES__
#define __EXTERNAL_LOG_VARIABLES__

#include <stdint.h>
#include "../log.h"

#define AMK_SHARED_VARS(engine, init_extra_op)\
  AMK_STATUS_##engine init_extra_op,\
  AMK_ACTUAL_VELOCITY_##engine,\
  AMK_TORQUE_CURRENT_##engine,\
  AMK_VOLTAGE_##engine,\
  AMK_CURRENT_##engine, /*INFO: to remove. Exist only for legacy compatibility*/ \
  AMK_TEMP_MOTOR_##engine,\
  AMK_TEMP_INVERTER_##engine,\
  AMK_TEMP_IGBT_##engine,\
  AMK_ERROR_INFO_##engine,\
  AMK_LIMIT_POSITIVE_##engine,\
  AMK_LIMIT_NEGATIVE_##engine,\

typedef enum{
  AMK_SHARED_VARS(FL,=0)
  AMK_SHARED_VARS(FR, )
  AMK_SHARED_VARS(RL, )
  AMK_SHARED_VARS(RR, )

  __NUM_OF_SHARED_ID__
}SharedDataId;


int8_t
external_log_variables_store_pointer(const void* p_data, const SharedDataId data_id)
  __attribute__((__nonnull__(1)));

int8_t
external_log_variables_add_to_log(Log_h* const restrict p_log)__attribute__((__nonnull__(1)));

#endif // !__EXTERNAL_LOG_VARIABLES__
