#include "can_freq_check.h"
#include "bst/bst.h"
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

enum SEARCH_FILTERS {
    ID_FILTER = 0,
    DEADLINE_FILTER =1,
};

static struct bst* FREQ_TREE = NULL;

static int8_t cmp_key_info(const void* tree_key, const void* other_key)
{
    const struct MexInfo* tree_info = tree_key;
    const struct MexInfo* other_info = other_key;
    return memcmp(&tree_info->can_id, &other_info->can_id, sizeof(other_info->can_id));
}

static int8_t cmp_key_freq(const void* tree_key, const void* other_key)
{
    const struct MexInfo* tree_info = tree_key;
    const struct MexInfo* other_info = other_key;

    int8_t cmp_deadline = memcmp(&tree_info->deadline, &other_info->deadline, 
            sizeof(tree_info->deadline));

    if (!cmp_deadline) {
        return !(tree_info->can_id == other_info->can_id);
    }
    return  cmp_deadline;
}

static int8_t update_info_fun(void* const restrict tree_key, const void* const restrict other_key)
{
    struct MexInfo* node = tree_key;
    const struct MexInfo* const other = other_key;
    node->deadline = other->deadline;
    return 0;
}

//public

int8_t can_freq_class_init(const uint16_t amount_of_ids)
{
    if (FREQ_TREE) {
        return -1; //INFO: already init
    }

    FREQ_TREE = bst_new(cmp_key_info, cmp_key_freq, update_info_fun,
            amount_of_ids, sizeof(struct MexInfo));
    if (!FREQ_TREE) {
        return -2;
    }

    return 0;
}

int8_t can_freq_add_mex_freq(const uint16_t can_id, const time_var_microseconds freq, 
        const fault_manager fault_fun)
{
    struct MexInfo info = {
        .can_id = can_id,
        .fault_f = fault_fun,
        .freq = freq,
        .deadline = timer_time_now() + freq,
    };
    return !!bst_insert(FREQ_TREE , &info);
}

int8_t can_freq_update_freq(const uint16_t can_id)
{
    struct MexInfo info_s = {
        .can_id = can_id,
        .deadline = timer_time_now(),
    };
    bst_update(FREQ_TREE, &info_s, ID_FILTER);
    return 0;
}


int8_t can_freq_check_faults(void)
{
    const struct MexInfo* min = bst_min(FREQ_TREE,DEADLINE_FILTER);
    if (min && min->deadline < timer_time_now()) {
        bst_delete(FREQ_TREE, min, DEADLINE_FILTER);
        min->fault_f();
        return min->can_id;
    }
    return 0;
}
