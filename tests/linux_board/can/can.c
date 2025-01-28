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

#define CAN_INTERFACE_0 "culo_can_0"
#define CAN_INTERFACE_1 "culo_can_1"
#define CAN_INTERFACE_2 "culo_can_2"


typedef  uint8_t BoardComponentId;

struct CanNode{
    uint8_t can_fd;
    atomic_flag taken;
    uint8_t init_done:1;
    uint8_t mex_to_read:1;
};

struct CanMailbox{
    const struct CanNode* can_node;
    CanMessage mex;
    uint8_t mailbox_mode:1;
    atomic_bool action_flag_mailbox;
    uint8_t init_done:1;
};

enum MAILBOX_MODE{
    MAILBOX_RECV=0,
    MAILBOX_SEND=1,
};

static struct{
    struct CanNode nodes[__NUM_OF_CAN_MODULES__];
}BOARD_CAN_NODES;

#define NUM_OF_MAILBOX 256
static struct{
    struct CanMailbox mailbox_pool[NUM_OF_MAILBOX];
    uint16_t last_assigned;
}MAILBOX_MANAGER;

static int virtual_can_manager(void* args __attribute_maybe_unused__)
{
    for (uint8_t i=0; i<MAILBOX_MANAGER.last_assigned; i= (i+1)%NUM_OF_MAILBOX) {
           
    }
    return 0;
}

//public

int8_t virtual_can_manager_init(void)
{
    thrd_t thrd;
    return thrd_create(&thrd, virtual_can_manager, NULL);
}


int8_t
hardware_init_can(const enum CAN_MODULES mod,
        const enum CAN_FREQUENCY baud_rate __attribute__((__unused__)))
{
    uint8_t can_fd;
    if (mod == __NUM_OF_CAN_MODULES__)
    {
        return -1;
    }
    while(atomic_flag_test_and_set(&BOARD_CAN_NODES.nodes[mod].taken));
    switch (mod) {
        case 0:
            can_fd = can_init(CAN_INTERFACE_0);
            break;
        case 1:
            can_fd = can_init(CAN_INTERFACE_1);
            break;
        case 2:
            can_fd = can_init(CAN_INTERFACE_2);
            break;
        default:
            atomic_flag_clear(&BOARD_CAN_NODES.nodes[mod].taken);
            return -1;
    }
    BOARD_CAN_NODES.nodes[mod].can_fd = can_fd;
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
hardware_read_can(struct CanNode* const restrict self ,
        CanMessage* const restrict mex )
{
    struct can_frame frame;
    memset(&frame, 0, sizeof(frame));
    int can_node = self->can_fd;
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
    int can_node = self->can_fd;
    frame.can_id = mex->id;
    frame.len = mex->message_size;
    memcpy(frame.data, mex->buffer, mex->message_size);
    can_send_frame(can_node, &frame);
    close(can_node);
    return 0;
}

struct CanMailbox*
hardware_get_mailbox(const struct CanNode* const restrict self,
        const uint16_t mex_id, const uint16_t mex_size)
{
    struct CanMailbox* mailbox;
    uint8_t maillbox_index = MAILBOX_MANAGER.last_assigned++;

    if (MAILBOX_MANAGER.last_assigned >= NUM_OF_MAILBOX) {
        return NULL;
    }

    mailbox = &MAILBOX_MANAGER.mailbox_pool[maillbox_index];

    mailbox->can_node = self;
    mailbox->mex.id = mex_id;
    mailbox->mex.message_size = mex_size;
    mailbox->init_done=1;
    mailbox->mailbox_mode =MAILBOX_RECV;


    return  mailbox;
}

struct CanMailbox*
hardware_get_mailbox_send(const struct CanNode* const restrict self,
        const uint16_t mex_id, const uint16_t mex_size)
{
    struct CanMailbox* mailbox;
    uint8_t maillbox_index = MAILBOX_MANAGER.last_assigned++;

    if (MAILBOX_MANAGER.last_assigned >= NUM_OF_MAILBOX) {
        return NULL;
    }

    mailbox = &MAILBOX_MANAGER.mailbox_pool[maillbox_index];

    mailbox->can_node = self;
    mailbox->mex.id = mex_id;
    mailbox->mex.message_size = mex_size;
    mailbox->init_done=1;
    mailbox->mailbox_mode =MAILBOX_SEND;

    return mailbox;
}

int8_t
hardware_mailbox_read(struct CanMailbox* const restrict self ,
        CanMessage* const restrict o_mex)
{
    if (atomic_load(&self->action_flag_mailbox))
    {
        memcpy(o_mex, &self->mex, sizeof(*o_mex));
        atomic_store(&self->action_flag_mailbox, 0);
        return self->mex.message_size;   
    }

    return -1;
}

int8_t
hardware_mailbox_send(struct CanMailbox* const restrict self,
        const uint64_t data)
{
    self->mex.full_word = data;
    atomic_store(&self->action_flag_mailbox, 1);
    return 0;
}

void
hardware_free_mailbox_can(struct CanMailbox* const* restrict self)
{
    struct CanMailbox* mailbox = *self;
    mailbox->init_done=1;
    printf("mailbox free index to set\n");
    self = NULL;
    return;
}
