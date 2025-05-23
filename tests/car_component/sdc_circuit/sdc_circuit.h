#ifndef __SD_CIRCUIT__
#define __SD_CIRCUIT__

#include <stdint.h>

int8_t sdc_init(void);
int8_t sdc_is_closed(void);
void sdc_stop(void);

#endif // !__SD_CIRCUIT__
