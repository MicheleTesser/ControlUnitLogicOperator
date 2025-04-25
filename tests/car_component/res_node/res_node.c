#include "res_node.h"
#include "../../linux_board/linux_board.h"

#include <stdint.h>
#include <string.h>
#include <threads.h>
#include <stdio.h>

struct res_node_t{
  struct CanNode* p_can_node_general;
  uint8_t running;
  thrd_t thread;
};

union res_node_h_t_conv{
  res_node_h* const hidden;
  struct res_node_t* const clear;
};

union res_node_h_t_conv_const{
  const res_node_h* const hidden;
  const struct res_node_t* const clear;
};

#ifdef DEBUG
char __assert_size_res_node[(sizeof(res_node_h)==sizeof(struct res_node_t))?+1:-1];
char __assert_align_res_node[(_Alignof(res_node_h)==_Alignof(struct res_node_t))?+1:-1];
#endif /* ifdef DEBUG */

int _start_res_node(void* arg)
{
  struct res_node_t* const p_self = arg;

  time_var_microseconds t_var =0;

  while (p_self->running)
  {
    ACTION_ON_FREQUENCY(t_var, 50 MILLIS)
    {
      //TODO: send res status
    }
  }

  return 0;
}


//public

int8_t res_node_start(res_node_h* const restrict self)
{
  union res_node_h_t_conv conv = {self};
  struct res_node_t* const p_self = conv.clear;

  memset(p_self, 0, sizeof(*p_self));

  p_self->p_can_node_general = hardware_init_new_external_node(CAN_GENERAL);
  if (!p_self->p_can_node_general)
  {
    return -1;
  }

  p_self->running=1;
  thrd_create(&p_self->thread, _start_res_node, p_self);
  return 0;
}

int8_t res_node_stop(res_node_h* const restrict self)
{
  union res_node_h_t_conv conv = {self};
  struct res_node_t* const p_self = conv.clear;

  printf("stopping res_node\n");
  p_self->running=0;
  thrd_join(p_self->thread, NULL);

  return 0;
}

