#ifndef __CAR_LOG__
#define __CAR_LOG__

#include <stdint.h>

#include "../../../../lib/raceup_board/raceup_board.h"
#include "log_obj_types.h"

#define LOG_ENTRY_NAME_MAX_SIZE 32

typedef struct{
  uint8_t log_mode:2;
  enum DATA_MODE data_mode;
  const void* data_ptr;
  const char* data_format;
  char name[LOG_ENTRY_NAME_MAX_SIZE];
}LogEntry_h;

enum LOG_MODE{
  LOG_SD = (1<<0),
  LOG_TELEMETRY = (1<<1),
};

typedef struct __attribute__((aligned(4))){
  const uint8_t private_data[64];
}Log_h;

int8_t
log_init(Log_h* const restrict self )__attribute__((__nonnull__(1)));

int8_t
log_add_entry(Log_h* const restrict self ,
    const LogEntry_h* entry)__attribute__((__nonnull__(1,2)));

int8_t
log_update_and_send(Log_h* const restrict self )__attribute__((__nonnull__(1)));

#endif // !__CAR_LOG__
