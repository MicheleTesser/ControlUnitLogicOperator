#ifndef __TELEMETRY_SD__
#define __TELEMETRY_SD__

#include <stdint.h>
#include "../log_obj_types.h"

typedef struct LogTelemetry_h{
    const uint8_t private_data[32];
}LogTelemetry_h;

int8_t
log_telemetry_init(LogTelemetry_h* const restrict self )__attribute__((__nonnull__(1)));
int8_t
log_telemetry_add_entry(LogTelemetry_h* const restrict self ,
        const char* const restrict name, const void* const restrict var,
        const enum DATA_MODE data_type, const uint32_t data_range)__attribute__((__nonnull__(1,2,3)));

#endif // !__TELEMETRY_SD__
