#ifndef __JSON_BUILDER__
#define __JSON_BUILDER__

#include <stdint.h>

typedef float JsonVarValue;

typedef struct __attribute__((aligned(8))){
  const uint8_t private_data[16];
}Json_h;

int8_t
json_init(Json_h* const restrict self)__attribute__((__nonnull__(1)));

//INFO: var_name string MUST be null terminating
int8_t
json_push_element(Json_h* const restrict self, 
    const char* const restrict var_name,
    const JsonVarValue value)__attribute__((__nonnull__(1)));

const char*
json_get(const Json_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t
json_destroy(Json_h* self)__attribute__((__nonnull__(1)));

#endif // !__JSON_BUILDER__
