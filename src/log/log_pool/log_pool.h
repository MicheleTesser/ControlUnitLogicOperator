#ifndef __LOG_POOL__
#define __LOG_POOL__

#include <stdint.h>
struct LogEntry{
    char* name;
    void* value;
    uint8_t var_size;
};

#define LOG_MAX_SIZE 10

int8_t log_pool_init(void);
int8_t log_pool_push(
        void* const restrict var,
        const uint8_t var_size,
        const char* const restrict name);
int8_t log_pool_link_entry_in_buffer(const uint16_t position, struct LogEntry** const restrict buffer);

#endif // !__LOG_POOL__
