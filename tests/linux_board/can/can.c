#include "can.h"
#include "can_lib/canlib.h"
#include <linux/can.h>
#include <stdio.h>
#include <string.h>
#include <sys/cdefs.h>

#define CAN_INTERFACE_0 "culo_can_0"
#define CAN_INTERFACE_1 "culo_can_1"
#define CAN_INTERFACE_2 "culo_can_2"

static int extract_can_node(const BoardComponentId id){
    // switch (id) {
    //     case 0:
    //         return can0;
    //     case 1:
    //         return can1;
    //     case 2:
    //         return can2;
    //     default:
    //         return 0;
    // }
    switch (id) {
        case 0:
             
            return can_init(CAN_INTERFACE_0);
        case 1:
            return can_init(CAN_INTERFACE_1);
        case 2:
            return can_init(CAN_INTERFACE_2);
        default:
            return -1;
    }
}

//public
int8_t hardware_init_can(const BoardComponentId id,uint32_t baud_rate __attribute_maybe_unused__)
{
    // switch (id) {
    //     case 0:
    //         can0 = can_init(CAN_INTERFACE_0);
    //         return can0;
    //     case 1:
    //         can1 = can_init(CAN_INTERFACE_1);
    //         return can1;
    //     case 2:
    //         can2 = can_init(CAN_INTERFACE_2);
    //         return can2;
    //
    //     default:
    //         return -1;
    // }
    return 0;
}

int8_t hardware_read_can(const BoardComponentId id, CanMessage* mex)
{
    struct can_frame frame;
    memset(&frame, 0, sizeof(frame));
    int can_node = extract_can_node(id);
    can_recv_frame(can_node, &frame);
    mex->message_size = frame.len;
    mex->id = frame.can_id;
    memcpy(mex->buffer, frame.data, frame.len);
    close(can_node);
    return 0;
}

int8_t hardware_write_can(const BoardComponentId id, const CanMessage* restrict const mex)
{
    struct can_frame frame;
    int can_node = extract_can_node(id);
    frame.can_id = mex->id;
    frame.len = mex->message_size;
    memcpy(frame.data, mex->buffer, mex->message_size);
    can_send_frame(can_node, &frame);
    close(can_node);
    return 0;
}
