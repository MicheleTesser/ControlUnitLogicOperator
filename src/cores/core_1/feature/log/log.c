#include "log.h"
#include "sd/sd.h"
#include "telemetry/telemetry.h"
#include "can_log/can_log.h"
#include "../../../core_utility/core_utility.h"
#include <stdint.h>
#include <string.h>

struct Log_t{
  CanLog_h m_can;
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

int8_t log_init(Log_h* const restrict self )
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

  if (can_log_init(&p_self->m_can)<0)
  {
    return -3;
  }

  return 0;
}

int8_t log_add_entry(Log_h* const restrict self,
    const LogEntry_h* entry)
{
  union Log_h_t_conv conv = {self};
  struct Log_t* const restrict p_self = conv.clear;
  int8_t err=0;

  if (entry->log_mode & LOG_TELEMETRY) {
    if(log_telemetry_add_entry(
          &p_self->telemetry,
          entry->name,
          entry->data_format,
          entry->data_ptr,
          entry->data_mode)<0)
    {
      err +=1;
    }
  }
  

  //TODO: SD

  return -err;
}

int8_t log_update_and_send(Log_h* const restrict self)
{
  union Log_h_t_conv conv = {self};
  struct Log_t* const restrict p_self = conv.clear;
  int8_t err =0;

  if(log_telemetry_send(&p_self->telemetry)<0)
  {
    err |= 1;
  }
  if(can_log_update(&p_self->m_can)<0)
  {
    err |= 2;
  }
  //TODO: SD

  if (err)
  {
    SET_TRACE(CORE_1);
  }

  return -err;
}
