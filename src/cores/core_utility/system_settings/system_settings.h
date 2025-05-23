/**
 * @file system_settings.h
 * @brief Interface for managing runtime-configurable system settings.
 */

#ifndef __SYSTEM_SETTINGS__
#define __SYSTEM_SETTINGS__

#include <stdint.h>

/**
 * @brief Opaque handle for system settings management.
 *
 * This object must be initialized with `system_settings_init()` before use.
 * Internally aligned to 4 bytes and contains implementation-specific data.
 */
typedef struct __attribute__((aligned(4))) {
  const uint8_t private_data[544];
} SytemSettingOwner_h;

/**
 * @brief Represents the value of a system setting.
 *
 * This union can store different data types. The type used for each setting
 * is defined in the `SystemParams` array.
 */
union SystemSettingValue_t {
  uint8_t  u8;
  uint16_t u16;
  uint32_t u32;
  int8_t   i8;
  int16_t  i16;
  int32_t  i32;
  float    f32;
};

/**
 * @brief define here the list of settings you want to have.
 */
#define SYSTEM_SETTINGS\
  X(CORE_0_SERIAL_TRACE)\
  X(CORE_1_SERIAL_TRACE)\
  X(CORE_2_SERIAL_TRACE)\

/**
 * @brief (optionally) define here the list of default value (with type) of the settings the type are listed in the union SystemSettingValue_t.
 */
#define DEFAULT_VALUES\
  X(CORE_0_SERIAL_TRACE, u8,  0)\
  X(CORE_1_SERIAL_TRACE, u8,  0)\
  X(CORE_2_SERIAL_TRACE, u8,  0)\

/**
 * @brief List of configurable system settings.
 */
typedef enum {
#define X(name) name,
  SYSTEM_SETTINGS
#undef X
  __NUM_OF_SYSTEM_SETTINGS ///< Number of defined system settings
} SystemSettingName;

/**
 * @brief Initializes the system settings module.
 *
 * Allocates and prepares all internal resources, CAN mailboxes, and DPS monitoring
 * required to manage the settings.
 *
 * @param self Pointer to a preallocated SytemSettingOwner_h instance
 * @return 0 on success, negative value on error
 */
int8_t system_settings_init(SytemSettingOwner_h* const restrict self) __attribute__((__nonnull__(1)));

/**
 * @brief Processes received CAN messages to update system settings.
 *
 * This function should be called periodically to allow DPS communication to function.
 *
 * @param self Pointer to an initialized SytemSettingOwner_h instance
 * @return 0 on success or if no message is received, negative on error
 */
int8_t system_settings_update(SytemSettingOwner_h* const restrict self) __attribute__((__nonnull__(1)));

/**
 * @brief Retrieves the current value of a system setting.
 *
 * @param setting Setting identifier to query
 * @param o_value Output pointer to receive the current setting value
 * @return 0 on success, negative value on error (e.g., invalid ID or uninitialized)
 */
int8_t system_settings_get(const SystemSettingName setting,
                           union SystemSettingValue_t* const restrict o_value) __attribute__((__nonnull__(2)));

#endif // !__SYSTEM_SETTINGS__
