#ifndef __JSON_BUILDER__
#define __JSON_BUILDER__

#include <stdint.h>

static uint8_t VAR_NAME_MAX_LENGTH __attribute__((__unused__)) = 16;

typedef float JsonVarValue;

typedef struct __attribute__((aligned(8))){
  const uint8_t private_data[16];
}Json_h;

int8_t
json_init(Json_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t
json_push_element(Json_h* const restrict self, 
    const char* const restrict var_name, const uint8_t var_name_length,
    const JsonVarValue value)__attribute__((__nonnull__(1)));

const char*
json_get(const Json_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t
json_destroy(Json_h* self)__attribute__((__nonnull__(1)));

#endif // !__JSON_BUILDER__
