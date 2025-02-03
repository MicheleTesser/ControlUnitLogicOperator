#include "telemetry.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../log_obj_types.h"

struct TelemetryEntry{
    const void* p_var;
    uint32_t json_cursor;
    enum DATA_MODE var_type;
};

struct Json{
    char* s_json;
    uint32_t json_cursor_offset;
};

struct LogTelemetry_t{
    struct Json json;
    struct TelemetryEntry* vars;
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

static inline void push_in_json(struct Json* const restrict self,
        const char* const restrict str, const uint32_t str_len)
{
  return; //HACK: for debugging purpose 
  sprintf(&self->s_json[self->json_cursor_offset], "%s", str);
  self->json_cursor_offset+=str_len;
}

int8_t
log_telemetry_init(LogTelemetry_h* const restrict self )
{
    union LogTelemetry_h_t_conv conv = {self};
    struct LogTelemetry_t* const restrict p_self = conv.clear;

    memset(p_self, 0, sizeof(*p_self));
    p_self->json.s_json = malloc(3 * sizeof(*p_self->json.s_json));
    p_self->json.s_json[0] = '{';
    p_self->json.s_json[1] = '}';
    p_self->json.s_json[2] = '\0';

    p_self->json.json_cursor_offset = 1;

    return 0;
}

int8_t
log_telemetry_add_entry(LogTelemetry_h* const restrict self ,
        const char* name, const void* const var,
        const enum DATA_MODE data_type, const DataRange data_range)
{
  return 0; //HACK: for debugging purpose 
    union LogTelemetry_h_t_conv conv = {self};
    struct LogTelemetry_t* const restrict p_self = conv.clear;
    const uint8_t name_len = strlen(name);
    const char var_header[] = ",\"";
    const char value_header[] = "\":";
    struct Json* json = &p_self->json;
    char str_var_value[data_range];
    memset(str_var_value, ' ', data_range);
    const char termination[] ="}\0";

    p_self->json.s_json = realloc(p_self->json.s_json,
            strlen(json->s_json) +
            strlen(var_header) +
            name_len + 
            strlen(value_header)+
            data_range);

    if(p_self->num_entry>=p_self->cap_entry)
    {
        p_self->cap_entry*=2;
        p_self->vars = realloc(p_self->vars, p_self->cap_entry * sizeof(*p_self->vars));
    }

    push_in_json(json, var_header, strlen(var_header));
    push_in_json(json, name, name_len);
    push_in_json(json, value_header, strlen(value_header));
    push_in_json(json, str_var_value, data_range);
    push_in_json(json, termination, strlen(termination));
    json->json_cursor_offset-=strlen(termination);
    
    struct TelemetryEntry* const restrict entry = &p_self->vars[p_self->num_entry];
    entry->p_var = var;
    entry->var_type = data_type;
    p_self->num_entry++;
    entry->json_cursor = json->json_cursor_offset;


    return 0;
}


int8_t
log_telemetry_destroy(LogTelemetry_h* const restrict self)
{
    union LogTelemetry_h_t_conv conv = {self};
    struct LogTelemetry_t* const restrict p_self = conv.clear;

    free(p_self->json.s_json);

    return 0;
}
