#include "log.h"
#include "log_obj_types.h"
#include "sd/sd.h"
#include "telemetry/telemetry.h"
#include <stdint.h>
#include <string.h>

struct Log_t{
  struct LogSd_h sd;
  struct LogTelemetry_h telemetry;
};

union Log_h_t_conv{
  Log_h* const restrict hidden;
  struct Log_t* const restrict clear;
};

#ifdef DEBUG
char __assert_size_log[(sizeof(Log_h) == sizeof(struct Log_t))? 1:-1];
#endif // DEBUG

  int8_t
log_init(Log_h* const restrict self )
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
log_add_entry(Log_h* const restrict self ,
    const LogEntry_h* entry, const DataPosition position)
{
  union Log_h_t_conv conv = {self};
  struct Log_t* const restrict p_self = conv.clear;
  const uint32_t data_range = entry->data_max - entry->data_min;

  if (entry->log_mode & LOG_TELEMETRY) {
    log_telemetry_add_entry(&p_self->telemetry, entry->name,
        entry->data_ptr, entry->data_mode, data_range, position);
  }
  
  return 0;
}

int8_t
log_update_and_send(Log_h* const restrict self __attribute__((__unused__)))
{
  return 0;
}
