#include "can.h"
#include "can_lib/canlib.h"
#include "../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#include <linux/can.h>
#include <math.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <unistd.h>
#include <sys/socket.h>

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
  int size;
  uint8_t running:1;
  enum MAILBOX_TYPE type:2;
};

enum MAILBOX_MODE{
  MAILBOX_RECV=0,
  MAILBOX_SEND=1,
};

static struct{
  struct CanMailbox pool[NUM_OF_MAILBOX];
  uint16_t last_assigned;
}MAILBOX_POOL;

static struct{
  struct CanNode nodes[__NUM_OF_CAN_MODULES__];
}BOARD_CAN_NODES;

static int _run_recv_mailbox(void* args)
{
  struct CanMailbox* const restrict self = args;

  while (self->running)
  {
    struct can_frame frame = {0};

    if(can_recv_frame(self->can_fd, &frame)>0)
    {
      while (atomic_flag_test_and_set(&self->lock));
      switch (self->type)
      {
        case RECV_MAILBOX:
          self->fifo_buffer.buffer[0].id = frame.can_id;
          memcpy(&self->fifo_buffer.buffer[0].full_word, frame.data, sizeof(frame.data));
          break;
        case FIFO_BUFFER:
          struct FifoBuffer* fifo = &self->fifo_buffer;
          const uint16_t next_buffer = (fifo->head +1)%NUM_OF_MAILBOX;
          if (next_buffer != fifo->tail)
          {
            fifo->head = next_buffer;
            fifo->buffer[fifo->head].message_size = frame.len;
            fifo->buffer[fifo->head].id = frame.can_id;
            memcpy(&fifo->buffer[fifo->head].full_word, frame.data,
                sizeof(fifo->buffer[fifo->head].full_word));
          }
          break;
        default:
          break;
      }
      atomic_flag_clear(&self->lock);
    }
  }

  return 0;
}

//public

int8_t hardware_init_can(const enum CAN_MODULES mod,
    const enum CAN_FREQUENCY baud_rate __attribute__((__unused__)))
{
  const char* can_interface=NULL;
  if (mod == __NUM_OF_CAN_MODULES__)
  {
    return -1;
  }
  if(BOARD_CAN_NODES.nodes[mod].init_done)
  {
    return -2;
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
      return -3;
  }
  BOARD_CAN_NODES.nodes[mod].can_interface = can_interface;
  BOARD_CAN_NODES.nodes[mod].init_done =1;

  atomic_flag_clear(&BOARD_CAN_NODES.nodes[mod].taken);
  return 0;
}

struct CanNode* hardware_init_can_get_ref_node(const enum CAN_MODULES mod)
{
  struct CanNode* node =  &BOARD_CAN_NODES.nodes[mod];
  while(atomic_flag_test_and_set(&node->taken));
  if (!node->init_done) {
    atomic_flag_clear(&node->taken);
    return NULL;
  }
  return node;
}

void hardware_init_can_destroy_ref_node(struct CanNode** restrict self)
{
  atomic_flag_clear(&(*self)->taken);
  self=NULL;
}

int8_t hardware_read_can(struct CanNode* const restrict self, CanMessage* const restrict mex)
{
  struct can_frame frame={0};
  int can_node = can_init(self->can_interface);
  can_recv_frame(can_node, &frame);
  mex->message_size = frame.len;
  mex->id = frame.can_id;
  memcpy(mex->buffer, frame.data, frame.len);
  close(can_node);
  return 0;
}

int8_t hardware_write_can(const struct CanNode* const restrict self ,
    const CanMessage* restrict const mex )
{
  struct can_frame frame={0};
  int can_node = can_init(self->can_interface);
  frame.can_id = mex->id;
  frame.len = mex->message_size;
  memcpy(frame.data, mex->buffer, mex->message_size);
  can_send_frame(can_node, &frame);
  close(can_node);
  return 0;
}

