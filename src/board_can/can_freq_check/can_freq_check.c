#include "can_freq_check.h"
#include "bst/bst.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>


//private

struct MexInfo{
    uint16_t can_id;
    time_var_microseconds freq;
    time_var_microseconds timestamp;
    fault_manager fault_f;
};

struct FreqNode{
    time_var_microseconds time_id;
    struct MexInfo* metadata;
};


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

int8_t can_freq_add_mex_freq(const uint16_t can_id, const time_var_microseconds freq, const fault_manager fault_fun)
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
    
    const uint8_t init_value = 0;
    bst_push(FREQ_TREE.freq_tree, &init_value, sizeof(init_value), info);

    return 0;
}

int8_t can_freq_update_freq(const uint16_t can_id, const time_var_microseconds now)
{
    struct MexInfo* info = NULL;
    for (uint16_t i=0; i<FREQ_TREE.info_pool_size; i++) {
        if (FREQ_TREE.info_pool[i].can_id == can_id) {
            info = &FREQ_TREE.info_pool[i];
            break;
        }
    }
    if (!info) {
        return -1;
    }

    bst_pop(FREQ_TREE.freq_tree, &info->timestamp, sizeof(info->timestamp));
    bst_push(FREQ_TREE.freq_tree, &now , sizeof(now), info);
    info->timestamp = now;

    return 0;
}

int8_t can_freq_check_faults(void)
{
    struct MexInfo metadata;
    const time_var_microseconds* min = bst_min(FREQ_TREE.freq_tree, &metadata);
    if (min && (timer_time_now() - *min) > metadata.freq) {
        bst_pop(FREQ_TREE.freq_tree, &metadata.timestamp, sizeof(metadata.timestamp));
        metadata.fault_f();
        return metadata.can_id;
    }
    return 0;
}
