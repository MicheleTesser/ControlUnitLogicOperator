#ifndef __TELEMETRY_SD__
#define __TELEMETRY_SD__

#include <stdint.h>
typedef struct LogTelemetry_h{
    const uint8_t private_data[1];
}LogTelemetry_h;

int8_t
log_telemetry_init(LogTelemetry_h* const restrict self __attribute__((__nonnull__)));

#endif // !__TELEMETRY_SD__
