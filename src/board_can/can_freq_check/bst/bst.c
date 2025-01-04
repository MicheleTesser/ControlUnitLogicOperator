#include "bst.h"
#include <stdint.h>
#include <stdlib.h>

//private

struct BstNode{
    void* key;
    struct BstNode* l_child;
    struct BstNode* r_child;
};

struct NodeQueue{
    uint8_t* nodes;
    int16_t next_free;
};

struct bst{
    struct NodeQueue free_queue;
    struct BstNode* node_pool;
    struct BstNode* node_root;
};

//public

struct bst* bst_new(const uint16_t initial_size)
{
    struct bst* res = calloc(sizeof(struct bst), 1);
    res->node_pool = calloc(initial_size, sizeof(*res->node_pool));
    res->free_queue.nodes = calloc(initial_size, sizeof(*res->free_queue.nodes));
    for (uint16_t i =0; i< initial_size; i++) {
        res->free_queue.nodes[i] = i;
    }
    res->free_queue.next_free = initial_size -1;

    return  res;
}

int8_t bst_push(struct bst* const restrict self, const void* key, const uint8_t key_size, 
        const void* const restrict other_data)
{
    return 0;
}

int8_t bst_pop(struct bst* const restrict self, const void* key, const uint8_t key_size)
{
    return 0;
}

const void* bst_min(struct bst* const restrict self, void* metadata)
{
    return NULL;
}
