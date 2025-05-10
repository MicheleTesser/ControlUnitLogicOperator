#include "dummy.h"
#include "../../linux_board/linux_board.h"

#include <stdint.h>
#include <string.h>
#include <threads.h>
#include <stdio.h>

struct Dummy_t{
  uint8_t running;
  thrd_t thread;
};

union Dummy_h_t_conv{
  Dummy_h* const hidden;
  struct Dummy_t* const clear;
};

union Dummy_h_t_conv_const{
  const Dummy_h* const hidden;
  const struct Dummy_t* const clear;
};

#ifdef DEBUG
char __assert_size_dummy[(sizeof(Dummy_h)==sizeof(struct Dummy_t))?+1:-1];
char __assert_align_dummy[(_Alignof(Dummy_h)==_Alignof(struct Dummy_t))?+1:-1];
#endif /* ifdef DEBUG */

int _start_dummy(void* arg)
{
  struct Dummy_t* const p_self = arg;

  time_var_microseconds t_var =0;

  while (p_self->running)
  {
    ACTION_ON_FREQUENCY(t_var, get_tick_from_millis(50))
    {
    }
  }

  return 0;
}


//public

int8_t dummy_start(Dummy_h* const restrict self)
{
  union Dummy_h_t_conv conv = {self};
  struct Dummy_t* const p_self = conv.clear;

  memset(p_self, 0, sizeof(*p_self));

  p_self->running=1;
  thrd_create(&p_self->thread, _start_dummy, p_self);
  return 0;
}

int8_t dummy_stop(Dummy_h* const restrict self)
{
  union Dummy_h_t_conv conv = {self};
  struct Dummy_t* const p_self = conv.clear;

  printf("stopping dummy\n");
  p_self->running=0;
  thrd_join(p_self->thread, NULL);

  return 0;
}

