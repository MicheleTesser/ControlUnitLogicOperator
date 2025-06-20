#ifndef __TELEMETRY_SD__
#define __TELEMETRY_SD__

#include <stdint.h>
#include "../log_obj_types.h"
#include "../../../../../lib/raceup_board/raceup_board.h"

typedef struct __attribute__((aligned(4))){
  const uint8_t private_data[16];
}LogTelemetry_h;


int8_t
log_telemetry_init(LogTelemetry_h* const restrict self )__attribute__((__nonnull__(1)));

//INFO: name string MUST me null terminating
int8_t
log_telemetry_add_entry(
    LogTelemetry_h* const restrict self,
    const char* const restrict name, const char* const data_format,
    const void* const restrict var, const enum DATA_MODE data_type)
  __attribute__((__nonnull__(1,2,4)));

int8_t
log_telemetry_send(LogTelemetry_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t
log_telemetry_destroy(LogTelemetry_h* const restrict self)__attribute__((__nonnull__(1)));

#endif // !__TELEMETRY_SD__
