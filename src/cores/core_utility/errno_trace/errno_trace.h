#ifndef __ERRNO_TRACE__
#define __ERRNO_TRACE__

#include <stdint.h>
#include "../core_status/core_status.h"
#define TRACE_DEPTH 16

typedef struct {
  int32_t m_err;
  const char* p_place;
}Trace;

typedef struct
{
  const Trace* p_traces;
  uint8_t m_err_num;
}ErrnoTraces;

int8_t
errno_trace_push_trace(const enum CORES core, const int32_t err, const char* const place);

int8_t
errno_trace_get(const enum CORES core, ErrnoTraces* const restrict o_trace);

int8_t
errno_trace_print(const enum CORES core);

int8_t
errno_trace_clear(const enum CORES core);

#define SET_TRACE(core) errno_trace_push_trace(core, (uint32_t) __LINE__, __FILE__);

#endif // !__ERRNO_TRACE__
