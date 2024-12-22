#ifndef __CAR_TELEMETRY__
#define __CAR_TELEMETRY__

#include <stdint.h>

int8_t telemetry_init(void);
int8_t telemetry_add_entry(const uint16_t position);
int8_t telemetry_update_and_send(void);

#endif // !__CAR_TELEMETRY__
