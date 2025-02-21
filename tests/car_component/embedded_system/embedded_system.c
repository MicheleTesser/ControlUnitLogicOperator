#include "embedded_system.h"
#include "../../linux_board/linux_board.h"
#include "../src/lib/board_dbc/dbc/out_lib/can3/can3.h"

#include <stdint.h>
#include <string.h>
#include <threads.h>
#include <stdio.h>

struct EmbeddedSystem_t{
  uint8_t running;
  thrd_t thread;
};

union EmbeddedSystem_h_t_conv{
  EmbeddedSystem_h* const hidden;
  struct EmbeddedSystem_t* const clear;
};

union EmbeddedSystem_h_t_conv_const{
  const EmbeddedSystem_h* const hidden;
  const struct EmbeddedSystem_t* const clear;
};

#ifdef DEBUG
char __assert_size_embedded_system[(sizeof(EmbeddedSystem_h)==sizeof(struct EmbeddedSystem_t))?+1:-1];
#endif /* ifdef DEBUG */

int _start_embedded_system(void* arg)
{
  struct EmbeddedSystem_t* const p_self = arg;

  time_var_microseconds t_var =0;

  while (p_self->running)
  {
    ACTION_ON_FREQUENCY(t_var, 50 MILLIS)
    {
    }
  }

  return 0;
}


//public

int8_t embedded_system_start(EmbeddedSystem_h* const restrict self)
{
  union EmbeddedSystem_h_t_conv conv = {self};
  struct EmbeddedSystem_t* const p_self = conv.clear;

  memset(p_self, 0, sizeof(*p_self));

  thrd_create(&p_self->thread, _start_embedded_system, p_self);
  return 0;
}

int8_t embedded_system_stop(EmbeddedSystem_h* const restrict self)
{
  union EmbeddedSystem_h_t_conv conv = {self};
  struct EmbeddedSystem_t* const p_self = conv.clear;

  printf("stopping embedded_system\n");
  p_self->running=0;
  thrd_join(p_self->thread, NULL);

  return 0;
}

