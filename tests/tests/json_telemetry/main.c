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

#define LOG_VAR(p_self, VAR, data_type, data_format)\
  log_telemetry_add_entry(p_self, #VAR, &log_var_a, data_format, data_type)

int main(void)
{
  LogTelemetry_h o_telemetry;

  if (log_telemetry_init(&o_telemetry)<0)
  {
    goto end;
  }

  //JSON 1
  if(log_telemetry_add_entry(&o_telemetry, "lap","", &log_var_a, __u8__)<0)
  {
    FAILED("failed to store the log entry for log_var_a");
    goto destroy_log;
  }
  PASSED("inserted log_var_a");

  //JSON 2
  if(log_telemetry_add_entry(&o_telemetry, "amk_temp_motor_fr", "", &log_var_b, __float__)<0)
  {
    FAILED("failed to store the log entry for log_var_b");
    goto destroy_log;
  }
  PASSED("inserted log_var_b");

  //JSON 3
  if(log_telemetry_add_entry(&o_telemetry, "lv_soc", "", &log_var_c, __u32__)<0)
  {
    FAILED("failed to store the log entry for log_var_c");
    goto destroy_log;
  }
  PASSED("inserted log_var_c");

  if(log_telemetry_add_entry(&o_telemetry, "lv_soc", "", &log_var_d, __u8__)<0)
  {
    PASSED("prevent to store the log entry for log_var_d with already used var: lv_soc");
  }
  else
  {
    FAILED("did not prevent to store the log entry for log_var_d with already used var: lv_soc");
  }

  //JSON 1
  if(log_telemetry_add_entry(&o_telemetry, "amk_status_fr", "", &log_var_d, __u8__)<0)
  {
    FAILED("failed to store the log entry for log_var_d");
    goto destroy_log;
  }
  PASSED("inserted log_var_d");


  // FAILED("log telemetry send bugged. TO FIX. HEAP BUFFER OVERFLOW");
  log_telemetry_send(&o_telemetry);

destroy_log:
  log_telemetry_destroy(&o_telemetry);
end:
  print_SCORE();
  return 0;
}
