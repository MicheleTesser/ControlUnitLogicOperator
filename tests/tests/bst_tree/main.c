#include "score_lib/test_lib.h"
#include "src/board_can/can_freq_check/bst/bst.h"
#include <stdio.h>
#include <string.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <stdint.h>

struct BstNodeData{
    uint8_t key_1;
    uint8_t key_2;
};

static int8_t cmp_key_1(const void* tree_key, const void* other_key)
{
    const struct BstNodeData* key_1 = tree_key;
    const struct BstNodeData* key_2 = other_key;

    return memcmp(&key_2->key_1, &key_1->key_1, sizeof(key_1->key_1));
}

static int8_t cmp_key_2(const void* tree_key, const void* other_key)
{
    const struct BstNodeData* key_1 = tree_key;
    const struct BstNodeData* key_2 = other_key;

    return memcmp(&key_2->key_2, &key_1->key_2, sizeof(key_1->key_2));
}

static int8_t update_key_info(void* const restrict tree_key __attribute_maybe_unused__)
{
    struct BstNodeData* tree_info = tree_key;
    tree_info->key_1 += 5;
    return 0;
}

static int8_t update_key_freq(void* const restrict tree_key)
{
    struct BstNodeData* tree_info = tree_key;
    tree_info->key_2 += 10;

    return 0;
}

static int8_t insert_new_node(struct bst* self, const uint8_t key_1, const uint8_t key_2)
{
    struct BstNodeData temp_node = {
        .key_1 = key_1,
        .key_2 = key_2,
    };

    if (bst_insert_new(self, &temp_node)<0)
    {
        FAILED("failed insertion of node:");
        return -1;
    }
    else
    {
        PASSED("passed insertion of node:");
    }
    printf("(%d,%d)\n",key_1,key_2);

    return 0;
}

int main(void)
{
    struct bst* root = bst_new(cmp_key_1, cmp_key_2,
            update_key_info, update_key_freq,
            10, sizeof(struct BstNodeData));
    
    insert_new_node(root, 20, 11);
    insert_new_node(root, 13, 5);
    insert_new_node(root, 10, 15);
    insert_new_node(root, 5, 12);

    const struct BstNodeData* min_1 = bst_min(root, 0);
    if (min_1->key_1==5) {
        PASSED("extracted correct min after all the initial insertion with search alg 1: ");
    }else{
        FAILED("extracted wrong min after all the initial insertion with search alg 1: ");
    }
    printf("(%d,%d)\n",min_1->key_1 ,min_1->key_2);

    const struct BstNodeData* min_2 = bst_min(root, 1);
    if (min_2->key_2==5) {
        PASSED("extracted correct min after all the initial insertion with search alg 2: ");
    }else{
        FAILED("extracted wrong min after all the initial insertion with search alg 2: ");
    }
    printf("(%d,%d)\n",min_2->key_1 ,min_2->key_2);

    if(bst_delete(root, min_1, 0) < 0){
        FAILED("delete of min with search alg 1 failed: ");
    }else{
        PASSED("delete of min with search alg 1 passed: ");
    }
    printf("(%d,%d)\n",min_1->key_1 ,min_1->key_2);


    const struct BstNodeData* new_min_1 = bst_min(root, 0);
    if (new_min_1->key_1==10) {
        PASSED("extracted correct min after first delete with search alg 1: ");
    }else{
        FAILED("extracted wrong min after first delete with search alg 1: ");
    }
    printf("(%d,%d)\n",new_min_1->key_1 ,new_min_1->key_2);

    if(bst_delete(root, min_2, 1) < 0){
        FAILED("delete of min with search alg 2 failed: ");
    }else{
        PASSED("delete of min with search alg 2 passed: ");
    }
    printf("(%d,%d)\n",min_2->key_1 ,min_2->key_2);
    

    const struct BstNodeData* new_min_2 = bst_min(root, 1);
    if (new_min_2->key_2==11) {
        PASSED("extracted correct min after first delete with search alg 2: ");
    }else{
        FAILED("extracted wrong min after first delete with search alg 2: ");
    }
    printf("(%d,%d)\n",new_min_2->key_1 ,new_min_2->key_2);

    bst_free(root);
    print_SCORE();
}
