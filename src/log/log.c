#include "log.h"

#include "./sd/sd.h"
#include "./telemetry/telemetry.h"
#include <stdint.h>

static struct{
    char* name;
    void* value;
    uint8_t log_method;
}LOG[LOG_MAX_SIZE];

int8_t log_system_init(void)
{
    return 0;
}

int8_t log_add_var_to_log(
        const uint8_t log_method,
        const void* const restrict var, 
        const uint16_t position,
        const char* const restrict name)
{
    return 0;
}

int8_t log_update(const enum LOG_LOCATION log_type)
{
    return 0;
}
