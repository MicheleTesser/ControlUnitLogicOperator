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
long log_var_c = 912;

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
    goto end;
  }

  log_telemetry_destroy(&o_telemetry);

end:
  print_SCORE();
  return 0;
}
