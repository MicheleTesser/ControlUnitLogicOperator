#include "bst.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef int16_t array_index;

struct queue{
    uint8_t queue_size;
    int8_t cursor;

    uint8_t nodes_queue[];
};

struct sub_node{
    array_index data_index;
    array_index l_child_index;
    array_index r_child_index;
};

struct sub_search{
    struct queue* sub_queue;
    key_compare* cmp_fun;
    key_update* update_fun;
    array_index root;
    uint8_t pool_size;
    struct sub_node node_pool[];
};

struct bst{
    struct sub_search* search_tree[2];

    uint8_t data_size;
    uint8_t data_amount;
    array_index data_cursor;
    uint8_t data[];
};

static inline void* get_data_from_index(struct bst* const restrict self, const uint8_t index)
{
    return self->data + (self->data_size * index);
}

static inline void add_free_data_index(struct queue* self, const uint8_t index)
{
    self->nodes_queue[++self->cursor] = index;
}

static struct queue* queue_new(const uint8_t data_reserved)
{
    struct queue *queue= calloc(1, sizeof(*queue) + 
                                (data_reserved* sizeof(*queue->nodes_queue)));
    queue->cursor = -1;
    queue->queue_size = data_reserved;
    for (uint8_t i=0; i<data_reserved; i++){
        add_free_data_index(queue, i);
    }
    return queue;
}

static inline uint8_t get_new_data_index(struct queue* self)
{
    return self->nodes_queue[self->cursor--];
}

static struct sub_search* 
sub_bst_new(const uint8_t data_reserved,  key_compare cmp_fun, key_update update_fun)
{
    struct sub_search* sub = calloc(1, sizeof(*sub) + data_reserved * sizeof(*sub->node_pool));
    sub->sub_queue = queue_new(data_reserved);
    sub->cmp_fun = cmp_fun;
    sub->update_fun = update_fun;
    sub->root = -1;
    sub->pool_size = data_reserved;
    for (array_index i=0; i<data_reserved; i++) {
        sub->node_pool[i].data_index = -1;
        sub->node_pool[i].r_child_index = -1;
        sub->node_pool[i].l_child_index = -1;
    }
    return sub;
}

static int16_t sub_bst_min(struct sub_search* self, const uint8_t start_index)
{
    if (self->root <0) {
        return -1;
    }

    int16_t parent = -1;
    int16_t cursor = start_index;
    
    while (cursor >= 0) {
        parent = cursor;
        cursor = self->node_pool[cursor].l_child_index;
    }

    return  self->node_pool[parent].data_index;
}

static int16_t sub_bst_max(struct sub_search* self, const uint8_t start_index)
{
    if (self->root <0) {
        return -1;
    }

    int16_t parent = -1;
    int16_t cursor = start_index;
    
    while (cursor >= 0) {
        parent = cursor;
        cursor = self->node_pool[cursor].r_child_index;
    }

    return  parent;
}

static int16_t sub_bst_search(struct bst* root, struct sub_search* self, 
        const void* const restrict key)
{
    key_compare* cmp_fun = self->cmp_fun;
    array_index cursor = self->root;
    struct sub_node* p_node = NULL;

    while (cursor >= 0) {
        p_node = &self->node_pool[cursor];
        void* data = get_data_from_index(root, p_node->data_index);
        int8_t cmp_res = cmp_fun(data,key);
        if (cmp_res > 0){
            cursor = p_node->r_child_index;
        }else if(cmp_res < 0){
            cursor = p_node->l_child_index;
        }else{
            break;
        }
    }

    return p_node->data_index;
}

static int8_t sub_bst_insert(struct bst* root, struct sub_search* self, 
        const uint8_t data_index)
{
    array_index new_index = get_new_data_index(self->sub_queue);
    void* key = get_data_from_index(root, data_index);
    key_compare* cmp_fun = self->cmp_fun;
    array_index parent = -1;
    array_index cursor = self->root;
    int8_t last_move = 0;
    struct sub_node* p_node = NULL;

    self->node_pool[new_index].data_index = data_index;
    if (self->root < 0)
    {
        self->root = new_index;
        return 0;
    }

    while (cursor >= 0) {
        parent = cursor;
        p_node = &self->node_pool[cursor];
        void* data = get_data_from_index(root, p_node->data_index);
        int8_t cmp_res = cmp_fun(data,key);
        if (cmp_res > 0){
            cursor = p_node->r_child_index;
            last_move=1;
        }else if(cmp_res < 0){
            cursor = p_node->l_child_index;
            last_move=-1;
        }else{
            break;
        }
    }

    if (last_move > 0)
    {
        self->node_pool[parent].r_child_index = new_index;
    }
    else if (last_move < 0)
    {
        self->node_pool[parent].l_child_index = new_index;
    }

    return 0;
}

