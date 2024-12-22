#ifndef __CAR_LOG_SYTEM__
#define __CAR_LOG_SYTEM__

#include <stdint.h>
enum LOG_LOCATION{
    _SD_        = (1<<0),
    _TELEMETRY_ = (1<<1),
};

int8_t log_system_init(void);
int8_t log_add_var_to_log(
        const uint8_t log_method,
        void* const restrict var, 
        const uint8_t var_size,
        const uint16_t position,
        const char* const restrict name);
int8_t log_update(const uint8_t log_type);

#endif // !__CAR_LOG_SYTEM__
