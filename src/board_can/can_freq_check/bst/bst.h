#ifndef __CAN_FREQ_BST__
#define __CAN_FREQ_BST__

#include <stdint.h>

struct bst;

struct bst* bst_new(const uint16_t initial_size);
int8_t bst_push(struct bst* const restrict self, const void* key, const uint8_t key_size, 
        const void* const restrict other_data);
int8_t bst_pop(struct bst* const restrict self, const void* key, const uint8_t key_size);
const void* bst_min(struct bst* const restrict self, void* metadata);

#endif // !__CAN_FREQ_BST__
