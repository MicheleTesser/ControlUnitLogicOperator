#include "telemetry.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "../log_obj_types.h"
#include "json_builder/json_builder.h"



struct TelemetryEntry{
  const void* p_var;
  const char* p_name;
  enum DATA_MODE var_type;
  DataPosition json_cursor;
};

struct BstPos{
  uint16_t entry_pos;
  struct BstPos* parent;
  struct BstPos* r_child;
  struct BstPos* l_child;
};

struct LogTelemetry_t{
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
char __assert_align_telemetry[(_Alignof(LogTelemetry_h) == _Alignof(struct LogTelemetry_t))? 1:-1];
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

static const struct TelemetryEntry* pop_node(struct BstPos* root, const struct TelemetryEntry* const entry_pool)__attribute__((__unused__));
static const struct TelemetryEntry* pop_node(struct BstPos* root, const struct TelemetryEntry* const entry_pool)
{
  struct BstPos* temp_node = root;
  struct BstPos* min_node= NULL;
  struct BstPos* min_parent= NULL;
  const struct TelemetryEntry* res=0;

  while (temp_node)
  {
    min_parent = min_node;
    min_node = temp_node;
    temp_node = temp_node->l_child;
  }

  if (min_node->r_child && min_parent)
  {
    min_parent->l_child = min_node->r_child;
  }


  res = &entry_pool[min_node->entry_pos];
  free(min_node);


  return res;
}

static int8_t add_node(struct BstPos** root, const struct TelemetryEntry* const entry_pool,
    struct TelemetryEntry* p_key)
{
  struct BstPos* cursor = *root;
  struct BstPos* parent= NULL;
  const uint16_t key = p_key->json_cursor;
  const uint16_t entry_pos = p_key - entry_pool;
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

  cursor->parent = parent;
  cursor->entry_pos = entry_pos;

  return 0;
}

static int8_t _pre_order_execute(struct BstPos* root, const struct TelemetryEntry* const entry_pool,
    Json_h* json)
{
  struct BstPos* cursor = root;
  const struct TelemetryEntry* p_entry = NULL;
  float var_value = 0.0f;
  uint8_t children_done = 0;
  int8_t err =0;

  while (cursor)
  {
    if (!children_done && cursor->l_child)
    {
      children_done =0;
      cursor = cursor->l_child;
      continue;
    }
    p_entry = &entry_pool[cursor->entry_pos];
    switch (p_entry->var_type)
    {
      case __u8__:
        var_value = *(uint8_t *) p_entry->p_var;
        break;
      case __u16__:
        var_value = *(uint16_t *) p_entry->p_var;
        break;
      case __u32__:
        var_value = *(uint32_t *) p_entry->p_var;
        break;
      case __i8__:
        var_value = *(int8_t *) p_entry->p_var;
        break;
      case __i16__:
        var_value = *(int16_t *) p_entry->p_var;
        break;
      case __i32__:
        var_value = *(int32_t *) p_entry->p_var;
        break;
      case __float__:
        var_value = *(float*) p_entry->p_var;
        break;
      default:
      }
    if((err = json_push_element(json, p_entry->p_name, var_value))<0)
    {
      return err;
    }
    if (!children_done && cursor->r_child)
    {
      cursor = cursor->r_child;
      children_done =0;
      continue;
    }

    cursor = cursor->parent;
    children_done = 1;
  }

  return 0;
}

//public

int8_t log_telemetry_init(LogTelemetry_h* const restrict self )
{
  union LogTelemetry_h_t_conv conv = {self};
  struct LogTelemetry_t* const restrict p_self = conv.clear;

  memset(p_self, 0, sizeof(*p_self));

  p_self->vars = calloc(1, sizeof(*p_self->vars));
  p_self->cap_entry=1;
  p_self->num_entry=0;
  p_self->root_pos = NULL;

  return 0;
}

int8_t log_telemetry_add_entry(LogTelemetry_h* const restrict self ,
    const char* name,
    const void* const var, const enum DATA_MODE data_type, 
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
  entry->json_cursor = position;

  return add_node(&p_self->root_pos, p_self->vars, entry);
}

int8_t log_telemetry_send(LogTelemetry_h* const restrict self)
{
  union LogTelemetry_h_t_conv conv = {self};
  struct LogTelemetry_t* const restrict p_self __attribute__((__unused__))= conv.clear;
  Json_h json = {0};
  int8_t err=0;

  if ((err = json_init(&json))<0)
  {
    return err; 
  }

  err = _pre_order_execute(p_self->root_pos, p_self->vars, &json);
  json_destroy(&json);

  return err;
}


int8_t log_telemetry_destroy(LogTelemetry_h* const restrict self)
{
  union LogTelemetry_h_t_conv conv = {self};
  struct LogTelemetry_t* const restrict p_self = conv.clear;

  free(p_self->vars);
  free_tree(p_self->root_pos);

  memset(self, 0, sizeof(*self));

  return 0;
}
