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
#define NUM_OF_MAILBOX 256

struct CanNode{
    uint8_t can_fd;
    atomic_flag taken;
    uint8_t init_done:1;
    uint8_t mex_to_read:1;
    uint8_t read_mailbox[NUM_OF_MAILBOX];
    uint8_t last_read_m;
    uint8_t write_mailbox[NUM_OF_MAILBOX];
    uint8_t last_write_m;
};

struct CanMailbox{
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

static struct{
    struct CanMailbox mailbox_pool[NUM_OF_MAILBOX];
    uint16_t last_assigned;
}MAILBOX_MANAGER;

static int read_mailbox_f(void* args )
{
    struct CanNode * const restrict node = args;
    for (;;) {
        CanMessage mex={0};
        hardware_read_can(node, &mex);
        for (uint8_t i=0; i<node->last_read_m; i++)
        {
            struct CanMailbox* m = &MAILBOX_MANAGER.mailbox_pool[node->read_mailbox[i]];
            if (m->mex.id == mex.id)
            {
                memcpy(&m->mex, &mex, sizeof(mex));
                atomic_store(&m->action_flag_mailbox, 1);
                break;
            }
        }
    }
    return 0;
}

static int write_mailbox_f(void* args)
{
    struct CanNode * const restrict node = args;
    for (uint8_t i=0; i<node->last_write_m; i= (i+1)%node->last_write_m) {
        struct CanMailbox* m = &MAILBOX_MANAGER.mailbox_pool[node->write_mailbox[i]];
        if (atomic_load(&m->action_flag_mailbox))
        {
            hardware_write_can(node,&m->mex);
            atomic_store(&m->action_flag_mailbox, 0);
        }
    }
    return 0;
}

//public

int8_t virtual_can_manager_init(void)
{
    thrd_t thrd;
    thrd_t thrd1;
    thrd_t thrd2;
    thrd_t thrd3;
    return thrd_create(&thrd, write_mailbox_f, NULL);
    return thrd_create(&thrd1, read_mailbox_f, &BOARD_CAN_NODES.nodes[0]);
    return thrd_create(&thrd2, read_mailbox_f, &BOARD_CAN_NODES.nodes[1]);
    return thrd_create(&thrd3, read_mailbox_f, &BOARD_CAN_NODES.nodes[2]);
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
hardware_get_mailbox(struct CanNode* const restrict self,
        const uint16_t mex_id, const uint16_t mex_size)
{
    struct CanMailbox* mailbox;
    uint8_t maillbox_index = MAILBOX_MANAGER.last_assigned++;

    if (MAILBOX_MANAGER.last_assigned >= NUM_OF_MAILBOX) {
        return NULL;
    }

    mailbox = &MAILBOX_MANAGER.mailbox_pool[maillbox_index];

    mailbox->mex.id = mex_id;
    mailbox->mex.message_size = mex_size;
    mailbox->init_done=1;
    mailbox->mailbox_mode =MAILBOX_RECV;

    self->read_mailbox[self->last_read_m++]=maillbox_index;

    return  mailbox;
}

struct CanMailbox*
hardware_get_mailbox_send(struct CanNode* const restrict self,
        const uint16_t mex_id, const uint16_t mex_size)
{
    struct CanMailbox* mailbox;
    uint8_t maillbox_index = MAILBOX_MANAGER.last_assigned++;

    if (MAILBOX_MANAGER.last_assigned >= NUM_OF_MAILBOX) {
        return NULL;
    }

    mailbox = &MAILBOX_MANAGER.mailbox_pool[maillbox_index];

    mailbox->mex.id = mex_id;
    mailbox->mex.message_size = mex_size;
    mailbox->init_done=1;
    mailbox->mailbox_mode =MAILBOX_SEND;

    self->write_mailbox[self->last_write_m++]=maillbox_index;

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
