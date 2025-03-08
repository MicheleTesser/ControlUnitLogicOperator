#include "shared_memory.h"

#include <stdint.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
  const void* p_data;
  const SharedDataId data_id;
}SharedDataInstance;

static struct{
  atomic_flag lock;
  SharedDataInstance* memory;
  uint32_t memory_length;
  uint32_t memory_capacity;
}SHARED_MEMORY = {0};

//private

#define MUTEX_LOCK_ACTION()\
  while(atomic_flag_test_and_set(&SHARED_MEMORY.lock));\
  for (uint8_t b=0;!b;(b=1,atomic_flag_clear(&SHARED_MEMORY.lock)))\

#define MUTEXT_ACTION_EXIT(ret_val) atomic_flag_clear(&SHARED_MEMORY.lock); return ret_val;

//public

int8_t shared_memory_init(const uint32_t initial_capacity)
{
  if (!initial_capacity)
  {
    return -1;
  }

  if (SHARED_MEMORY.memory_capacity)
  {
    return 1;
  }

  SHARED_MEMORY.memory_capacity = initial_capacity;
  SHARED_MEMORY.memory = calloc(SHARED_MEMORY.memory_capacity, sizeof(*SHARED_MEMORY.memory));
  SHARED_MEMORY.memory_length = 0;

  return 0;
}

int8_t shared_memory_store_pointer(const void* p_data, const SharedDataId data_id)
{
  while (atomic_flag_test_and_set(&SHARED_MEMORY.lock)){}

  SharedDataInstance instance = 
  {
    .data_id = data_id,
    .p_data = p_data,
  };

  if (SHARED_MEMORY.memory_length >= SHARED_MEMORY.memory_capacity)
  {
    SHARED_MEMORY.memory_capacity *= 2;
    SHARED_MEMORY.memory = realloc(SHARED_MEMORY.memory,
        sizeof(*SHARED_MEMORY.memory) * SHARED_MEMORY.memory_capacity);
    if (!SHARED_MEMORY.memory)
    {
      MUTEXT_ACTION_EXIT(-99);
    }
  }

  uint8_t i=0;
  for (i=0; i<SHARED_MEMORY.memory_length; i++)
  {
    if(SHARED_MEMORY.memory[i].data_id == data_id)
    {
      break;
    }
  }

  if (i==SHARED_MEMORY.memory_length)
  {
    memcpy(&SHARED_MEMORY.memory[i], &instance, sizeof(instance));
    ++SHARED_MEMORY.memory_length;
    MUTEXT_ACTION_EXIT(0);
  }
  MUTEXT_ACTION_EXIT(-1);
}

const void* shared_memory_fetch_pointer(const SharedDataId data_id)
{
  const void* res = NULL;
  MUTEX_LOCK_ACTION()
  {
    for (uint8_t i=0; i<SHARED_MEMORY.memory_length; i++)
    {
      SharedDataInstance *p_ins = &SHARED_MEMORY.memory[i];
      if (p_ins->data_id == data_id)
      {
        res = p_ins->p_data;
        break;
      }
    }
  }

  return res;
}
