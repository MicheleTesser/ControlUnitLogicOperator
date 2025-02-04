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
    p_self->vars = calloc(1, sizeof(*p_self->vars));
    p_self->cap_entry=1;
    p_self->num_entry=0;

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

    if (p_self->num_entry >= p_self->cap_entry)
    {
      p_self->cap_entry*=2;
      p_self->vars = realloc(p_self->vars, p_self->cap_entry * sizeof(*p_self->vars));
    }
    struct TelemetryEntry* const restrict entry = &p_self->vars[p_self->num_entry];
    p_self->num_entry++;

    entry->p_var = var;
    entry->var_type = data_type;
    entry->p_name = name;
    entry->data_rage = data_range;
    entry->json_cursor = position;

    return 0;
}


int8_t
log_telemetry_destroy(LogTelemetry_h* const restrict self)
{
    union LogTelemetry_h_t_conv conv = {self};
    struct LogTelemetry_t* const restrict p_self = conv.clear;

    free(p_self->json.s_json);
    free(p_self->vars);

    memset(self, 0, sizeof(*self));

    return 0;
}
