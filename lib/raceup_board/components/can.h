#ifndef __VIRTUAL_CAN__
#define __VIRTUAL_CAN__

#include <stdint.h>

typedef struct{
  uint16_t id;
  union{
    uint8_t buffer[8];
    uint32_t words[2];
    uint64_t full_word;
  };
  uint8_t message_size;
}CanMessage;

enum CAN_FREQUENCY{
  _1_MBYTE_S_ = 1000000L,
  _500_KBYTE_S_ = 500000L,
  _250_KBYTE_S_ = 250000L,
  _125_KBYTE_S_ = 125000L,
  _50_KBYTE_S_ = 50000L,
  _20_KBYTE_S_ = 20000L,
  _10_KBYTE_S_ = 10000L,
  _5_KBYTE_S_ = 5000L,
};

enum CAN_MODULES{
  CAN_INVERTER=0,
  CAN_GENERAL,
  CAN_DV,

  __NUM_OF_CAN_MODULES__
};

struct CanNode;
struct CanMailbox;

enum MAILBOX_TYPE
{
  FIFO_BUFFER,
  RECV_MAILBOX,
  SEND_MAILBOX,
};

int8_t
hardware_init_can(const enum CAN_MODULES mod , const enum CAN_FREQUENCY baud_rate);

struct CanNode*
hardware_init_can_get_ref_node(const enum CAN_MODULES mod);
void
hardware_init_can_destroy_ref_node(struct CanNode** restrict self)
  __attribute__((__nonnull__(1)));

extern int8_t
hardware_read_can(struct CanNode* const restrict self ,
    CanMessage* const restrict mex )__attribute__((__nonnull__(1,2)));

extern int8_t
hardware_write_can(const struct CanNode* const restrict self ,
    const CanMessage* restrict const mex )__attribute__((__nonnull__(1,2)));

extern struct CanMailbox*
hardware_get_mailbox(struct CanNode* const restrict self, const enum MAILBOX_TYPE type,
    uint16_t filter_id, const uint16_t filter_mask,
    const uint8_t mex_size)__attribute__((__nonnull__(1)));

#define hardware_get_mailbox_single_mex(self,type,filter_id,mex_size)\
  hardware_get_mailbox(self,type,filter_id,(uint16_t)(~0),mex_size)

extern int32_t
hardware_mailbox_read(struct CanMailbox* const restrict self ,
    CanMessage* const restrict o_mex)__attribute__((__nonnull__(1,2)));

extern int8_t
hardware_mailbox_send(struct CanMailbox* const restrict self ,
    const uint64_t data)__attribute__((__nonnull__(1)));

extern void
hardware_free_mailbox_can(struct CanMailbox** restrict self)__attribute__((__nonnull__(1)));

#define ACTION_ON_CAN_NODE(node,p_node)\
  for (p_node =NULL;!p_node;p_node = hardware_init_can_get_ref_node(node)){}\
  for (uint8_t node_used=0;!node_used;(hardware_init_can_destroy_ref_node(&p_node), node_used=1))

#endif // !__VIRTUAL_CAN__
