#include "external_log_variables.h"

#include <stdint.h>
#include <stdatomic.h>
#include <stddef.h>

static struct{
  atomic_flag lock;
  const void* memory[__NUM_OF_SHARED_ID__];
}SHARED_MEMORY = {0};

//private

#define MUTEX_LOCK_ACTION()\
  while(atomic_flag_test_and_set(&SHARED_MEMORY.lock));\
  for (uint8_t b=0;!b;(b=1,atomic_flag_clear(&SHARED_MEMORY.lock)))\

//public

int8_t external_log_variables_store_pointer(const void* p_data, const SharedDataId data_id)
{
  if (data_id >= __NUM_OF_SHARED_ID__)
  {
    return -1;
  }

  MUTEX_LOCK_ACTION()
  {
    SHARED_MEMORY.memory[data_id] =p_data;
  }
  return 0;
}

int8_t external_log_variables_add_to_log(Log_h* const restrict p_log __attribute__((__unused__)))
{
  return 0;
}

const void* external_log_extract_data_ptr_r_only(const SharedDataId data_id)
{
  const void* res = NULL;
  if (data_id >= __NUM_OF_SHARED_ID__)
  {
    return NULL;
  }

  MUTEX_LOCK_ACTION()
  {
    res = &SHARED_MEMORY.memory[data_id];
  }
  return res;
}
