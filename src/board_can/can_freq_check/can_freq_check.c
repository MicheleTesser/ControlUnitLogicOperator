#include "can_freq_check.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/cdefs.h>


//private

struct MexInfo{
    uint16_t can_id;
    time_var_microseconds freq;
    time_var_microseconds deadline;
    fault_manager fault_f;
};

struct BstNode{
    time_var_microseconds deadline;
    struct MexInfo* metadata;
    struct BstNode* l_child;
    struct BstNode* r_child;
};

struct NodeQueue{
    uint8_t* nodes;
    int16_t next_free;
    uint8_t queue_size;
};

struct bst{
    struct NodeQueue free_queue;
    struct BstNode* node_pool;
    struct BstNode* node_root;
};

static struct bst* bst_new(const uint16_t initial_size)
{
    struct bst* res = calloc(1,sizeof(*res));
    res->node_pool = calloc(initial_size, sizeof(*res->node_pool));
    res->free_queue.nodes = calloc(initial_size, sizeof(*res->free_queue.nodes));
    for (uint16_t i =0; i< initial_size; i++) {
        res->free_queue.nodes[i] = i;
    }
    res->free_queue.next_free = initial_size -1;
    res->free_queue.queue_size = initial_size;

    return  res;
}

static struct BstNode* bst_node_min(struct BstNode* const restrict self)
{
    struct BstNode* parent= NULL;
    struct BstNode* node = self;

    while (node) {
        parent = node;
        node = node->l_child;
    }

    return parent;
}

static struct BstNode* bst_node_max(struct BstNode* const restrict self)
{
    struct BstNode* parent= NULL;
    struct BstNode* node = self;

    while (node) {
        parent = node;
        node = node->r_child;
    }

    return parent;
}

static int8_t bst_push(struct bst* const restrict self, const time_var_microseconds deadline, 
        struct MexInfo* const restrict metadata )
{
    if (self->free_queue.next_free < 0) {
        const uint8_t old_size = self->free_queue.queue_size;
        const uint8_t new_size = old_size*2;

        self->free_queue.nodes = realloc(self->free_queue.nodes, new_size);
        for (uint8_t i=0; i< old_size; i++) {
            self->free_queue.nodes[i] = old_size + i;
        }
        self->free_queue.next_free = old_size -1;
        self->free_queue.queue_size = new_size;
    }

    struct BstNode* node = &self->node_pool[self->free_queue.nodes[self->free_queue.next_free]];
    self->free_queue.next_free--;
    node->deadline = deadline;
    node->metadata = metadata;
    node->l_child = NULL;
    node->r_child = NULL;

    struct BstNode* parent = NULL;
    struct BstNode* cursor = self->node_root;

    while (cursor) {
        parent = cursor;
        if (deadline < cursor->deadline)
        {
            cursor = cursor->l_child;
        }
        else if (deadline >= cursor->deadline)
        {
            cursor = cursor->r_child;
        }
    }

    if (!parent && !cursor)
    {
        self->node_root = node;
    }
    else if(parent->r_child == cursor)
    {
        parent->r_child = node;
    }
    else if (parent->l_child == cursor)
    {
        parent->l_child = node;
    }

    return 0;
}

