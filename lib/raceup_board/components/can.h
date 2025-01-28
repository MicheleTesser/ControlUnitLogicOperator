#ifndef __VIRTUAL_CAN__
#define __VIRTUAL_CAN__

#include <stdint.h>

typedef struct{
    uint32_t id;
    union{
        uint8_t buffer[8];
        uint32_t words[2];
        uint64_t full_word;
    };
    uint8_t message_size;
}CanMessage;

enum CAN_FREQUENCY{
    _1_MBYTE_S_ = 1000000L,
    _500_KBYTE_S_ = 500000L,
    _250_KBYTE_S_ = 250000L,
    _125_KBYTE_S_ = 125000L,
    _50_KBYTE_S_ = 50000L,
    _20_KBYTE_S_ = 20000L,
    _10_KBYTE_S_ = 10000L,
    _5_KBYTE_S_ = 5000L,
};

enum CAN_MODULES{
    CAN_INVERTER=0,
    CAN_GENERAL,
    CAN_DV,

    __NUM_OF_CAN_MODULES__
};

// enum CAN_MAILBOXES_RECV{
//     CORE_0_DRIVER_HUMAN,
//     CORE_0_DRIVER_DV,
//     CORE_0_IMU,
//     CORE_0_HV,
//     CORE_0_MAPS,
//     CORE_0_MISSIONS,
//
//     CORE_1_IMU,
//     CORE_1_DRIVER,
//     CORE_1_REGEN,
//     CORE_1_SUSP_FRONT,
//     CORE_1_SUSP_REAR,
//     CORE_1_BMS_HV,
//
//     CORE_2_IMU,
//     CORE_2_DV_DRIVER_INPUT,
//     CORE_2_HUMAN_DRIVER_INPUT,
// };


//INFO: maintain the order
// enum CAN_MAILBOXES_SEND{
//     CORE_0_BMS_HV,
//
//     CORE_1_FAN_PUMP,
// };

struct CanNode;
struct CanMailbox;

int8_t
hardware_init_can(const enum CAN_MODULES mod , const enum CAN_FREQUENCY baud_rate);

struct CanNode*
hardware_init_can_get_ref_node(const enum CAN_MODULES mod);

void
hardware_init_can_destroy_ref_node(struct CanNode** restrict self)
    __attribute__((__nonnull__(1)));

extern int8_t
hardware_read_can(struct CanNode* const restrict self ,
        CanMessage* const restrict mex )__attribute__((__nonnull__(1,2)));

extern int8_t
hardware_write_can(const struct CanNode* const restrict self ,
        const CanMessage* restrict const mex )__attribute__((__nonnull__(1,2)));

extern struct CanMailbox*
hardware_get_mailbox(struct CanNode* const restrict self,
        const uint16_t mex_id, const uint16_t mex_size)__attribute__((__nonnull__(1)));

extern struct CanMailbox*
hardware_get_mailbox_send(struct CanNode* const restrict self,
        const uint16_t mex_id, const uint16_t mex_size)__attribute__((__nonnull__(1)));

extern int8_t
hardware_mailbox_read(struct CanMailbox* const restrict self ,
        CanMessage* const restrict o_mex)__attribute__((__nonnull__(1,2)));

extern int8_t
hardware_mailbox_send(struct CanMailbox* const restrict self ,
        const uint64_t data)__attribute__((__nonnull__(1)));

extern void
hardware_free_mailbox_can(struct CanMailbox* const* restrict self )
    __attribute__((__nonnull__(1)));

#define ACTION_ON_CAN_NODE(node,exp)\
{\
    struct CanNode* can_node = hardware_init_can_get_ref_node(node);\
    exp;\
    hardware_init_can_destroy_ref_node(&can_node);\
}

#endif // !__VIRTUAL_CAN__
