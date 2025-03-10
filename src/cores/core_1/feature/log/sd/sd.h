#ifndef __LOG_SD__
#define __LOG_SD__

#include <stdint.h>
typedef struct {
    const uint8_t private_data[1];
}LogSd_h;

int8_t
log_sd_init(LogSd_h* const restrict self )__attribute__((__nonnull__(1)));

int8_t
log_sd_lock(LogSd_h* const restrict self )__attribute__((__nonnull__(1)));

int8_t
log_sd_write(LogSd_h* const restrict self ,
        const void* const restrict buffer , 
        const uint8_t buffer_size)__attribute__((__nonnull__(1,2)));

#endif // !__LOG_SD__
