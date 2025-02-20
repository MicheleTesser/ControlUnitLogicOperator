#include "steering_wheel.h"
#include "../../linux_board/linux_board.h"

#include <stdint.h>
#include <string.h>
#include <threads.h>
#include <stdio.h>

struct SteeringWheel_t{
  uint8_t running;
  thrd_t thread;
};

union SteeringWheel_h_t_conv{
  SteeringWheel_h* const hidden;
  struct SteeringWheel_t* const clear;
};

union SteeringWheel_h_t_conv_const{
  const SteeringWheel_h* const hidden;
  const struct SteeringWheel_t* const clear;
};

#ifdef DEBUG
char __assert_size_SteeringWheel[(sizeof(SteeringWheel_h)==sizeof(struct SteeringWheel_t))?+1:-1];
#endif /* ifdef DEBUG */

int _start_SteeringWheel(void* arg)
{
  struct SteeringWheel_t* const p_self = arg;

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

int8_t steering_wheel_start(struct SteeringWheel_h* const restrict self)
{
  union SteeringWheel_h_t_conv conv = {self};
  struct SteeringWheel_t* const p_self = conv.clear;

  memset(p_self, 0, sizeof(*p_self));

  thrd_create(&p_self->thread, _start_SteeringWheel, p_self);
  return 0;
}

int8_t steering_wheel_stop(struct SteeringWheel_h* const restrict self)
{
  union SteeringWheel_h_t_conv conv = {self};
  struct SteeringWheel_t* const p_self = conv.clear;

  printf("stopping SteeringWheel\n");
  p_self->running=0;
  thrd_join(p_self->thread, NULL);

  return 0;
}

