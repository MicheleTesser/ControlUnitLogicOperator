#ifndef __GENERIC_BST__
#define __GENERIC_BST__

#include <stdint.h>

struct bst;

/*
 *  return:
 *      0  => same key
 *      -1 => other_key < tree_key
 *      1  => other_key > tree_key
 */
typedef int8_t key_compare(const void* tree_key, const void* other_key);
typedef int8_t key_update(void* const restrict tree_key, const void* const restrict other_key);

struct bst* bst_new(key_compare cmp_fun_1, key_compare cmp_fun_2, key_update update_fun,
        const uint8_t initial_reserved_size, 
        const uint16_t ele_size);
int8_t bst_insert(struct bst* self, void* const restrict key);
int8_t bst_delete(struct bst* self, const void* const restrict key, uint8_t which_sub_tree);
uint8_t bst_update(struct bst* const restrict self, const void* const restrict key, 
        const uint8_t filter_index);
const void* bst_min(struct bst* self, const uint8_t filter_index);
int8_t bst_free(struct bst* self);

#endif // !__GENERIC_BST__