static int8_t bst_pop(struct bst* const restrict self, 
        const time_var_microseconds deadline , const uint16_t can_id )
{
    if (!self->node_root) {
        return -1;
    }

    struct BstNode* parent = self->node_root;
    struct BstNode* cursor = self->node_root;
    struct BstNode* new = NULL;
    int8_t last_move = -1;

    while (cursor && (cursor->deadline != deadline || cursor->metadata->can_id != can_id)) {
        parent = cursor;
        if (deadline < cursor->deadline)
        {
            cursor = cursor->l_child;
            last_move =0;
        }
        else if (deadline >= cursor->deadline)
        {
            cursor = cursor->r_child;
            last_move =1;
        }
    }

    if (!cursor) {
        return  -1;
    }

    if (cursor->r_child)
    {
        struct BstNode* min = bst_node_min(cursor->r_child);
        min->l_child = cursor->l_child;
        new = cursor->r_child;
    }
    else if(cursor->l_child)
    {
        struct BstNode* max = bst_node_max(cursor->l_child);
        max->r_child = cursor->r_child;
        new = cursor->l_child;
    }

    switch (last_move) {
        case 0:
            parent->l_child = new;
            break;
        case 1:
            parent->r_child = new;
            break;
        default:
            self->node_root = new;
    }

    uint8_t size_of_node = sizeof(*cursor);
    uint8_t index_node = (cursor - self->node_pool)/size_of_node;
    self->free_queue.nodes[self->free_queue.next_free] = index_node;
    self->free_queue.next_free += 1;

    return 0;
}


static struct{
    uint8_t info_pool_size;
    uint8_t info_pool_cursor;
    struct MexInfo* info_pool;
    struct bst* freq_tree;
}FREQ_TREE;

//public

int8_t can_freq_class_init(const uint16_t amount_of_ids)
{
    int8_t err=0;
    if (FREQ_TREE.info_pool) {
        return -1; //INFO: already init
    }

    FREQ_TREE.info_pool_size = amount_of_ids;
    FREQ_TREE.info_pool_cursor = 0;
    FREQ_TREE.info_pool = calloc(amount_of_ids, sizeof(*FREQ_TREE.info_pool));
    if (!FREQ_TREE.info_pool) {
        goto failed_malloc_info_pool;
    }
    FREQ_TREE.freq_tree = bst_new(amount_of_ids);
    if (!FREQ_TREE.freq_tree) {
        goto failed_init_bst;   
    }

    return 0;

failed_init_bst:
    free(FREQ_TREE.info_pool);
    err--;

failed_malloc_info_pool:
    err--;
    memset(&FREQ_TREE, 0, sizeof(FREQ_TREE));

    return err;
}

int8_t can_freq_add_mex_freq(const uint16_t can_id, const time_var_microseconds freq, 
        const fault_manager fault_fun)
{
    if (FREQ_TREE.info_pool_cursor >= FREQ_TREE.info_pool_size) {
        FREQ_TREE.info_pool = realloc(FREQ_TREE.info_pool, FREQ_TREE.info_pool_size * 2);
        FREQ_TREE.info_pool_size*=2;
    }

    struct MexInfo* info = &FREQ_TREE.info_pool[FREQ_TREE.info_pool_cursor];

    info->freq = freq;
    info->can_id = can_id;
    info->fault_f = fault_fun;
    FREQ_TREE.info_pool_cursor++;
    
    const time_var_microseconds init_value = timer_time_now() + freq;
    return bst_push(FREQ_TREE.freq_tree, init_value , info);
}

int8_t can_freq_update_freq(const uint16_t can_id)
{
    struct MexInfo* info = NULL;
    time_var_microseconds deadline = timer_time_now();
    for (uint16_t i=0; i<FREQ_TREE.info_pool_size; i++) {
        if (FREQ_TREE.info_pool[i].can_id == can_id) {
            info = &FREQ_TREE.info_pool[i];
            break;
        }
    }
    if (!info) {
        return -1;
    }
    deadline += info->freq;
    bst_pop(FREQ_TREE.freq_tree, info->deadline, can_id);
    bst_push(FREQ_TREE.freq_tree, deadline , info);
    info->deadline = deadline;

    return 0;
}


int8_t can_freq_check_faults(void)
{
    const struct BstNode* min = bst_node_min(FREQ_TREE.freq_tree->node_root);
    if (min && min->deadline < timer_time_now()) {
        bst_pop(FREQ_TREE.freq_tree, min->deadline, min->metadata->can_id);
        min->metadata->fault_f();
        return min->metadata->can_id;
    }
    return 0;
}
