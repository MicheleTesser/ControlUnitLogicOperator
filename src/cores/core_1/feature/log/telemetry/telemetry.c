#include "telemetry.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../log_obj_types.h"


struct TelemetryEntry{
  const void* p_var;
  const char* p_name;
  enum DATA_MODE var_type;
  DataPosition json_cursor;
  uint32_t data_rage;
};

struct BstPos{
  uint16_t entry_pos;
  struct BstPos* r_child;
  struct BstPos* l_child;
};

struct Json{
  char* s_json;
  uint16_t json_size;
  uint32_t json_cursor_offset;
};

struct LogTelemetry_t{
  struct Json json;
  struct TelemetryEntry* vars;
  struct BstPos* root_pos;
  uint8_t num_entry;
  uint8_t cap_entry;
};

union LogTelemetry_h_t_conv{
  LogTelemetry_h* const restrict hidden;
  struct LogTelemetry_t* const restrict clear;
};


#ifdef DEBUG
char __assert_size_telemetry[(sizeof(LogTelemetry_h) == sizeof(struct LogTelemetry_t))? 1:-1];
#endif // DEBUG

static void free_tree(struct BstPos* root)
{
  if (!root) {
    return;
  }

  free_tree(root->r_child);
  free_tree(root->l_child);
  free(root);
}

static const struct TelemetryEntry* pop_node(struct BstPos* root, const struct TelemetryEntry* const entry_pool)
{
  struct BstPos* cursor = root;
  struct BstPos* parent= NULL;
  const struct TelemetryEntry* res=0;

  while (cursor)
  {
    parent = cursor;
    cursor = cursor->l_child;
  }

  if (cursor->r_child)
  {
    struct BstPos* temp_cursor = cursor->r_child;
    struct BstPos* temp_parent= NULL;

    while (temp_cursor)
    {
      temp_parent = temp_cursor;
      temp_cursor = temp_cursor->l_child;
    }
    temp_parent->l_child = cursor->l_child;
    parent->l_child = cursor->r_child;
  }
  else if (cursor->l_child)
  {
    parent->l_child = cursor->l_child;
  }

  res=&entry_pool[cursor->entry_pos];
  free(cursor);

  return res;
}

static int8_t
add_node(struct BstPos** root, const struct TelemetryEntry* const entry_pool,
    struct TelemetryEntry* p_key)
{
  struct BstPos* cursor = *root;
  struct BstPos* parent= NULL;
  const uint16_t key = p_key->json_cursor;
  const uint16_t entry_pos = p_key - entry_pool; //HACK: check if it's also true in different architectures
  DataPosition n_key = 0;

  if (!cursor)
  {
    *root = calloc(1, sizeof(**root));
    (*root)->entry_pos = entry_pos;
    return 0;
  }

  while (cursor)
  {
    n_key = entry_pool[cursor->entry_pos].json_cursor;
    parent = cursor;
    if (key > n_key)
    {
      cursor = cursor->r_child;
    }
    else if (key < n_key)
    {
      cursor = cursor->l_child;
    }
    else
    {
      return -1;
    }
  }

  if (key > n_key)
  {
    parent->r_child = calloc(1,sizeof(*parent->r_child));
    cursor=parent->r_child;
  }
  else
  {
    parent->l_child = calloc(1,sizeof(*parent->l_child));
    cursor=parent->l_child;
  }

  cursor->entry_pos = entry_pos;

  return 0;
}

static void json_init(struct Json* const restrict self)
{
  const char json_init[] = "{}";
  self->json_size = sizeof(json_init);
  self->s_json = malloc(self->json_size * sizeof(*self->s_json));
  memcpy(self->s_json, json_init, sizeof(json_init));
}

//public

int8_t
log_telemetry_init(LogTelemetry_h* const restrict self )
{
  union LogTelemetry_h_t_conv conv = {self};
  struct LogTelemetry_t* const restrict p_self = conv.clear;

  memset(p_self, 0, sizeof(*p_self));

  json_init(&p_self->json);

  p_self->json.json_cursor_offset = 1;
  p_self->vars = calloc(1, sizeof(*p_self->vars));
  p_self->cap_entry=1;
  p_self->num_entry=0;
  p_self->root_pos = NULL;

  return 0;
}

  int8_t
log_telemetry_add_entry(LogTelemetry_h* const restrict self ,
    const char* name, const void* const var,
    const enum DATA_MODE data_type, const DataRange data_range,
    const DataPosition position)
{
  union LogTelemetry_h_t_conv conv = {self};
  struct LogTelemetry_t* const restrict p_self = conv.clear;
  struct TelemetryEntry* entry=NULL;

  if (p_self->num_entry >= p_self->cap_entry)
  {
    p_self->cap_entry*=2;
    p_self->vars = realloc(p_self->vars, p_self->cap_entry * sizeof(*p_self->vars));
  }
  entry = &p_self->vars[p_self->num_entry++];
  entry->p_var = var;
  entry->var_type = data_type;
  entry->p_name = name;
  entry->data_rage = data_range;
  entry->json_cursor = position;

  return add_node(&p_self->root_pos, p_self->vars, entry);
}

  int8_t
log_telemetry_lock_json(LogTelemetry_h* const restrict self)
{
  union LogTelemetry_h_t_conv conv = {self};
  struct LogTelemetry_t* const restrict p_self = conv.clear;
  const struct TelemetryEntry* cursor = NULL;

  if (p_self->json.json_size>3)
  {
    free(p_self->json.s_json);
    json_init(&p_self->json);
  }

  cursor = pop_node(p_self->root_pos, p_self->vars);
  while (cursor)
  {
    //TODO: build json
  }

  return 0;
}

  int8_t
log_telemetry_destroy(LogTelemetry_h* const restrict self)
{
  union LogTelemetry_h_t_conv conv = {self};
  struct LogTelemetry_t* const restrict p_self = conv.clear;

  free(p_self->json.s_json);
  free(p_self->vars);
  free_tree(p_self->root_pos);

  memset(self, 0, sizeof(*self));

  return 0;
}
