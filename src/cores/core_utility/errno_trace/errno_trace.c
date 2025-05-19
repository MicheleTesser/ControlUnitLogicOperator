#include "errno_trace.h"
#include "../core_status/core_status.h"
#include "../../../lib/raceup_board/raceup_board.h"

#include <stddef.h>
#include <stdint.h>


typedef struct{
  Trace m_err_buffer[TRACE_DEPTH];
  uint8_t m_cursor;
}CoreTraces;

static struct
{
  CoreTraces m_core_trace[__NUM_OF_CORES__];
}CORE_TRACES;

#define VARLID_CORE(core) if (core >= __NUM_OF_CORES__) return -1;

//public

int8_t errno_trace_push_trace(const enum CORES core, const int8_t err, const char* const place)
{
  CoreTraces* core_trace = NULL;
  Trace* trace = NULL;

  VARLID_CORE(core);
  core_trace = &CORE_TRACES.m_core_trace[core];

  if (core_trace->m_cursor==TRACE_DEPTH)
  {
    return -2; //queue full
  }

  trace = &core_trace->m_err_buffer[core_trace->m_cursor++];

  trace->m_err = err;
  trace->p_place = place;

  return 0;
}

int8_t errno_trace_get(const enum CORES core, ErrnoTraces* const restrict o_trace)
{
  CoreTraces* core_trace = NULL;

  VARLID_CORE(core);
  core_trace = &CORE_TRACES.m_core_trace[core];

  o_trace->p_traces = core_trace->m_err_buffer;
  o_trace->m_err_num = core_trace->m_cursor;

  return 0;
}

int8_t errno_trace_print(const enum CORES core)
{
  ErrnoTraces core_trace = {0};
  const Trace* trace = NULL;
  uint8_t tabs =0;

  VARLID_CORE(core);
  if(errno_trace_get(core, &core_trace)<0)
  {
    return -2;
  }

  serial_write_raw("error traceback core : ");
  serial_write_uint32_t(core);
  serial_write_str("");

  for (int8_t i = 0; i < core_trace.m_err_num; i++)
  {
    trace = &core_trace.p_traces[i];
    for (uint8_t j=0; j<tabs; j++)
    {
      serial_write_raw("\t");
    }
    serial_write_raw(trace->p_place);
    serial_write_raw(" :" );
    serial_write_int8_t(trace->m_err);
    serial_write_str("");
    tabs++;
  }

  return 0;
}

int8_t errno_trace_clear(const enum CORES core)
{
  VARLID_CORE(core);

  CORE_TRACES.m_core_trace[core].m_cursor=0;
  return 0;
}
