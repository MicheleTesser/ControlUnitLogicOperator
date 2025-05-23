#include "system_settings.h"
#include "../../../lib/DPS/dps.h"
#include "../../../lib/raceup_board/raceup_board.h"

#include <stdint.h>
#include <string.h>
#include <sys/types.h>

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
}SYSTEM_SETTINGS_PARAMS[__NUM_OF_SYSTEM_SETTINGS] =
{
  NEW_SETTING_PARAM(DPS_TYPES_UINT8_T),  //INFO: SET_NAME CORE_0_SERIAL_TRACE
  NEW_SETTING_PARAM(DPS_TYPES_UINT8_T),  //INFO: SET_NAME CORE_1_SERIAL_TRACE
  NEW_SETTING_PARAM(DPS_TYPES_UINT8_T),  //INFO: SET_NAME CORE_2_SERIAL_TRACE
};

#ifdef DEBUG
char __assert_size_system_settings[sizeof(SytemSettingOwner_h)==sizeof(struct SytemSettingOwner_t)?+1:-1];
char __assert_align_system_settings[_Alignof(SytemSettingOwner_h)==_Alignof(struct SytemSettingOwner_t)?+1:-1];
char __assert_minimum_size_setting_data_buffer[(__NUM_OF_SYSTEM_SETTINGS < BUFFER_SIZE_CAPACITY_BYTE)?+1:-1];
#endif /* ifdef DEBUG */

static int8_t _dps_send_fun(const DpsCanMessage* const restrict mex)
{
  return hardware_mailbox_send(P_MAILBOX_SEND_DPS_MEX, mex->full_word);
}

static inline const char* _to_string(const SystemSettingName name)
{
  switch (name) {
#define X(name) case name: return #name;
    SYSTEM_SETTINGS
#undef X
    case __NUM_OF_SYSTEM_SETTINGS:
    default:
      return NULL;
  }
}

static inline void _default_value(const SystemSettingName name, union SystemSettingValue_t* const o_buffer)
{
  switch (name)
  {
#define X(name, type, value) case name: o_buffer->type = value; break;
    DEFAULT_VALUES
#undef X
    default:
      o_buffer->u8 =0;
  }
}

static inline uint8_t _get_data_size(const enum DPS_PRIMITIVE_TYPES type)
{
  switch (type)
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
    default:
      return 4;
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
          0x6000, 
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
          0x610,
          8);
  }

  if (!P_MAILBOX_SEND_DPS_MEX)
  {
    goto init_send_mailbox_failed;
  }

  const char board_name[BOARD_NAME_LENGTH] = "CULO";
  if(dps_slave_init(&p_self->m_dps_slave, _dps_send_fun, board_name, 0, 0x600, 0x610)<0)
  {
    goto init_dps_failed;
  }

  dps_slave_start(&p_self->m_dps_slave);

  for (SystemSettingName sett=0; sett<__NUM_OF_SYSTEM_SETTINGS; sett++)
  {
    const enum DPS_PRIMITIVE_TYPES var_type = SYSTEM_SETTINGS_PARAMS[sett].m_setting_type;
    const uint8_t data_size = _get_data_size(var_type);
    void* restrict p_data = NULL;

    if (cursor + data_size >= BUFFER_SIZE_CAPACITY_BYTE)
    {
      goto monitoring_failed;
    }

    p_data = &SETTING_DATA_BUFFER[cursor];
    _default_value(sett, p_data);

    if(dps_monitor_primitive_var(
        &p_self->m_dps_slave,
        var_type,
        p_data,
        NULL,
        _to_string(sett))<0)
    {
      goto monitoring_failed;
    }

    SYSTEM_SETTINGS_PARAMS[sett].p_data = p_data;

    cursor+=data_size;
  }

  return 0;

monitoring_failed:
  err--;
  dps_slave_destroy(&p_self->m_dps_slave);
init_dps_failed:
  err--;
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

  if (setting >= __NUM_OF_SYSTEM_SETTINGS)
  {
    goto invalid_system_settings;
  }
  struct SystemParams* p_param = &SYSTEM_SETTINGS_PARAMS[setting];

  if (!p_param->p_data)
  {
    goto parameter_not_initialized;
  }

  data_size = _get_data_size(p_param->m_setting_type);

  memcpy(o_value, p_param->p_data, data_size);
  return 0;

parameter_not_initialized:
  err--;
invalid_system_settings:
  err--;

  return err;
}
