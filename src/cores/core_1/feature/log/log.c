#include "log.h"
#include "external_log_variables/external_log_variables.h"
#include "sd/sd.h"
#include "telemetry/telemetry.h"
#include <stdint.h>
#include <string.h>

struct Log_t{
  LogSd_h sd;
  LogTelemetry_h telemetry;
};

union Log_h_t_conv{
  Log_h* const restrict hidden;
  struct Log_t* const restrict clear;
};

#ifdef DEBUG
char __assert_size_log[(sizeof(Log_h) == sizeof(struct Log_t))? 1:-1];
char __assert_align_log[(_Alignof(Log_h) == _Alignof(struct Log_t))? 1:-1];
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
    return -2;
  }

  return 0;
}

int8_t log_add_entry(Log_h* const restrict self,
    const LogEntry_h* entry)
{
  union Log_h_t_conv conv = {self};
  struct Log_t* const restrict p_self = conv.clear;

  if (entry->log_mode & LOG_TELEMETRY) {
    log_telemetry_add_entry(&p_self->telemetry, entry->name,
        entry->data_ptr, entry->data_format, entry->data_mode);
  }
  

  //TODO: SD

  return 0;
}

int8_t
log_update_and_send(Log_h* const restrict self)
{
  union Log_h_t_conv conv = {self};
  struct Log_t* const restrict p_self = conv.clear;

  //TODO: SD
  return log_telemetry_send(&p_self->telemetry);
}
