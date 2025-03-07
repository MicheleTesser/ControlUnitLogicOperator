#ifndef __SHARED_MEMORY__
#define __SHARED_MEMORY__

#include <stdint.h>

typedef uint32_t SharedDataId;

int8_t shared_memory_init(const uint32_t initial_capacity);
int8_t shared_memory_store_pointer(const void* p_data, const SharedDataId data_id);
const void* shared_memory_fetch_pointer(const SharedDataId data_id);

#endif // !__SHARED_MEMORY__
