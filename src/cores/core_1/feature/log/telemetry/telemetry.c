#include "telemetry.h"
#include "../log_obj_types.h"
#include "json_builder/json_builder.h"
#include "../../../../../lib/raceup_board/components/ethernet.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct
{
  enum DATA_MODE m_data_type;
  const void* p_data;
  const char* const m_name;
  const char* data_format;
}JsonCell;

#define NEW_EMPTY_CELL(name) \
  { .m_data_type = __u8__, .p_data = NULL, .m_name =name, .data_format = ""},

static JsonCell JSON_1[] =
{
  NEW_EMPTY_CELL("stest")
  NEW_EMPTY_CELL("timestamp")
  NEW_EMPTY_CELL("lap")
  NEW_EMPTY_CELL("type")
  NEW_EMPTY_CELL("bms_lv0")
  NEW_EMPTY_CELL("bms_lv1")
  NEW_EMPTY_CELL("bms_lv2")
  NEW_EMPTY_CELL("bms_lv3")
  NEW_EMPTY_CELL("bms_lv4")
  NEW_EMPTY_CELL("bms_lv5")
  NEW_EMPTY_CELL("bms_lv6")
  NEW_EMPTY_CELL("bms_lv7")
  NEW_EMPTY_CELL("amk_status_fl")
  NEW_EMPTY_CELL("amk_actual_velocity_fl")
  NEW_EMPTY_CELL("amk_torque_current_fl")
  NEW_EMPTY_CELL("amk_voltage_fl")
  NEW_EMPTY_CELL("amk_current_fl")
  NEW_EMPTY_CELL("amk_status_fr")
  NEW_EMPTY_CELL("amk_actual_velocity_fr")
  NEW_EMPTY_CELL("amk_torque_current_fr")
  NEW_EMPTY_CELL("amk_voltage_fr")
  NEW_EMPTY_CELL("amk_current_fr")
  NEW_EMPTY_CELL("amk_status_rr")
  NEW_EMPTY_CELL("amk_actual_velocity_rr")
  NEW_EMPTY_CELL("amk_torque_current_rr")
  NEW_EMPTY_CELL("amk_voltage_rr")
  NEW_EMPTY_CELL("amk_current_rr")
  NEW_EMPTY_CELL("amk_status_rl")
  NEW_EMPTY_CELL("amk_actual_velocity_rl")
  NEW_EMPTY_CELL("amk_torque_current_rl")
  NEW_EMPTY_CELL("amk_voltage_rl")
  NEW_EMPTY_CELL("amk_current_rl")
  NEW_EMPTY_CELL("amk_temp_motor_fl")
  NEW_EMPTY_CELL("amk_temp_inverter_fl")
  NEW_EMPTY_CELL("amk_temp_igbt_fl")
  NEW_EMPTY_CELL("amk_error_info_fl")
};

static JsonCell JSON_2[] =
{
  NEW_EMPTY_CELL("stest")
  NEW_EMPTY_CELL("timestamp")
  NEW_EMPTY_CELL("lap")
  NEW_EMPTY_CELL("type")
  NEW_EMPTY_CELL("amk_temp_motor_fr")
  NEW_EMPTY_CELL("amk_temp_inverter_fr")
  NEW_EMPTY_CELL("amk_temp_igbt_fr")
  NEW_EMPTY_CELL("amk_error_info_fr")
  NEW_EMPTY_CELL("amk_temp_motor_rr")
  NEW_EMPTY_CELL("amk_temp_inverter_rr")
  NEW_EMPTY_CELL("amk_temp_igbt_rr")
  NEW_EMPTY_CELL("amk_error_info_rr")
  NEW_EMPTY_CELL("amk_temp_motor_rl")
  NEW_EMPTY_CELL("amk_temp_inverter_rl")
  NEW_EMPTY_CELL("amk_temp_igbt_rl")
  NEW_EMPTY_CELL("amk_error_info_rl")
  NEW_EMPTY_CELL("amk_torque_limit_positive_fl")
  NEW_EMPTY_CELL("amk_torque_limit_negative_fl")
  NEW_EMPTY_CELL("amk_torque_limit_positive_fr")
  NEW_EMPTY_CELL("amk_torque_limit_negative_fr")
  NEW_EMPTY_CELL("amk_torque_limit_positive_rr")
  NEW_EMPTY_CELL("amk_torque_limit_negative_rr")
  NEW_EMPTY_CELL("amk_torque_limit_positive_rl")
  NEW_EMPTY_CELL("amk_torque_limit_negative_rl")
  NEW_EMPTY_CELL("throttle")
  NEW_EMPTY_CELL("steering_angle")
  NEW_EMPTY_CELL("brake")
  NEW_EMPTY_CELL("brake_press_front")
  NEW_EMPTY_CELL("brake_press_rear")
  NEW_EMPTY_CELL("actual_velocity_kmh")
  NEW_EMPTY_CELL("car_status")
};

