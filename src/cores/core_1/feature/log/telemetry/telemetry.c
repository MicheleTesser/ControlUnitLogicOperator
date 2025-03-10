#include "telemetry.h"
#include "../log_obj_types.h"
#include "json_builder/json_builder.h"
#include "../../../../../lib/raceup_board/components/ethernet.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

const char* JSON_1[] =
{
  "stest",
  "timestamp",
  "lap",
  "type",
  "bms_lv0",
  "bms_lv1",
  "bms_lv2",
  "bms_lv3",
  "bms_lv4",
  "bms_lv5",
  "bms_lv6",
  "bms_lv7",
  "amk_status_fl",
  "amk_actual_velocity_fl",
  "amk_torque_current_fl",
  "amk_voltage_fl",
  "amk_current_fl",
  "amk_status_fr",
  "amk_actual_velocity_fr",
  "amk_torque_current_fr",
  "amk_voltage_fr",
  "amk_current_fr",
  "amk_status_rr",
  "amk_actual_velocity_rr",
  "amk_torque_current_rr",
  "amk_voltage_rr",
  "amk_current_rr",
  "amk_status_rl",
  "amk_actual_velocity_rl",
  "amk_torque_current_rl",
  "amk_voltage_rl",
  "amk_current_rl",
  "amk_temp_motor_fl",
  "amk_temp_inverter_fl",
  "amk_temp_igbt_fl",
  "amk_error_info_fl",
};

const char* JSON_2[] =
{
  "stest",
  "timestamp",
  "lap",
  "type",
  "amk_temp_motor_fr",
  "amk_temp_inverter_fr",
  "amk_temp_igbt_fr",
  "amk_error_info_fr",
  "amk_temp_motor_rr",
  "amk_temp_inverter_rr",
  "amk_temp_igbt_rr",
  "amk_error_info_rr",
  "amk_temp_motor_rl",
  "amk_temp_inverter_rl",
  "amk_temp_igbt_rl",
  "amk_error_info_rl",
  "amk_torque_limit_positive_fl",
  "amk_torque_limit_negative_fl",
  "amk_torque_limit_positive_fr",
  "amk_torque_limit_negative_fr",
  "amk_torque_limit_positive_rr",
  "amk_torque_limit_negative_rr",
  "amk_torque_limit_positive_rl",
  "amk_torque_limit_negative_rl",
  "throttle",
  "steering_angle",
  "brake",
  "brake_press_front",
  "brake_press_rear",
  "actual_velocity_kmh",
  "car_status",
};

const char* JSON_3[] =
{
  "stest",
  "timestamp",
  "lap",
  "type",
  "acc_pot",
  "brk_pot",
  "brk_req",
  "thr_req",
  "max_hv_volt",
  "min_hv_volt",
  "avg_hv_volt",
  "max_hv_temp",
  "min_hv_temp",
  "avg_hv_temp",
  "max_temp_n_slave",
  "bms_error_map",
  "lem_current",
  "car_voltage",
  "current_sens",
  "total_power",
  "fan_speed",
  "acceleration_x",
  "acceleration_y",
  "acceleration_z",
  "omega_x",
  "omega_y",
  "omega_z",
  "motor_post_rl",
  "motor_pre_rl",
  "motor_pre_rr",
  "cplate_pre_right",
  "cplate_pre_left",
  "motor_post_fr",
  "motor_post_rr",
  "cplate_post_left",
  "suspensions_rl",
  "suspensions_fl",
  "suspensions_fr",
  "suspensions_rr",
  "gpio_bms",
  "gpio_imd",
  "velocity",
  "latitude",
  "longitude",
};

const char **JSON_ARRAYS[] = {JSON_1, JSON_2, JSON_3};
const uint32_t JSON_ARRAY_SIZES[] = 
{
  sizeof(JSON_1) / sizeof(JSON_1[0]),
  sizeof(JSON_2) / sizeof(JSON_2[0]),
  sizeof(JSON_3) / sizeof(JSON_3[0]),
};
#define MACRO_JSON_ARRAY_COUNT (sizeof(JSON_ARRAYS) / sizeof(JSON_ARRAYS[0]))
const uint8_t JSON_ARRAY_COUNT = MACRO_JSON_ARRAY_COUNT;

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
  struct LogPage{
    uint8_t num_entry;
    uint8_t cap_entry;
    struct TelemetryEntry* vars;
    struct BstPos* root_pos;
  }m_log_pages[MACRO_JSON_ARRAY_COUNT];
  EthernetNodeIpv4_t* p_ethernet_udp_telemetry;
};

union LogTelemetry_h_t_conv{
  LogTelemetry_h* const restrict hidden;
  struct LogTelemetry_t* const restrict clear;
};


#ifdef DEBUG
char __assert_size_telemetry[(sizeof(LogTelemetry_h) == sizeof(struct LogTelemetry_t))? 1:-1];
char __assert_align_telemetry[(_Alignof(LogTelemetry_h) == _Alignof(struct LogTelemetry_t))? 1:-1];
#endif // DEBUG

