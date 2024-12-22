#include "log_pool.h"
#include <stdint.h>
#include <stddef.h>

static struct{
    struct LogEntry logs[LOG_MAX_SIZE];
    uint8_t next_position;
}log_pool;

int8_t log_pool_init(void)
{
    return 0;
}

int8_t log_pool_push(
        void* const restrict var,
        const uint8_t var_size,
        const char* const restrict name)
{
    int8_t err=0;
    struct LogEntry* entry_ptr = NULL;

    if (log_pool.next_position >= LOG_MAX_SIZE) {
        goto log_pool_full;
    }
    entry_ptr = &log_pool.logs[log_pool.next_position];
    ++log_pool.next_position;

    *entry_ptr->name = *name;
    entry_ptr->value = var;
    entry_ptr->var_size = var_size;
    return 0;

log_pool_full:
    err--;

    return err;
}

int8_t log_pool_link_entry_in_buffer(const uint16_t position, 
        struct LogEntry** const restrict buffer)
{
    if (buffer[position]) {
        return -1;
    }

    buffer[position] = &log_pool.logs[position];
    return 0;
}