struct CanMailbox* hardware_get_mailbox(struct CanNode* const restrict self,
    const enum MAILBOX_TYPE type, const uint16_t filter_id,
    const uint16_t filter_mask, uint16_t mex_size)
{
  struct CanMailbox* mailbox;
  uint16_t maillbox_index = MAILBOX_POOL.last_assigned++;

  if (maillbox_index >= NUM_OF_MAILBOX) {
    return NULL;
  }

  mailbox = &MAILBOX_POOL.pool[maillbox_index];

  mailbox->type = type;
  mailbox->fifo_buffer.filter_id = filter_id;
  mailbox->size=mex_size;
  mailbox->running=1;
  mailbox->can_fd = can_init_full(self->can_interface, filter_id, filter_mask);

  switch (type)
  {
    case FIFO_BUFFER:
      mailbox->fifo_buffer.tail=0;
      mailbox->fifo_buffer.head=1;
      mailbox->fifo_buffer.filter_mask = filter_mask;
      thrd_create(&mailbox->thread, _run_recv_mailbox, mailbox);
      break;
    case RECV_MAILBOX:
      thrd_create(&mailbox->thread, _run_recv_mailbox, mailbox);
      break;
    case SEND_MAILBOX:
      break;
    default:
      MAILBOX_POOL.last_assigned--;
      return NULL;
  }


  return  mailbox;
}

int8_t hardware_mailbox_read(struct CanMailbox* const restrict self,
    CanMessage* const restrict o_mex)
{
  uint16_t buffer_index=0;
  uint8_t size=0;

  if (self->type == SEND_MAILBOX)
  {
    return -1;
  }
  while (atomic_flag_test_and_set(&self->lock));
  switch (self->type)
  {
    case RECV_MAILBOX:
      buffer_index=0;
      size = self->size;
      break;
    case FIFO_BUFFER:
      struct FifoBuffer* fifo = &self->fifo_buffer;
      if (fifo->tail == fifo->head)
      {
        atomic_flag_clear(&self->lock);
        return -1;
      }
      buffer_index = fifo->tail;
      size = self->fifo_buffer.buffer[buffer_index].message_size;
      fifo->tail = (fifo->tail +1) % NUM_OF_MAILBOX;
      break;
    default:
      atomic_flag_clear(&self->lock);
      return -1;
  }
  o_mex->id = self->fifo_buffer.buffer[buffer_index].id;
  o_mex->message_size = size;
  memcpy(&o_mex->full_word, &self->fifo_buffer.buffer[buffer_index].full_word, sizeof(o_mex->full_word));

  atomic_flag_clear(&self->lock);
  //HACK: for a bug in the emulation some messages are wrong
  if (!o_mex->id || o_mex->id > (pow(2, 11)-1))
  {
    return -1;
  }
  return 0;
}

int8_t hardware_mailbox_send(struct CanMailbox* const restrict self, const uint64_t data)
{
  if (self->type!=SEND_MAILBOX || self->can_fd==-1)
  {
    return -1;
  }
  struct can_frame frame = {
    .can_id = self->fifo_buffer.filter_id,
    .can_dlc = self->size,
  };
  memcpy(frame.data, &data, sizeof(data));
  return can_send_frame(self->can_fd, &frame);
}

void hardware_free_mailbox_can(struct CanMailbox** restrict self)
{
  memset(*self, 0, sizeof(**self));
  *self=NULL;
}

void stop_thread_can(void)
{
  for (uint16_t i=0; i<NUM_OF_MAILBOX; i++)
  {
    struct CanMailbox* mail = &MAILBOX_POOL.pool[i];
    if (mail->running)
    {
      printf("closing mail: %d\n",i);
      mail->running=0;
      shutdown(mail->can_fd, SHUT_RDWR);
      mail->can_fd=-1;
    }
  }
}


//emulation

struct CanNode* hardware_init_new_external_node(const enum CAN_MODULES mod)
{
  struct CanNode* node = calloc(1, sizeof(*node));
  const char* can_interface;
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
      return NULL;
  }
  node->can_interface = can_interface;
  node->init_done =1;

  return node;
}

void hardware_init_new_external_node_destroy(struct CanNode* const restrict self)
{
  free(self);
}

void hardware_init_can_debug_print_status(struct CanMailbox* const restrict self)
{
  printf("mailbox: id: %d\t, mailbox type: %d\t, mailbox running: %d\n",
      self->can_fd,
      self->type,
      self->running);
}
