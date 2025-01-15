#ifndef __CAR_STATUS__
#define __CAR_STATUS__

#include <stdint.h>

struct CarStatus;

enum CarStatusInfo{
    CAR_STATUS_DV_STATUS_READY,
    CAR_STATUS_DV_STATUS_DRIVING,
    CAR_STATUS_RES_READY_GO,
};

int8_t car_status_send_status(void);
int8_t car_status_get_info(const enum CarStatusInfo info);

#endif // !__CAR_STATUS__
