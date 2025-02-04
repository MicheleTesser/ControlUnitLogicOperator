#include "score_lib/test_lib.h"
#include "src/cores/core_1/feature/log/telemetry/telemetry.h"
#include "linux_board/linux_board.h"
#include <stdint.h>
#include <stdio.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <unistd.h>

int log_var_a = 'a';
float log_var_b = 12.5f;
unsigned long log_var_c = 912;
uint8_t log_var_d = 5;

#define LOG_VAR(p_self, VAR, data_type, data_range, POS)\
  log_telemetry_add_entry(p_self, #VAR, &log_var_a, data_type, data_range,POS)

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

  if (LOG_VAR(&o_telemetry, log_var_a, DATA_SIGNED, 'c' - 'a',1)<0)
  {
    FAILED("failed to store the log entry for log_var_a");
    goto destroy_log;
  }
  PASSED("inserted log_var_a");

  if (LOG_VAR(&o_telemetry, log_var_b, DATA_FLOATED, 20.0f - 10.0f ,2)<0)
  {
    FAILED("failed to store the log entry for log_var_b");
    goto destroy_log;
  }
  PASSED("inserted log_var_b");

  if (LOG_VAR(&o_telemetry, log_var_c, DATA_UNSIGNED, 1000 - 500 ,3)<0)
  {
    FAILED("failed to store the log entry for log_var_c");
    goto destroy_log;
  }
  PASSED("inserted log_var_c");

  if (LOG_VAR(&o_telemetry, log_var_d, DATA_UNSIGNED, 20 - 0 ,3)<0)
  {
    PASSED("prevent to store the log entry for log_var_d with already used position");
  }
  else
  {
    FAILED("did not prevent to store the log entry for log_var_d with already used position");
  }

  if (LOG_VAR(&o_telemetry, log_var_d, DATA_UNSIGNED, 20 - 0 ,4)<0)
  {
    FAILED("failed to store the log entry for log_var_d");
    goto destroy_log;
  }
  PASSED("inserted log_var_d");



  PASSED("all the variable has been stored in the log class");


destroy_log:
  log_telemetry_destroy(&o_telemetry);
end:
  print_SCORE();
  return 0;
}
