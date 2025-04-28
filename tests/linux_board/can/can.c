#include "can.h"
#include "can_lib/canlib.h"
#include "raceup_board/components/timer.h"

#include <linux/can.h>
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
#define FIFO_BUFFER_SIZE 32


struct FifoBuffer{
  CanMessage buffer[FIFO_BUFFER_SIZE];
  uint8_t head;
  uint8_t tail;
  uint16_t filter_id;
  uint16_t filter_mask;
};

struct CanMailbox{
  uint8_t mailbox_index;
  struct FifoBuffer fifo_buffer;
  atomic_flag lock;
  atomic_bool running;
  thrd_t thread;
  int can_fd;
  int size;
  enum MAILBOX_TYPE type:2;
};

struct CanNode{
  const char* can_interface;
  struct CanMailbox r_mailbox[64];
  struct CanMailbox w_mailbox[32];
  struct CanMailbox fifo_mailbox[2];
  atomic_bool taken;
  uint8_t init_done:1;
};

enum MAILBOX_MODE{
  MAILBOX_RECV=0,
  MAILBOX_SEND=1,
};

static struct{
  struct CanNode nodes[__NUM_OF_CAN_MODULES__];
}BOARD_CAN_NODES;

static int _run_recv_mailbox_fifo_buffer(void* args)
{
  struct CanMailbox* const restrict self = args;

  while (atomic_load(&self->running))
  {
    struct can_frame frame = {0};

    if(can_recv_frame(self->can_fd, &frame)>0)
    {
      while (atomic_flag_test_and_set(&self->lock));
      struct FifoBuffer* fifo = &self->fifo_buffer;
      const uint16_t next_buffer = (fifo->head +1)%FIFO_BUFFER_SIZE;
      if (next_buffer != fifo->tail)
      {
        fifo->buffer[next_buffer].full_word = 0;
        fifo->buffer[next_buffer].id = 0;
        fifo->buffer[next_buffer].message_size = 0;

        fifo->buffer[next_buffer].message_size = frame.len;
        fifo->buffer[next_buffer].id = frame.can_id;
        memcpy(&fifo->buffer[next_buffer].full_word, frame.data, frame.can_dlc);
        fifo->head = next_buffer;
      }
      atomic_flag_clear(&self->lock);
    }
  }

  return 0;
}

static int _run_recv_mailbox_single_mex(void* args)
{
  struct CanMailbox* const restrict self = args;

  while (atomic_load(&self->running))
  {
    struct can_frame frame = {0};

    if(can_recv_frame(self->can_fd, &frame)>0)
    {
      while (atomic_flag_test_and_set(&self->lock));
      self->fifo_buffer.buffer[0].id = frame.can_id;
      self->fifo_buffer.buffer[0].message_size = frame.can_dlc;
      self->fifo_buffer.buffer[0].full_word =0;
      memcpy(&self->fifo_buffer.buffer[0].full_word, frame.data, frame.can_dlc);
      atomic_flag_clear(&self->lock);
    }
  }

  return 0;
}

static void _init_mailboxes(struct CanMailbox* const restrict mailbox_array, 
    const enum MAILBOX_TYPE type, const uint8_t size)
{
  for (uint8_t i=0; i<size; i++)
  {
    memset(&mailbox_array[i], 0, sizeof(mailbox_array[i]));
    mailbox_array[i].mailbox_index = i;
    mailbox_array[i].type = type;
  }
}

static void _free_mailboxes(struct CanMailbox* const restrict mailbox_array, const uint8_t size)
{
  struct CanMailbox* m = NULL;
  for (uint8_t i=0; i<size; i++)
  {
    m = &mailbox_array[i];
    if (atomic_load(&m->running))
    {
      hardware_free_mailbox_can(&m);
    }
  }
}

struct CanMailbox* _find_free_mailbox(struct CanMailbox* const restrict mailbox_array,
    const uint8_t size)
{
  for (uint8_t i=0; i<size; i++)
  {
    if (!atomic_exchange(&mailbox_array[i].running,1))
    {
      return &mailbox_array[i];
    }
  }

  return NULL;
}

//public