#define FOR_EACH_JSON(p_json) for(const char*** p_json = &JSON_ARRAYS[0];*p_json;p_json++)

#define FOR_EACH_JSON_FIELD(p_field, p_json)\
  for(const char* p_field=*p_json[0];*p_field;++p_field)

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

typedef struct
{
  uint8_t json_num;
  uint8_t json_line;
}LogVarPosition;

static int8_t _search_json_for_log_var(const char* var_name, LogVarPosition* o_log_position)
{
  for (uint8_t json_num=0;json_num<JSON_ARRAY_COUNT;++json_num)
  {
    for(uint32_t json_line=0;json_line<JSON_ARRAY_SIZES[json_num];++json_line)
    {
      if (!strcmp(JSON_ARRAYS[json_num][json_line], var_name))
      {
        o_log_position->json_num = json_num;
        o_log_position->json_line = json_line;

        return 0;
      }

    }
  }

  return -1;
}

//public

int8_t log_telemetry_init(LogTelemetry_h* const restrict self )
{
  union LogTelemetry_h_t_conv conv = {self};
  struct LogTelemetry_t* const restrict p_self = conv.clear;

  memset(p_self, 0, sizeof(*p_self));

  //addr: 204.216.214.158
  //port: 8086
  //INFO: telemetry infos: https://docs.google.com/document/d/1T5-u_UYU2VZMfcQa-BXe2rGkpZzWMHQcCuYubQNwaYI/edit?tab=t.0
  IpAddrIpV4Port addr =
  {
    .addr = (204 << 3) | (216 << 2) | (214 << 1) | (158 << 0),
    .port = 8086,
  };
  p_self->p_ethernet_udp_telemetry = hardware_ethernet_udp_init(&addr);

  if (!p_self->p_ethernet_udp_telemetry)
  {
    return -1;
  }

  for (uint8_t i=0; i<JSON_ARRAY_COUNT; i++)
  {
    struct LogPage* log_page = &p_self->m_log_pages[i];
    log_page->vars = calloc(1, sizeof(*log_page->vars));
    if (!log_page->vars)
    {
      hardware_ethernet_udp_free(&p_self->p_ethernet_udp_telemetry);
      for (uint8_t j=0; j<i; j++)
      {
        free(p_self->m_log_pages[i].vars);
      }
      return -99;
    }
    log_page->cap_entry=1;
    log_page->num_entry=0;
    log_page->root_pos = NULL;
  
  }

  return 0;
}

int8_t log_telemetry_add_entry(LogTelemetry_h* const restrict self ,
    const char* name,
    const void* const var, const enum DATA_MODE data_type)
{
  union LogTelemetry_h_t_conv conv = {self};
  struct LogTelemetry_t* const p_self = conv.clear;
  struct TelemetryEntry* entry=NULL;
  struct LogPage* p_log_page = NULL;
  LogVarPosition log_pos = {0};

  if(_search_json_for_log_var(name, &log_pos)<0)
  {
    return -1;
  }

  p_log_page = &p_self->m_log_pages[log_pos.json_num];
  if (p_log_page->num_entry >= p_log_page->cap_entry)
  {
    p_log_page->cap_entry*=2;
    p_log_page->vars = realloc(p_log_page->vars, p_log_page->cap_entry * sizeof(*p_log_page->vars));
  }

  entry = &p_log_page->vars[p_log_page->num_entry++];
  entry->p_var = var;
  entry->var_type = data_type;
  entry->p_name = name;
  entry->json_cursor = log_pos.json_line;

  if(add_node(&p_log_page->root_pos, p_log_page->vars, entry)<0)
  {
    --p_log_page->num_entry;
    return -1;
  }

  return 0;
}

int8_t log_telemetry_send(LogTelemetry_h* const restrict self)
{
  union LogTelemetry_h_t_conv conv = {self};
  struct LogTelemetry_t* const restrict p_self __attribute__((__unused__))= conv.clear;
  struct LogPage* p_log_page = NULL;
  Json_h json = {0};
  int8_t err=0;

  if ((err = json_init(&json))<0)
  {
    return err; 
  }

  for (uint8_t i=0; i<JSON_ARRAY_COUNT; i++)
  {
    p_log_page = &p_self->m_log_pages[i];
    err = _pre_order_execute(p_log_page->root_pos, p_log_page->vars, &json);
    json_destroy(&json);
    p_log_page = NULL;
  }



  return err;
}


int8_t log_telemetry_destroy(LogTelemetry_h* const restrict self)
{
  union LogTelemetry_h_t_conv conv = {self};
  struct LogTelemetry_t* const restrict p_self = conv.clear;
  struct LogPage* p_log_page = NULL;

  for (uint8_t i=0; i<JSON_ARRAY_COUNT; i++)
  {
    p_log_page = &p_self->m_log_pages[i];
    free(p_log_page->vars);
    free_tree(p_log_page->root_pos);
  }

  hardware_ethernet_udp_free(&p_self->p_ethernet_udp_telemetry);

  memset(self, 0, sizeof(*self));

  return 0;
}
