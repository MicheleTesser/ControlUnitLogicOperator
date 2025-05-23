#ifndef __SYSTEM_SETTINGS__
#define __SYSTEM_SETTINGS__

#include <stdint.h>


typedef struct __attribute__((aligned(4))){
  const uint8_t private_data[544];
}SytemSettingOwner_h;

union SystemSettingValue_t
{
  uint8_t u8;
  uint16_t u16;
  uint32_t u32;

  int8_t i8;
  int16_t i16;
  int32_t i32;

  float f32;
};

#define SYSTEM_SETTINGS\
  X(CORE_0_SERIAL_TRACE)\
  X(CORE_1_SERIAL_TRACE)\
  X(CORE_2_SERIAL_TRACE)\

typedef enum
{
#define X(name) name,
  SYSTEM_SETTINGS
#undef X
  __NUM_OF_SYSTEM_SETTINGS
}SystemSettingName;

int8_t
system_settings_init(SytemSettingOwner_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t
system_settings_update(SytemSettingOwner_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t
system_settings_get(const SystemSettingName setting,
    union SystemSettingValue_t* const restrict o_value) __attribute__((__nonnull__(2)));


#endif // !__SYSTEM_SETTINGS__
