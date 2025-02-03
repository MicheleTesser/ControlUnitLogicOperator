#include "score_lib/test_lib.h"
#include "src/cores/core_1/feature/log/telemetry/telemetry.h"
#include "linux_board/linux_board.h"
#include <stdint.h>
#include <stdio.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <unistd.h>

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

  log_telemetry_destroy(&o_telemetry);

end:
  print_SCORE();
  return 0;
}
