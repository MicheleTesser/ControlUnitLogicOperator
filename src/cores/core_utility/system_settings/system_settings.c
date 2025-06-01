#include "system_settings.h"
#include "../../../lib/DPS/dps.h"
#include "../../../lib/raceup_board/raceup_board.h"

#include <stdint.h>
#include <string.h>
#include <stdatomic.h>

struct SytemSettingOwner_t{
  DpsSlave_h m_dps_slave;
  struct CanMailbox* p_mailbox_recv_dps_mex;
};

union SytemSettingOwner_h_t_conv{
  SytemSettingOwner_h* const restrict hidden;
  struct SytemSettingOwner_t* const restrict clear;
};

#define NEW_SETTING_PARAM(SET_TYPE)\
  {.m_setting_type = SET_TYPE, .p_data= NULL}

#define BUFFER_SIZE_CAPACITY_BYTE 128

struct CanMailbox* P_MAILBOX_SEND_DPS_MEX;
static uint8_t SETTING_DATA_BUFFER[BUFFER_SIZE_CAPACITY_BYTE];
static struct SystemParams{
  enum DPS_PRIMITIVE_TYPES m_setting_type;
  void* p_data;
}SYSTEM_SETTINGS_PARAMS[__NUM_OF_SYSTEM_SETTINGS];
static volatile uint8_t INIT_DONE=0;

#ifdef DEBUG
char __assert_size_system_settings[sizeof(SytemSettingOwner_h)==sizeof(struct SytemSettingOwner_t)?+1:-1];
char __assert_align_system_settings[_Alignof(SytemSettingOwner_h)==_Alignof(struct SytemSettingOwner_t)?+1:-1];
char __assert_minimum_size_setting_data_buffer[(__NUM_OF_SYSTEM_SETTINGS < BUFFER_SIZE_CAPACITY_BYTE)?+1:-1];
#endif /* ifdef DEBUG */

static int8_t _dps_send_fun(const DpsCanMessage* const restrict mex)
{
  return hardware_mailbox_send(P_MAILBOX_SEND_DPS_MEX, mex->full_word);
}

