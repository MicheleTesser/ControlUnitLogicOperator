#include "../../src/lib/raceup_board/raceup_board.h"
#include <stdint.h>

#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wpedantic"
#include "Bsp.h"
#include "Ifx_Types.h"
#include "IfxCan_Can.h"
#include "IfxCan.h"
#include "IfxCpu_Irq.h"
#include "IfxPort.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"

#pragma GCC diagnostic pop

#include <stddef.h>


//private

typedef struct{
    IfxCan_Can_Config canConfig;                            /* CAN module configuration structure                   */
    IfxCan_Can canModule;                                   /* CAN module handle                                    */
    IfxCan_Can_Node canNode;                                /* CAN node handle data structure                       */
    IfxCan_Can_NodeConfig canNodeConfig;                    /* CAN node configuration structure                     */
    IfxCan_Can_Pins pins;
    uint8_t init_done;
}CanNodeDriver;

static CanNodeDriver CAN_NODES[] = 
{
  { 
    .pins = {
      &IfxCan_TXD01_P33_9_OUT, IfxPort_OutputMode_pushPull,
      &IfxCan_RXD01D_P33_10_IN, IfxPort_InputMode_pullUp,
      IfxPort_PadDriver_cmosAutomotiveSpeed1
    }, //INFO: node inverter
  },
  {
    .pins = {
      &IfxCan_TXD12_P23_2_OUT, IfxPort_OutputMode_pushPull,
      &IfxCan_RXD12C_P23_3_IN, IfxPort_InputMode_pullUp,
      IfxPort_PadDriver_cmosAutomotiveSpeed1
    },//INFO: node general
  },
  {
    .pins = {
      &IfxCan_TXD03_P00_2_OUT, IfxPort_OutputMode_pushPull,
      &IfxCan_RXD03A_P00_3_IN, IfxPort_InputMode_pullUp,
      IfxPort_PadDriver_cmosAutomotiveSpeed1
    },//INFO: node dv
  },
};
//public

int8_t hardware_init_can(const enum CAN_MODULES mod , const enum CAN_FREQUENCY baud_rate)
{
  if (mod >= __NUM_OF_CAN_MODULES__)
  {
    return -1;
  }
  CanNodeDriver* node = &CAN_NODES[mod];

  IfxCan_Can_initModuleConfig(&node->canConfig, node->pins.rxPin->module);
  IfxCan_Can_initModule(&node->canModule, &node->canConfig);
  IfxCan_Can_initNodeConfig(&node->canNodeConfig, &node->canModule);

  node->canNodeConfig.nodeId = node->pins.rxPin->nodeId;
  node->canNodeConfig.baudRate.baudrate = baud_rate;
  node->canNodeConfig.frame.type = IfxCan_FrameType_transmitAndReceive;

  //set rx mailbox node
  node->canNodeConfig.rxConfig.rxMode = IfxCan_RxMode_dedicatedBuffers;
  node->canNodeConfig.rxConfig.rxBufferDataFieldSize= IfxCan_DataFieldSize_8;

  //set tx mailbox node
  node->canNodeConfig.txConfig.txMode = IfxCan_TxMode_dedicatedBuffers;
  node->canNodeConfig.txConfig.txBufferDataFieldSize= IfxCan_DataFieldSize_8;

  if(!IfxCan_Can_initNode(&node->canNode, &node->canNodeConfig))
  {
    return -2;
  }

  node->init_done=1;

  return 0;
}

struct CanNode* hardware_init_can_get_ref_node(const enum CAN_MODULES mod)
{
  if (mod < __NUM_OF_CAN_MODULES__ && CAN_NODES[mod].init_done)
  {
    return &CAN_NODES[mod];
  }
  return NULL;
}

void hardware_init_can_destroy_ref_node(struct CanNode** restrict self)
{
  self = NULL;
}

int8_t hardware_read_can(struct CanNode* const restrict self , CanMessage* const restrict mex );

int8_t hardware_write_can(const struct CanNode* const restrict self ,
    const CanMessage* restrict const mex );

struct CanMailbox* hardware_get_mailbox(
    struct CanNode* const restrict self, const enum MAILBOX_TYPE type,
    uint16_t filter_id, const uint16_t filter_mask,
    const uint16_t mex_size);

int8_t hardware_mailbox_read(struct CanMailbox* const restrict self ,
    CanMessage* const restrict o_mex);

int8_t hardware_mailbox_send(struct CanMailbox* const restrict self ,
    const uint64_t data);

void hardware_free_mailbox_can(struct CanMailbox** restrict self);
