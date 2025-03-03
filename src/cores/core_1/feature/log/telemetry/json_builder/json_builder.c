#include "json_builder.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Json_t{
  char* p_str_json;
  uint16_t str_json_size;
  uint32_t json_cursor_offset;
};

union Json_h_t_conv {
  Json_h* const hidden;
  struct Json_t* const clear;
};

union Json_h_t_conv_const {
  const Json_h* const hidden;
  const struct Json_t* const clear;
};

#ifdef DEBUG
char __assert_size_json[(sizeof(Json_h)==sizeof(struct Json_t))?1:-1];
char __assert_size_json[(_Alignof(Json_h)==_Alignof(struct Json_t))?1:-1];
#endif /* ifdef DEBUG */

//private

//public

int8_t json_init(Json_h* const restrict self)
{
  union Json_h_t_conv conv = {self};
  struct Json_t* const p_self = conv.clear;

  const char initial_json[] = "{ }";

  memset(p_self, 0, sizeof(*p_self));

  p_self->str_json_size = sizeof(initial_json);
  p_self->p_str_json = calloc(1,p_self->str_json_size);
  p_self->json_cursor_offset = 1;
  memcpy(p_self->p_str_json, initial_json, p_self->str_json_size);

  return 0;
}

int8_t json_push_element(Json_h* const restrict self,
    const char* const restrict var_name,
    const JsonVarValue value)
{
  union Json_h_t_conv conv = {self};
  struct Json_t* const p_self = conv.clear;
  char new_json_field[128] = {'\0'};
  uint8_t new_json_cursor = 0;
  uint16_t num_byte_value_var __attribute__((__unused__))=0;
  uint32_t var_name_length = strlen(var_name);

  if (p_self->json_cursor_offset > 1)
  {
    new_json_field[new_json_cursor] = ',';
    new_json_cursor+=1;
  }

  new_json_field[new_json_cursor] = '"';
  new_json_cursor+=1;

  memcpy(&new_json_field[new_json_cursor], var_name, var_name_length);
  new_json_cursor+=var_name_length;


  new_json_field[new_json_cursor] = '"';
  new_json_cursor+=1;

  new_json_field[new_json_cursor] = ':';
  new_json_cursor+=1;

  num_byte_value_var = sprintf(&new_json_field[new_json_cursor], "%0.2f", value);
  new_json_cursor+=num_byte_value_var;

  new_json_field[new_json_cursor] = '}';
  new_json_cursor+=1;

  p_self->str_json_size += new_json_cursor;
  p_self->p_str_json = realloc(p_self->p_str_json, p_self->str_json_size);
  if (!p_self->p_str_json)
  {
    return -99;
  }

  memcpy(&p_self->p_str_json[p_self->json_cursor_offset], new_json_field, new_json_cursor);
  p_self->json_cursor_offset += new_json_cursor;

  p_self->p_str_json[p_self->json_cursor_offset] = '\0';
  p_self->json_cursor_offset -= 1;
  

  return 0;
}

const char*
json_get(const Json_h* const restrict self)
{
  const union Json_h_t_conv_const conv = {self};
  const struct Json_t* const p_self = conv.clear;

  return p_self->p_str_json;
}

int8_t json_destroy(Json_h* self)
{
  union Json_h_t_conv conv = {self};
  struct Json_t* const p_self = conv.clear;

  free(p_self->p_str_json);
  memset(p_self, 0, sizeof(*p_self));

  return 0;
}
