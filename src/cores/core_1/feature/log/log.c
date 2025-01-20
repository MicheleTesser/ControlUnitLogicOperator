#include "log.h"
#include "sd/sd.h"
#include "telemetry/telemetry.h"
#include <string.h>

struct Log_t{
    struct LogSd_h sd;
    struct LogTelemetry_h telemetry;
};

union Log_h_t_conv{
    Log_h* const restrict hidden;
    struct Log_t* const restrict clear;
};

int8_t
log_init(Log_h* const restrict self __attribute__((__nonnull__)))
{
    union Log_h_t_conv conv = {self};
    struct Log_t* const restrict p_self = conv.clear;

    memset(p_self, 0, sizeof(*p_self));
    if(log_sd_init(&p_self->sd)<0)
    {
        return -1;
    }

    if(log_telemetry_init(&p_self->telemetry)<0)
    {
        return -1;
    }

    return 0;
}

int8_t
log_add_entry(Log_h* const restrict self __attribute__((__nonnull__)),
        const LogEntry_h* entry  __attribute__((__nonnull__)));

int8_t
log_update_and_send(Log_h* const restrict self __attribute__((__nonnull__)));
