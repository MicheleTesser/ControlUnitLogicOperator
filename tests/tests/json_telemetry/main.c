#include "score_lib/test_lib.h"
#include "src/cores/core_1/feature/log/telemetry/telemetry.h"
#include "linux_board/linux_board.h"
#include <stdint.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <unistd.h>

int log_var_a = 'a';
float log_var_b = 12.5f;
unsigned long log_var_c = 912;
uint8_t log_var_d = 5;

#define LOG_VAR(p_self, VAR, data_type, POS)\
  log_telemetry_add_entry(p_self, #VAR, &log_var_a, data_type, POS)

int main(void)
{
  LogTelemetry_h o_telemetry;
  if(create_virtual_chip() <0){
    goto end;
  }

  if (log_telemetry_init(&o_telemetry)<0)
  {
    goto end;
  }

  if (LOG_VAR(&o_telemetry, log_var_a, __u8__,1)<0)
  {
    FAILED("failed to store the log entry for log_var_a");
    goto destroy_log;
  }
  PASSED("inserted log_var_a");

  if (LOG_VAR(&o_telemetry, log_var_b, __float__, 2)<0)
  {
    FAILED("failed to store the log entry for log_var_b");
    goto destroy_log;
  }
  PASSED("inserted log_var_b");

  if (LOG_VAR(&o_telemetry, log_var_c, __u32__, 3)<0)
  {
    FAILED("failed to store the log entry for log_var_c");
    goto destroy_log;
  }
  PASSED("inserted log_var_c");

  if (LOG_VAR(&o_telemetry, log_var_d, __u8__, 3)<0)
  {
    PASSED("prevent to store the log entry for log_var_d with already used position");
  }
  else
  {
    FAILED("did not prevent to store the log entry for log_var_d with already used position");
  }

  if (LOG_VAR(&o_telemetry, log_var_d, __u8__, 4)<0)
  {
    FAILED("failed to store the log entry for log_var_d");
    goto destroy_log;
  }
  PASSED("inserted log_var_d");



  PASSED("all the variable has been stored in the log class");

  log_telemetry_send(&o_telemetry);

destroy_log:
  log_telemetry_destroy(&o_telemetry);
end:
  print_SCORE();
  return 0;
}
