#ifndef __CAR_LOG__
#define __CAR_LOG__

#include <stdint.h>
// #include "entry/entry.h"

typedef struct LogEntry_h{
    const uint8_t data_size;
    const uint8_t log_mode:2;
    const uint8_t data_signed:1;
    const uint8_t data_float:1;
    const int8_t* const name;
    const void* const data_ptr;
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
        const LogEntry_h entry);

int8_t
log_update_and_send(Log_h* const restrict self __attribute__((__nonnull__)));

#endif // !__CAR_LOG__
