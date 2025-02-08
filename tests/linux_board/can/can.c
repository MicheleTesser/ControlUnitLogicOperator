#include "can.h"
#include "can_lib/canlib.h"
#include "../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#include <linux/can.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <unistd.h>

#define CAN_INTERFACE_0 "culo_can_0"
#define CAN_INTERFACE_1 "culo_can_1"
#define CAN_INTERFACE_2 "culo_can_2"


typedef  uint8_t BoardComponentId;
#define NUM_OF_MAILBOX 1024

struct CanNode{
  const char* can_interface;
  atomic_flag taken;
  uint8_t init_done:1;
};

struct FifoBuffer{
  CanMessage buffer[32];
  uint8_t head;
  uint8_t tail;
  uint16_t filter_id;
  uint16_t filter_mask;
};

struct CanMailbox{
  struct FifoBuffer fifo_buffer;
  atomic_flag lock;
  thrd_t thread;
  int can_fd;
  enum MAILBOX_TYPE type:2;
};

enum MAILBOX_MODE{
  MAILBOX_RECV=0,
  MAILBOX_SEND=1,
};

static struct{
  struct CanMailbox pool[NUM_OF_MAILBOX];
  uint8_t last_assigned;
}MAILBOX_POOL;

static struct{
  struct CanNode nodes[__NUM_OF_CAN_MODULES__];
}BOARD_CAN_NODES;

static int run_recv_mailbox(void* args)
{
  struct CanMailbox* const restrict self = args;

  while (1)
  {
    struct can_frame frame = {0};

    can_recv_frame(self->can_fd, &frame);
    while (atomic_flag_test_and_set(&self->lock));
    switch (self->type)
    {
      case RECV_MAILBOX:
        self->fifo_buffer.buffer[0].id = frame.can_id;
        memcpy(&self->fifo_buffer.buffer[0].full_word, frame.data, sizeof(frame.data));
        break;
      case SEND_MAILBOX:
        break;
      case FIFO_BUFFER:
        //TODO: not yet defined
        break;
    }
    //TODO:
    atomic_flag_clear(&self->lock);
  }

  return 0;
}

//public

int8_t
hardware_init_can(const enum CAN_MODULES mod,
    const enum CAN_FREQUENCY baud_rate __attribute__((__unused__)))
{
  const char* can_interface=NULL;
  if (mod == __NUM_OF_CAN_MODULES__)
  {
    return -1;
  }
  while(atomic_flag_test_and_set(&BOARD_CAN_NODES.nodes[mod].taken));
  switch (mod) {
    case 0:
      can_interface = CAN_INTERFACE_0;
      break;
    case 1:
      can_interface = CAN_INTERFACE_1;
      break;
    case 2:
      can_interface = CAN_INTERFACE_2;
      break;
    default:
      atomic_flag_clear(&BOARD_CAN_NODES.nodes[mod].taken);
      return -1;
  }
  BOARD_CAN_NODES.nodes[mod].can_interface = can_interface;
  BOARD_CAN_NODES.nodes[mod].init_done =1;

  atomic_flag_clear(&BOARD_CAN_NODES.nodes[mod].taken);
  return 0;
}

  struct CanNode*
hardware_init_can_get_ref_node(const enum CAN_MODULES mod)
{
  struct CanNode* node =  &BOARD_CAN_NODES.nodes[mod];
  while(atomic_flag_test_and_set(&node->taken));
  if (!node->init_done) {
    atomic_flag_clear(&node->taken);
    return NULL;
  }
  return node;
}

void
hardware_init_can_destroy_ref_node(struct CanNode** restrict self)
{
  atomic_flag_clear(&(*self)->taken);
  self=NULL;
}

int8_t
hardware_read_can(struct CanNode* const restrict self __attribute_maybe_unused__,
    CanMessage* const restrict mex )
{
  struct can_frame frame;
  memset(&frame, 0, sizeof(frame));
  int can_node = can_init(self->can_interface);
  can_recv_frame(can_node, &frame);
  mex->message_size = frame.len;
  mex->id = frame.can_id;
  memcpy(mex->buffer, frame.data, frame.len);
  close(can_node);
  return 0;
}

int8_t
hardware_write_can(const struct CanNode* const restrict self ,
    const CanMessage* restrict const mex )
{
  struct can_frame frame;
  memset(&frame, 0, sizeof(frame));
  int can_node = can_init(self->can_interface);
  frame.can_id = mex->id;
  frame.len = mex->message_size;
  memcpy(frame.data, mex->buffer, mex->message_size);
  can_send_frame(can_node, &frame);
  close(can_node);
  return 0;
}

struct CanMailbox*
hardware_get_mailbox(struct CanNode* const restrict self, const enum MAILBOX_TYPE type,
    const uint16_t filter_id, const uint16_t filter_mask, uint16_t mex_size)
{
  struct CanMailbox* mailbox;
  uint16_t maillbox_index = MAILBOX_POOL.last_assigned++;

  if (maillbox_index >= NUM_OF_MAILBOX) {
    return NULL;
  }

  mailbox = &MAILBOX_POOL.pool[maillbox_index];

  mailbox->type = type;
  mailbox->fifo_buffer.filter_id = filter_id;
  mailbox->fifo_buffer.buffer[0].message_size = mex_size; 

  switch (type)
  {
    case FIFO_BUFFER:
      mailbox->fifo_buffer.filter_mask = filter_mask;
      mailbox->can_fd = can_init_full(self->can_interface, filter_id, filter_mask);
      thrd_create(&mailbox->thread, run_recv_mailbox, mailbox);
      break;
    case RECV_MAILBOX:
      mailbox->can_fd = can_init_full(self->can_interface, filter_id, ~0);
      thrd_create(&mailbox->thread, run_recv_mailbox, mailbox);
      break;
    case SEND_MAILBOX:
      mailbox->can_fd = can_init(self->can_interface);
      break;
    default:
      MAILBOX_POOL.last_assigned--;
      return NULL;
  }


  return  mailbox;
}

int8_t
hardware_mailbox_read(struct CanMailbox* const restrict self ,
    CanMessage* const restrict o_mex)
{
  if (self->type == SEND_MAILBOX)
  {
    return -1;
  }
  while (atomic_flag_test_and_set(&self->lock));
  memcpy(o_mex, &self->fifo_buffer.buffer[0], sizeof(*o_mex));
  atomic_flag_clear(&self->lock);
  return 0;
}

int8_t
hardware_mailbox_send(struct CanMailbox* const restrict self,
    const uint64_t data)
{
  if (self->type!=SEND_MAILBOX)
  {
    return -1;
  }
  CanMessage* mex = &self->fifo_buffer.buffer[0];

  mex->full_word = data;
  struct can_frame frame = {
    .can_id = mex->id,
    .can_dlc = mex->message_size,
  };
  memcpy(frame.data, &data, sizeof(data));
  return can_send_frame(self->can_fd, &frame);
}

extern void
hardware_free_mailbox_can(struct CanMailbox** restrict self)
{
  memset(*self, 0, sizeof(**self));
  self=NULL;
}