static int16_t sub_bst_delete(struct bst* root, struct sub_search* self,
        const void* const restrict key)
{
    if (self->root <0) {
        return -1;
    }

    key_compare* cmp_fun = self->cmp_fun;
    array_index parent = -1;
    array_index cursor = self->root;
    int8_t last_move = 0;
    struct sub_node* p_node = NULL;
    
    while (cursor >= 0) {
        p_node = &self->node_pool[cursor];
        void* data = get_data_from_index(root, p_node->data_index);
        int8_t cmp_res = cmp_fun(data,key);
        if (cmp_res > 0){
            if (p_node->r_child_index >= 0) {
                parent = cursor;
                last_move=1;
            }
            cursor = p_node->r_child_index;
        }else if(cmp_res < 0){
            if (p_node->l_child_index >= 0) {
                parent = cursor;
                last_move=-1;
            }
            cursor = p_node->l_child_index;
        }else{
            break;
        }
    }

    if (cursor < 0) {
        return  -1;
    }

    array_index new_index = -1;
    struct sub_node* new = NULL;
    if (p_node->l_child_index >= 0)
    {
        new_index = sub_bst_max(self,p_node->l_child_index);
        new = &self->node_pool[new_index];
        new->r_child_index = p_node->r_child_index;
    }
    else if (p_node->r_child_index >= 0)
    {
        new_index = sub_bst_max(self,p_node->r_child_index);
        new = &self->node_pool[new_index];
        new->l_child_index = p_node->l_child_index;
    }

    struct sub_node* p_parent = &self->node_pool[parent];
    if (last_move < 0)
    {
        p_parent->l_child_index = new_index;
    }
    else if (last_move > 0)
    {
        p_parent->r_child_index = new_index;
    }
    else //root last_move == 0
    {
        self->root = -1;
    }


    return p_node->data_index;
}


static void sub_bst_free(struct sub_search** const restrict self)
{
    free((*self)->sub_queue);
    (*self)->sub_queue = NULL;
    free(*self);
    *self = NULL;
}

static int8_t sub_bst_resize(struct sub_search** const self_p, const uint8_t new_size)
{
    *self_p = realloc(*self_p, sizeof(**self_p) + new_size * sizeof(*(*self_p)->node_pool));
    struct sub_search* const self = *self_p;
    self->pool_size = new_size;
    self->sub_queue = realloc(self->sub_queue, 
            sizeof(*self->sub_queue) + new_size * sizeof(*self->sub_queue->nodes_queue));
    for (array_index i =self->sub_queue->queue_size; i<new_size; i++) {
        add_free_data_index(self->sub_queue, i);
        self->node_pool[i].data_index = -1;
        self->node_pool[i].r_child_index = -1;
        self->node_pool[i].l_child_index = -1;
    }
    self->sub_queue->queue_size = new_size;
    return 0;
}

//public

struct bst* bst_new(key_compare cmp_fun_id, key_compare cmp_fun_timeline, 
        key_update update_fun_1, key_update update_fun_2,
        const uint8_t initial_reserved_size, 
        const uint16_t ele_size)
{
    struct bst* res = calloc(1, sizeof(*res) + ele_size * initial_reserved_size);
    res->data_size = ele_size;
    res->data_amount = initial_reserved_size;
    res->data_cursor = 0;

    res->search_tree[0] = sub_bst_new(initial_reserved_size, cmp_fun_id, update_fun_1);
    res->search_tree[1] = sub_bst_new(initial_reserved_size, cmp_fun_timeline, update_fun_2);

    return  res;
}

int8_t bst_insert_new(struct bst** self_p, void* const restrict key)
{
    struct bst* self = *self_p;
    if (self->data_cursor >= self->data_amount) {
        self->data_amount *= 2;
        sub_bst_resize(&self->search_tree[0],self->data_amount);
        sub_bst_resize(&self->search_tree[1],self->data_amount);
        *self_p= realloc(self, sizeof(*self) + self->data_amount * self->data_size);
        self = *self_p;
    }
    
    void* const restrict data = get_data_from_index(self, self->data_cursor);
    memcpy(data, key, self->data_size);

    if (sub_bst_insert(self,self->search_tree[0], self->data_cursor) < 0){
        goto failed_insert_timeline;
    }

    if (sub_bst_insert(self,self->search_tree[1], self->data_cursor) < 0) {
        goto failed_insert_id;
    }

    self->data_cursor++;

    return 0;

failed_insert_timeline:
    sub_bst_delete(self,self->search_tree[0], key);
failed_insert_id:
    return -1;
}

int8_t bst_update_existing(struct bst* self, const void* const restrict key, const uint8_t filter_index)
{
    const array_index data_index = sub_bst_search(self, self->search_tree[!filter_index], key);
    struct sub_search* tree_to_update = self->search_tree[filter_index];
    void* tree_data = NULL;

    tree_data = get_data_from_index(self, data_index);

    sub_bst_delete(self, tree_to_update, tree_data);

    tree_to_update->update_fun(tree_data);

    return sub_bst_insert(self, tree_to_update, data_index);
}

int8_t bst_delete(struct bst* self, const void* const restrict key, uint8_t filter_index)
{
    struct sub_search* tree = self->search_tree[filter_index];
    return sub_bst_delete(self,tree, key);
}

const void* bst_search(struct bst* const restrict self, const void* const restrict key, 
        const uint8_t filter_index)
{
    struct sub_search* tree = self->search_tree[filter_index];
    const array_index data_index = sub_bst_search(self, tree, key);
    return get_data_from_index(self, data_index);
}

const void* bst_min(struct bst* self, const uint8_t filter_index)
{
    struct sub_search* tree = self->search_tree[filter_index];
    const array_index index = sub_bst_min(tree, tree->root);
    if (index >= 0) {
        return self->data + (index * self->data_size);
    }
    return NULL;
}

int8_t bst_free(struct bst* self)
{
    sub_bst_free(&self->search_tree[0]);
    sub_bst_free(&self->search_tree[1]);
    free(self);

    return 0;
}

