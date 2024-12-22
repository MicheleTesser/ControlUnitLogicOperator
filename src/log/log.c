#include "log.h"

#include "./sd/sd.h"
#include "./telemetry/telemetry.h"
#include "./log_pool/log_pool.h"
#include <stdint.h>

//private
static uint8_t init_done=0;

static inline void wait_init(void){
    while (!init_done) {}
}

//public
int8_t log_system_init(void)
{
    return log_pool_init() | sd_log_init() | telemetry_init();
}

int8_t log_add_var_to_log(
        const uint8_t log_method,
        void* const restrict var, 
        const uint8_t var_size,
        const uint16_t position,
        const char* const restrict name)
{
    int8_t err=0;
    wait_init();

    if (log_pool_push(var, var_size, name) < 0){
        goto pool_push_fail;
    }

    if ((log_method & _SD_) && sd_log_add_entry(position) <0){
            goto sd_add_entry_fail;
    }
    if ((log_method & _TELEMETRY_) && telemetry_add_entry(position) <0){
            goto telemetry_add_entry_fail;
    }

    return 0;

pool_push_fail:
    err--;
sd_add_entry_fail:
    err--;
telemetry_add_entry_fail:
    err--;

    return err;
}

int8_t log_update(const uint8_t log_type)
{
    int8_t err=0;
    wait_init();
    if ((log_type & _SD_)  && sd_update_and_write() < 0) {
        goto sd_update_fail;
    }

    if ((log_type & _TELEMETRY_)  && telemetry_update_and_send() < 0) {
        goto telemetry_update_fail;
    }

sd_update_fail:
    err--;
telemetry_update_fail:
    err--;

    return err;
}