static inline void _to_string(const SystemSettingName name, char str_name[VAR_NAME_LENGTH])
{
  uint8_t size =0;
  const uint8_t max_size = VAR_NAME_LENGTH;
  switch (name) {
#define X(name,type, value) case name:\
    size = sizeof(#name)>max_size? max_size: sizeof(#name);\
    memcpy(str_name, #name, size);\
    break;

    SYSTEM_SETTINGS
#undef X
    case __NUM_OF_SYSTEM_SETTINGS:
    default:
      break;
  }
}

static inline void _default_value(const SystemSettingName name, union SystemSettingValue_t* const o_buffer)
{
  switch (name)
  {
#define X(name, type, value) case name: o_buffer->f32 = value; break;
    SYSTEM_SETTINGS
#undef X
    default:
      o_buffer->f32 = 0;
      break;
  }
}

static inline uint8_t _get_data_size(const SystemSettingName name)
{
  enum DPS_PRIMITIVE_TYPES system_type =0;
  switch (name)
  {
#define X(name, type, value) case name: system_type = type; break;
    SYSTEM_SETTINGS
#undef X
    default:
      system_type = DPS_TYPES_UINT8_T;
      break;
  };

  switch (system_type)
  {
    case DPS_TYPES_UINT8_T:
    case DPS_TYPES_INT8_T:
      return 1;

    case DPS_TYPES_UINT16_T:
    case DPS_TYPES_INT16_T:
      return 2;

    case DPS_TYPES_UINT32_T:
    case DPS_TYPES_INT32_T:
    case DPS_TYPES_FLOAT_T:
      return 4;

    default:
      return 1;
  }
}

//public

int8_t system_settings_init(SytemSettingOwner_h* const restrict self)
{
  union SytemSettingOwner_h_t_conv conv = {self};
  struct SytemSettingOwner_t* const restrict p_self = conv.clear;
  struct CanNode* can_node = NULL;
  int8_t err=0;
  uint32_t cursor=0;

  if(*p_self->m_dps_slave.private_data)
  {
    goto init_already_done;
  }

  ACTION_ON_CAN_NODE(CAN_GENERAL, can_node)
  {
    p_self->p_mailbox_recv_dps_mex = 
      hardware_get_mailbox_single_mex(
          can_node,
          RECV_MAILBOX,
          CAN_ID_DPSMASTERMEX, 
          8);
  }

  if (!p_self->p_mailbox_recv_dps_mex)
  {
    goto init_recv_mailbox_failed;
  }

  ACTION_ON_CAN_NODE(CAN_GENERAL, can_node)
  {
    P_MAILBOX_SEND_DPS_MEX =
      hardware_get_mailbox_single_mex(
          can_node,
          SEND_MAILBOX,
          CAN_ID_DPSSLAVEMEX,
          8);
  }

  if (!P_MAILBOX_SEND_DPS_MEX)
  {
    goto init_send_mailbox_failed;
  }

  const char board_name[BOARD_NAME_LENGTH] = "CULO";
  if(dps_slave_init(
        &p_self->m_dps_slave,
        _dps_send_fun,
        board_name, 0,
        CAN_ID_DPSMASTERMEX,
        CAN_ID_DPSSLAVEMEX)<0)
  {
    goto init_dps_failed;
  }

  dps_slave_start(&p_self->m_dps_slave);

  for (SystemSettingName sett=0; sett<__NUM_OF_SYSTEM_SETTINGS; sett++)
  {
    const enum DPS_PRIMITIVE_TYPES var_type = SYSTEM_SETTINGS_PARAMS[sett].m_setting_type;
    const uint8_t data_size = _get_data_size(sett);
    union SystemSettingValue_t val= {0};
    char var_name[VAR_NAME_LENGTH] = {0};

    void* restrict p_data = &SETTING_DATA_BUFFER[cursor];

    if (cursor + data_size >= BUFFER_SIZE_CAPACITY_BYTE)
    {
      goto monitoring_failed;
    }

    ;
    _default_value(sett, &val);
    memcpy(p_data, &val, data_size);

    _to_string(sett, var_name);

    if(dps_monitor_primitive_var(
        &p_self->m_dps_slave,
        var_type,
        p_data,
        NULL,
        var_name)<0)
    {
      goto monitoring_failed;
    }

    SYSTEM_SETTINGS_PARAMS[sett].p_data = p_data;

    cursor+=data_size;
  }

  INIT_DONE = 1;

  return 0;

monitoring_failed:
  err--;
  dps_slave_destroy(&p_self->m_dps_slave);
init_dps_failed:
  err--;
  hardware_free_mailbox_can(&P_MAILBOX_SEND_DPS_MEX);
init_send_mailbox_failed:
  err--;
  hardware_free_mailbox_can(&p_self->p_mailbox_recv_dps_mex);
init_recv_mailbox_failed:
  err--;
init_already_done:
  err--;

  return err;
}

int8_t system_settings_update(SytemSettingOwner_h* const restrict self)
{
  union SytemSettingOwner_h_t_conv conv = {self};
  struct SytemSettingOwner_t* const restrict p_self = conv.clear;
  CanMessage mex ={0};

  if (hardware_mailbox_read(p_self->p_mailbox_recv_dps_mex, &mex))
  {
    DpsCanMessage dps_mex = 
    {
      .dlc = mex.message_size,
      .full_word = mex.full_word,
      .id = (uint16_t) mex.id,
    };
    return dps_slave_check_can_command_recv(&p_self->m_dps_slave, &dps_mex);
  }

  return 0;
}

int8_t system_settings_get(const SystemSettingName setting,
    union SystemSettingValue_t* const restrict o_value)
{
  int8_t err=0;
  uint8_t data_size =0;

  while (!INIT_DONE);

  if (setting >= __NUM_OF_SYSTEM_SETTINGS)
  {
    goto invalid_system_settings;
  }
  struct SystemParams* p_param = &SYSTEM_SETTINGS_PARAMS[setting];

  if (!p_param->p_data)
  {
    goto parameter_not_initialized;
  }

  data_size = _get_data_size(setting);

  memcpy(o_value, p_param->p_data, data_size);
  return 0;

parameter_not_initialized:
  err--;
invalid_system_settings:
  err--;

  return err;
}
