#ifndef __CORE_2_IMU__
#define __CORE_2_IMU__

#include <stdint.h>
#include "../../../../../../lib/raceup_board/components/can.h"

typedef struct DvImu_h{
    const uint8_t private_data[1];
}DvImu_h;

enum DV_IMU_DATA{
    //TODO: not yet defined
    GPS_MAYBE
};

int8_t
dv_imu_init(DvImu_h* const restrict self __attribute__((__nonnull__)),
        const struct CanMailbox* const restrict mailbox);

int8_t
dv_imu_update(DvImu_h* const restrict self __attribute__((__nonnull__)));

int8_t
dv_imu_get(DvImu_h* const restrict self __attribute__((__nonnull__)),
        const enum DV_IMU_DATA data);

#endif // !__CORE_2_IMU__
