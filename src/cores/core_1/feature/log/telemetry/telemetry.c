#include "telemetry.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../log_obj_types.h"

struct TelemetryEntry{
    const void* const restrict p_var;
    const uint8_t* var_name;
    const enum DATA_MODE var_type;
};

struct Json{
    char* s_json;
    uint32_t json_cursor_offset;
};

struct LogTelemetry_t{
    struct Json json;
    struct TelemetryEntry* vars;
};

union LogTelemetry_h_t_conv{
    LogTelemetry_h* const restrict hidden;
    struct LogTelemetry_t* const restrict clear;
};

static inline void push_in_json(struct Json* const restrict self,
        const char* const restrict str, const uint32_t str_len)
{
    sprintf(&self->s_json[self->json_cursor_offset], "%s", str);
    self->json_cursor_offset+=str_len;
}

int8_t
log_telemetry_init(LogTelemetry_h* const restrict self __attribute__((__nonnull__)))
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
log_telemetry_add_entry(LogTelemetry_h* const restrict self __attribute__((__nonnull__)),
        const char* const restrict name, const void* const restrict var,
        const enum DATA_MODE data_type, const DataRange data_range)
{
    union LogTelemetry_h_t_conv conv = {self};
    struct LogTelemetry_t* const restrict p_self = conv.clear;
    const uint8_t name_len = strlen(name);
    const char var_header[] = ",\"";
    const char value_header[] = "\":";
    struct Json* json = &p_self->json;
    char str_var_value[data_range];
    const char termination[] ="}\0";

    switch (data_type) {
        case DATA_UNSIGNED:
            if (data_range <= UINT8_MAX)
            {
                sprintf(str_var_value, "%uc", *(uint8_t *) var);
            }
            else if (data_range <= UINT32_MAX)
            {
                sprintf(str_var_value, "%ud", *(uint32_t *) var);
            }
            break;
        case DATA_SIGNED:
            if (data_range <= INT8_MAX)
            {
                sprintf(str_var_value, "%c", *(int8_t *) var);
            }
            else if (data_range <= INT32_MAX)
            {
                sprintf(str_var_value, "%d", *(int32_t *) var);
            }
            break;
        case DATA_FLOATED:
            sprintf(str_var_value, "%.6f", *(double *) var);
            break;
        default:
            return -1;
    }
    p_self->json.s_json = realloc(p_self->json.s_json,
            strlen(json->s_json) +
            strlen(var_header) +
            name_len + 
            strlen(value_header)+
            data_range);

    push_in_json(json, var_header, strlen(var_header));
    push_in_json(json, name, name_len);
    push_in_json(json, value_header, strlen(value_header));
    push_in_json(json, str_var_value, data_range);
    push_in_json(json, termination, strlen(termination));
    json->json_cursor_offset-=strlen(termination);


    return 0;
}
