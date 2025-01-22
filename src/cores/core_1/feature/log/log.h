#ifndef __CAR_LOG__
#define __CAR_LOG__

#include <stdint.h>
#include "log_obj_types.h"

#define LOG_ENTRY_NAME_MAX_SIZE 32

typedef struct LogEntry_h{
    const uint8_t log_mode:2;
    const uint8_t data_mode: 2;
    const uint32_t data_min;
    const uint32_t data_max;
    const void* const data_ptr;
    const uint8_t name[LOG_ENTRY_NAME_MAX_SIZE];
}LogEntry_h;

enum LOG_MODE{
    LOG_SD = (1<<0),
    LOG_TELEMETRY = (1<<1),
};

typedef struct Log_h{
    const uint8_t private_data[1];
}Log_h;

int8_t
log_init(Log_h* const restrict self __attribute__((__nonnull__)));

int8_t
log_add_entry(Log_h* const restrict self __attribute__((__nonnull__)),
        const LogEntry_h* entry  __attribute__((__nonnull__)));

int8_t
log_update_and_send(Log_h* const restrict self __attribute__((__nonnull__)));

#endif // !__CAR_LOG__