static JsonCell JSON_3[] =
{
  NEW_EMPTY_CELL("stest")
  NEW_EMPTY_CELL("timestamp")
  NEW_EMPTY_CELL("lap")
  NEW_EMPTY_CELL("type")
  NEW_EMPTY_CELL("acc_pot")
  NEW_EMPTY_CELL("brk_pot")
  NEW_EMPTY_CELL("brk_req")
  NEW_EMPTY_CELL("thr_req")
  NEW_EMPTY_CELL("max_hv_volt")
  NEW_EMPTY_CELL("min_hv_volt")
  NEW_EMPTY_CELL("avg_hv_volt")
  NEW_EMPTY_CELL("max_hv_temp")
  NEW_EMPTY_CELL("min_hv_temp")
  NEW_EMPTY_CELL("avg_hv_temp")
  NEW_EMPTY_CELL("max_temp_n_slave")
  NEW_EMPTY_CELL("bms_error_map")
  NEW_EMPTY_CELL("lem_current")
  NEW_EMPTY_CELL("car_voltage")
  NEW_EMPTY_CELL("current_sens")
  NEW_EMPTY_CELL("total_power")
  NEW_EMPTY_CELL("fan_speed")
  NEW_EMPTY_CELL("acceleration_x")
  NEW_EMPTY_CELL("acceleration_y")
  NEW_EMPTY_CELL("acceleration_z")
  NEW_EMPTY_CELL("omega_x")
  NEW_EMPTY_CELL("omega_y")
  NEW_EMPTY_CELL("omega_z")
  NEW_EMPTY_CELL("motor_post_rl")
  NEW_EMPTY_CELL("motor_pre_rl")
  NEW_EMPTY_CELL("motor_pre_rr")
  NEW_EMPTY_CELL("cplate_pre_right")
  NEW_EMPTY_CELL("cplate_pre_left")
  NEW_EMPTY_CELL("motor_post_fr")
  NEW_EMPTY_CELL("motor_post_rr")
  NEW_EMPTY_CELL("cplate_post_left")
  NEW_EMPTY_CELL("suspensions_rl")
  NEW_EMPTY_CELL("suspensions_fl")
  NEW_EMPTY_CELL("suspensions_fr")
  NEW_EMPTY_CELL("suspensions_rr")
  NEW_EMPTY_CELL("gpio_bms")
  NEW_EMPTY_CELL("gpio_imd")
  NEW_EMPTY_CELL("velocity")
  NEW_EMPTY_CELL("latitude")
  NEW_EMPTY_CELL("longitude")
};

JsonCell* JSON_ARRAYS[] = {JSON_1, JSON_2, JSON_3};
const uint32_t JSON_ARRAY_SIZES[] = 
{
  sizeof(JSON_1) / sizeof(JSON_1[0]),
  sizeof(JSON_2) / sizeof(JSON_2[0]),
  sizeof(JSON_3) / sizeof(JSON_3[0]),
};

const uint8_t JSON_ARRAY_COUNT = (sizeof(JSON_ARRAYS) / sizeof(JSON_ARRAYS[0]));

struct LogTelemetry_t{
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


static JsonCell* _search_json_for_log_var(const char* var_name)
{
  for (uint8_t json_num=0;json_num<JSON_ARRAY_COUNT;++json_num)
  {
    for(uint32_t json_line=0;json_line<JSON_ARRAY_SIZES[json_num];++json_line)
    {
      if (!strcmp(JSON_ARRAYS[json_num][json_line].m_name, var_name))
      {
        return &JSON_ARRAYS[json_num][json_line];
      }
    }
  }

  return NULL;
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

  return 0;
}

int8_t log_telemetry_add_entry(LogTelemetry_h* const restrict self __attribute__((__unused__)),
    const char* name, const char* const data_format, const void* const var, const enum DATA_MODE data_type)
{
  JsonCell* json_cell = NULL;

  if( (json_cell = _search_json_for_log_var(name)) == NULL || json_cell->p_data)
  {
    return -1;
  }

  json_cell->m_data_type = data_type;
  json_cell->p_data = var;
  json_cell->data_format = data_format;

  return 0;
}

int8_t log_telemetry_send(LogTelemetry_h* const restrict self)
{
  union LogTelemetry_h_t_conv conv = {self};
  struct LogTelemetry_t* const restrict p_self = conv.clear;
  JsonCell* p_log_page = NULL;
  Json_h json = {0};
  int8_t err=0;
  float var_value =0;
  const uint8_t MAX_RETRY_INIT_JSON = 255;


  for (uint8_t i=0; i<JSON_ARRAY_COUNT; i++)
  {
    uint8_t retry=0;
    while(json_init(&json)<0 && retry < MAX_RETRY_INIT_JSON)
    {
      ++retry;
    }
    if (retry == MAX_RETRY_INIT_JSON)
    {
      continue;
    }

    p_log_page = JSON_ARRAYS[i];
    for (uint8_t j=0;j<JSON_ARRAY_SIZES[i];j++)
    {
      JsonCell* cursor = &p_log_page[j];
      var_value =0;
      if (cursor->p_data)
      {
        switch (cursor->m_data_type)
        {
          case __u8__:
            var_value = *(uint8_t *) cursor->p_data;
            break;
          case __u16__:
            var_value = *(uint16_t *) cursor->p_data;
            break;
          case __u32__:
            var_value = *(uint32_t *) cursor->p_data;
            break;
          case __i8__:
            var_value = *(int8_t *) cursor->p_data;
            break;
          case __i16__:
            var_value = *(int16_t *) cursor->p_data;
            break;
          case __i32__:
            var_value = *(int32_t *) cursor->p_data;
            break;
          case __float__:
            var_value = *(float*) cursor->p_data;
            break;
          default:
        }
      }
      json_push_element(&json, cursor->m_name, cursor->data_format, var_value);
    }
    UdpIpv4Mex mex = {
      .data_length = json_len(&json),
      .raw_data = json_get(&json),
    };
    hardware_ethernet_udp_send(p_self->p_ethernet_udp_telemetry, &mex);
    json_destroy(&json);
    p_log_page = NULL;
  }

  return err;
}


int8_t log_telemetry_destroy(LogTelemetry_h* const restrict self)
{
  union LogTelemetry_h_t_conv conv = {self};
  struct LogTelemetry_t* const restrict p_self = conv.clear;

  hardware_ethernet_udp_free(&p_self->p_ethernet_udp_telemetry);

  memset(self, 0, sizeof(*self));

  return 0;
}