int8_t hardware_init_can(const enum CAN_MODULES mod,
    const enum CAN_FREQUENCY baud_rate __attribute__((__unused__)))
{
  const char* can_interface=NULL;
  if (mod >= __NUM_OF_CAN_MODULES__)
  {
    return -1;
  }

  struct CanNode* node = &BOARD_CAN_NODES.nodes[mod];

  if(node->init_done)
  {
    return -2;
  }

  while(!atomic_exchange(&node->taken,1));
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
      atomic_exchange(&node->taken,0);
      return -3;
  }
  node->can_interface = can_interface;
  node->init_done =1;

  _init_mailboxes(node->r_mailbox, RECV_MAILBOX, sizeof(node->r_mailbox)/sizeof(node->r_mailbox[0]));
  _init_mailboxes(node->w_mailbox, SEND_MAILBOX, sizeof(node->w_mailbox)/sizeof(node->w_mailbox[0]));
  _init_mailboxes(node->fifo_mailbox, FIFO_BUFFER,  sizeof(node->fifo_mailbox)/sizeof(node->fifo_mailbox[0]));

  atomic_exchange(&BOARD_CAN_NODES.nodes[mod].taken,0);
  return 0;
}

struct CanNode* hardware_init_can_get_ref_node(const enum CAN_MODULES mod)
{
  struct CanNode* node =  &BOARD_CAN_NODES.nodes[mod];
  if(node->init_done && !atomic_exchange(&node->taken, 1))
  {
    return node;
  }
  return NULL;
}

