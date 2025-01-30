#include "sd.h"
#include <stdint.h>
struct LogSd_t{
    const uint8_t private_data[1];
};

union LogSd_h_t_conv{
  LogSd_h* const restrict hidden;
  struct LogSd_t* const restrict clear;
};

//private

#ifdef DEBUG
uint8_t __assert_log_sd_size[(sizeof(LogSd_h)==sizeof(struct LogSd_t))?1:-1];
#endif /* ifdef DEBUG */

int8_t
log_sd_init(LogSd_h* const restrict self )
{
  union LogSd_h_t_conv conv = {self};
  struct LogSd_t* const restrict p_self __attribute__((__unused__))= conv.clear;

  return 0;
}

int8_t
log_sd_write(LogSd_h* const restrict self ,
        const void* const restrict buffer , 
        const uint8_t buffer_size)__attribute__((__nonnull__(1,2)));
