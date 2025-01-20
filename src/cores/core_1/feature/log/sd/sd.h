#ifndef __LOG_SD__
#define __LOG_SD__

#include <stdint.h>
typedef struct LogSd_h{
    const uint8_t private_data[1];
}LogSd_h;

int8_t
log_sd_init(LogSd_h* const restrict self __attribute__((__nonnull__)));

int8_t
log_sd_write(LogSd_h* const restrict self __attribute__((__nonnull__)),
        const void* const restrict buffer __attribute__((__nonnull__)), 
        const uint8_t buffer_size);

#endif // !__LOG_SD__
