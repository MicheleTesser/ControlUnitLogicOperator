#ifndef __CAR_SD_LOG__
#define __CAR_SD_LOG__

#include <stdint.h>

int8_t sd_log_init(void);
int8_t sd_log_add_entry(const uint16_t position);
int8_t sd_update_and_write(void);

#endif // !__CAR_SD_LOG__
