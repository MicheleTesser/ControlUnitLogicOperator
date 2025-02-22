#include "ebs.h"
#include "../../linux_board/linux_board.h"

#include <stdint.h>
#include <string.h>
#include <threads.h>
#include <stdio.h>

struct Ebs_t{
  uint8_t running;
  thrd_t thread;
};

union ebs_h_t_conv{
  Ebs_h* const hidden;
  struct Ebs_t* const clear;
};

union ebs_h_t_conv_const{
  const Ebs_h* const hidden;
  const struct Ebs_t* const clear;
};

#ifdef DEBUG
char __assert_size_ebs[(sizeof(Ebs_h)==sizeof(struct Ebs_t))?+1:-1];
char __assert_align_ebs[(_Alignof(Ebs_h)==_Alignof(struct Ebs_t))?+1:-1];
#endif /* ifdef DEBUG */

int _start_ebs(void* arg)
{
  struct Ebs_t* const p_self = arg;

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

int8_t ebs_start(Ebs_h* const restrict self)
{
  union ebs_h_t_conv conv = {self};
  struct Ebs_t* const p_self = conv.clear;

  memset(p_self, 0, sizeof(*p_self));

  p_self->running=1;
  thrd_create(&p_self->thread, _start_ebs, p_self);
  return 0;
}

int8_t ebs_stop(Ebs_h* const restrict self)
{
  union ebs_h_t_conv conv = {self};
  struct Ebs_t* const p_self = conv.clear;

  printf("stopping ebs\n");
  p_self->running=0;
  thrd_join(p_self->thread, NULL);

  return 0;
}

