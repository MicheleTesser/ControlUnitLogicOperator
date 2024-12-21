#ifndef __CAR_LOG_SYTEM__
#define __CAR_LOG_SYTEM__

#include <stdint.h>
enum LOG_LOCATION{
    _SD_        = (1<<0),
    _TELEMETRY_ = (1<<1),
};

#define LOG_MAX_SIZE 10

int8_t log_system_init(void);
int8_t log_add_var_to_log(
        const uint8_t log_method,
        const void* const restrict var, 
        const uint16_t position,
        const char* const restrict name);
int8_t log_update(const enum LOG_LOCATION log_type);

#endif // !__CAR_LOG_SYTEM__