void hardware_init_can_destroy_ref_node(struct CanNode** restrict self)
{
  atomic_store(&(*self)->taken,0);
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

int8_t hardware_write_can(struct CanNode* const restrict self ,
    const CanMessage* restrict const mex )
{
  const int can_node = can_init(self->can_interface);
  struct can_frame frame={
    .can_id = mex->id,
    .can_dlc = mex->message_size,
  };

  memcpy(frame.data, mex->buffer, mex->message_size);
  if(can_send_frame(can_node, &frame)<0)
  {
    printf("sending mex with id %d failed\n", frame.can_id);
  }
  close(can_node);

  return 0;
}

struct CanMailbox* hardware_get_mailbox(struct CanNode* const restrict self,
    const enum MAILBOX_TYPE type, const uint16_t filter_id,
    const uint16_t filter_mask, uint16_t mex_size)
{
  struct CanMailbox* mailbox = NULL;
  switch (type)
  {
    case FIFO_BUFFER:
      mailbox =
        _find_free_mailbox(self->fifo_mailbox,sizeof(self->fifo_mailbox)/sizeof(self->fifo_mailbox[0]));
      break;
    case RECV_MAILBOX:
      mailbox = 
        _find_free_mailbox(self->r_mailbox,sizeof(self->r_mailbox)/sizeof(self->r_mailbox[0]));
      break;
    case SEND_MAILBOX:
      mailbox = 
        _find_free_mailbox(self->w_mailbox,sizeof(self->w_mailbox)/sizeof(self->w_mailbox[0]));
      break;
  }

  if (!mailbox)
  {
    return NULL;
  }

  mailbox->type = type;
  mailbox->fifo_buffer.filter_id = filter_id;
  mailbox->fifo_buffer.filter_mask = filter_mask;
  mailbox->size=mex_size;
  mailbox->can_fd = can_init_full(self->can_interface, filter_id, filter_mask);
  atomic_store(&mailbox->running,1);

  switch (type)
  {
    case FIFO_BUFFER:
      thrd_create(&mailbox->thread, _run_recv_mailbox_fifo_buffer, mailbox);
      break;
    case RECV_MAILBOX:
      thrd_create(&mailbox->thread, _run_recv_mailbox_single_mex, mailbox);
      break;
    case SEND_MAILBOX:
      break;
  }

  return  mailbox;
}

int8_t hardware_mailbox_read(struct CanMailbox* const restrict self,
    CanMessage* const restrict o_mex)
{
  uint16_t buffer_index=0;
  CanMessage* original_mex = NULL;
  struct FifoBuffer* fifo = NULL;

  if (self->type == SEND_MAILBOX)
  {
    return -1;
  }

  while (atomic_flag_test_and_set(&self->lock));
  switch (self->type)
  {
    case RECV_MAILBOX:
      buffer_index=0;
      break;
    case FIFO_BUFFER:
      fifo = &self->fifo_buffer;
      if (fifo->tail == fifo->head)
      {
        atomic_flag_clear(&self->lock);
        return -1;
      }
      buffer_index = fifo->tail;
      fifo->tail = (fifo->tail +1) % FIFO_BUFFER_SIZE;
      break;
    default:
      atomic_flag_clear(&self->lock);
      return -1;
  }
  original_mex = &self->fifo_buffer.buffer[buffer_index];
  memcpy(o_mex, original_mex, sizeof(*original_mex));
  memset(original_mex, 0, sizeof(*original_mex));
  atomic_flag_clear(&self->lock);


  return o_mex->id;
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
  switch ((*self)->type)
  {
    case FIFO_BUFFER:
      printf("closing fifo mailbox: (%p, %d)\n", (void*) *self, (*self)->mailbox_index);
      break;
    case RECV_MAILBOX:
      printf("closing recv mailbox: (%p, %d)\n", (void*) *self, (*self)->mailbox_index);
      break;
    case SEND_MAILBOX:
      printf("closing send mailbox: (%p, %d)\n", (void*) *self, (*self)->mailbox_index);
      break;
  }
  shutdown((*self)->can_fd, SHUT_RDWR);
  memset(*self, 0, sizeof(**self));
  (*self)->can_fd=-1;
  *self=NULL;
}

void stop_thread_can(void)
{
  struct CanNode* node = NULL;
  printf("stopping can module\n");
  for (uint16_t i=0; i<sizeof(BOARD_CAN_NODES.nodes)/sizeof(BOARD_CAN_NODES.nodes[0]); i++)
  {
    node = &BOARD_CAN_NODES.nodes[i];
    _free_mailboxes(node->r_mailbox, sizeof(node->r_mailbox)/sizeof(node->r_mailbox[0]));
    _free_mailboxes(node->w_mailbox, sizeof(node->w_mailbox)/sizeof(node->w_mailbox[0]));
    _free_mailboxes(node->fifo_mailbox, sizeof(node->fifo_mailbox)/sizeof(node->fifo_mailbox[0]));
  }
}


//emulation

struct CanNode* hardware_init_new_external_node(const enum CAN_MODULES mod)
{
  struct CanNode* node = calloc(1, sizeof(*node));
  const char* can_interface;
  switch (mod)
  {
    case CAN_INVERTER:
      can_interface = CAN_INTERFACE_0;
      break;
    case CAN_GENERAL:
      can_interface = CAN_INTERFACE_1;
      break;
    case CAN_DV:
      can_interface = CAN_INTERFACE_2;
      break;
    default:
      return NULL;
  }
  node->can_interface = can_interface;
  node->init_done =1;

  _init_mailboxes(node->r_mailbox, RECV_MAILBOX, sizeof(node->r_mailbox)/sizeof(node->r_mailbox[0]));
  _init_mailboxes(node->w_mailbox, SEND_MAILBOX, sizeof(node->w_mailbox)/sizeof(node->w_mailbox[0]));
  _init_mailboxes(node->fifo_mailbox, FIFO_BUFFER, sizeof(node->fifo_mailbox)/sizeof(node->fifo_mailbox[0]));

  return node;
}

void hardware_init_new_external_node_destroy(struct CanNode* const restrict self)
{
  free(self);
}

void hardware_can_node_debug_print_status(void)
{

  struct CanMailbox* m = NULL;
  struct CanNode* self = NULL;

  for (enum CAN_MODULES i=0; i<__NUM_OF_CAN_MODULES__; i++)
  {
    uint8_t num_r_taken =0;
    uint8_t num_w_taken =0;
    uint8_t num_fifo_taken =0;
    self = &BOARD_CAN_NODES.nodes[i];
    for (uint8_t i=0; i<sizeof(self->r_mailbox)/sizeof(self->r_mailbox[0]); i++)
    {
      m = &self->r_mailbox[i];
      if (atomic_load(&m->running))
      {
        num_r_taken++;
      }

    }

    for (uint8_t i=0; i<sizeof(self->w_mailbox)/sizeof(self->w_mailbox[0]); i++)
    {
      m = &self->w_mailbox[i];
      if (atomic_load(&m->running))
      {
        num_w_taken++;
      }

    }

    for (uint8_t i=0; i<sizeof(self->fifo_mailbox)/sizeof(self->fifo_mailbox[0]); i++)
    {
      m = &self->fifo_mailbox[i];
      if (atomic_load(&m->running))
      {
        num_fifo_taken++;
      }

    }
    printf("node: %d, case usage  read: %d, can usage write: %d, can usage fifo: %d\n",
        i, num_r_taken, num_w_taken, num_fifo_taken);
  }


}
