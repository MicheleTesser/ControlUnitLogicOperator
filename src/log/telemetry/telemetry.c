#include "telemetry.h"
#include "../log_pool/log_pool.h"

static struct LogEntry* logs_ptr[LOG_MAX_SIZE];

int8_t telemetry_init(void)
{
    return 0;
}

int8_t telemetry_add_entry(const uint16_t position)
{
    return log_pool_link_entry_in_buffer(position, logs_ptr);
}

int8_t telemetry_update_and_send(void)
{
    return 0;
}
