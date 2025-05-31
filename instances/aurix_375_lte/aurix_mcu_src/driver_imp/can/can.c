#include "../../src/lib/raceup_board/raceup_board.h"

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
#include "IfxPort.h"

#pragma GCC diagnostic pop

#include <stdint.h>
#include <stdatomic.h>
#include <stddef.h>


//private

#define NUMBER_OF_FIFO_ELEMENTS 15
#define WAIT_TIME                   1                      /* Number of milliseconds to wait after transmission */
#define DO_NOT_CARE_BUFFER_INDEX    (IfxCan_RxBufferId)0
#define CHECK_INIT(node,ret) if(!atomic_load(&(node->init_done))) return ret;

#define LED_STB         &MODULE_P20,6                                           /* LED: Port, Pin definition            */

typedef struct __attribute__((aligned(4))){
  enum MAILBOX_TYPE type;
  atomic_bool in_use;
  struct CanNode* can_node;
}CommonCanMailbox;

typedef struct __attribute__((aligned(4))) CanMailbox {
  CommonCanMailbox common;
  const uint8_t data[32];
}CanMailbox;

typedef struct __attribute__((aligned(4)))
{
  CommonCanMailbox common;
  IfxCan_Filter filter;
  const uint8_t padding[8];
}CanMailboxRx;


typedef struct __attribute__((aligned(4)))
{
  CommonCanMailbox common;
  IfxCan_Message tx_message;
}CanMailboxTx;

typedef union{
  CanMailboxRx* rx_mailbox;
  CanMailboxTx* tx_mailbox;
  CanMailbox* general_mailbox;
}Conv_mailbox_tx_rx;

typedef struct CanNode{
    IfxCan_Can_Config canConfig;                            /* CAN module configuration structure                   */
    IfxCan_Can canModule;                                   /* CAN module handle                                    */
    IfxCan_Can_Node canNode;                                /* CAN node handle data structure                       */
    IfxCan_Can_NodeConfig canNodeConfig;                    /* CAN node configuration structure                     */
    atomic_bool init_done;
    atomic_bool taken;
    struct CanMailbox rx_mailbox[IfxCan_RxBufferId_63 + 1];
    struct CanMailbox tx_mailbox[IfxCan_TxBufferId_31 + 1];
}CanNodeDriver;



#ifdef DEBUG
char __assert_size_can_mailbox_rx[sizeof(CanMailbox)==sizeof(CanMailboxRx)?1:-1];
char __assert_size_can_mailbox_tx[sizeof(CanMailbox)==sizeof(CanMailboxTx)?1:-1];
char __assert_align_can_mailbox_rx[_Alignof(CanMailbox)==_Alignof(CanMailboxRx)?1:-1];
char __assert_align_can_mailbox_tx[_Alignof(CanMailbox)==_Alignof(CanMailboxTx)?1:-1];
#endif

static CanNodeDriver CAN_NODES[__NUM_OF_CAN_MODULES__];

//public

int8_t hardware_init_can(const enum CAN_MODULES mod, const enum CAN_FREQUENCY baud_rate)
{
  if (mod >= __NUM_OF_CAN_MODULES__)
  {
    return -1;
  }
  IfxCpu_disableInterrupts();
  CanNodeDriver* node = &CAN_NODES[mod];
  Conv_mailbox_tx_rx conv_mailbox = {0};
  IfxCan_Can_Pins pins =
  {
    0, IfxPort_OutputMode_pushPull,
    0, IfxPort_InputMode_pullUp,
    IfxPort_PadDriver_cmosAutomotiveSpeed1
  };

  switch (mod)
  {
    case CAN_INVERTER:
      pins.txPin = &IfxCan_TXD01_P33_9_OUT;
      pins.rxPin = &IfxCan_RXD01D_P33_10_IN;
      break;
    case CAN_GENERAL:
      pins.txPin = &IfxCan_TXD12_P23_2_OUT;
      pins.rxPin = &IfxCan_RXD12C_P23_3_IN;
      break;
    case CAN_DV:
      pins.txPin = &IfxCan_TXD03_P00_2_OUT;
      pins.rxPin = &IfxCan_RXD03A_P00_3_IN;
      break;
    case CAN_DEBUG:
      IfxPort_setPinModeOutput(LED_STB, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
      IfxPort_setPinLow(LED_STB);
      pins.txPin = &IfxCan_TXD00_P20_8_OUT;
      pins.rxPin = &IfxCan_RXD00B_P20_7_IN;
      break;
    case __NUM_OF_CAN_MODULES__:
    default:
      IfxCpu_enableInterrupts();
      return -2;
      break;
  }

  IfxCan_Can_initModuleConfig(&node->canConfig, pins.rxPin->module);
  IfxCan_Can_initModule(&node->canModule, &node->canConfig);
  IfxCan_Can_initNodeConfig(&node->canNodeConfig, &node->canModule);

  node->canNodeConfig.pins = &pins;
  node->canNodeConfig.nodeId = pins.rxPin->nodeId;
  node->canNodeConfig.baudRate.baudrate = baud_rate;

  node->canNodeConfig.frame.type = IfxCan_FrameType_transmitAndReceive;
  node->canNodeConfig.frame.mode = IfxCan_FrameMode_standard;

  node->canNodeConfig.filterConfig.standardListSize = 64;
  node->canNodeConfig.filterConfig.standardFilterForNonMatchingFrames = IfxCan_NonMatchingFrame_reject;
  node->canNodeConfig.filterConfig.extendedFilterForNonMatchingFrames = IfxCan_NonMatchingFrame_reject;

  //set rx mailbox node
  node->canNodeConfig.rxConfig.rxMode = IfxCan_RxMode_dedicatedBuffers;
  node->canNodeConfig.rxConfig.rxFifo0Size = NUMBER_OF_FIFO_ELEMENTS;
  node->canNodeConfig.rxConfig.rxFifo1Size = NUMBER_OF_FIFO_ELEMENTS;
  node->canNodeConfig.rxConfig.rxBufferDataFieldSize= IfxCan_DataFieldSize_8;

  uint8_t i=0;
  for (; i<=IfxCan_RxBufferId_63; i++)
  {
    conv_mailbox.general_mailbox = &node->rx_mailbox[i];
    conv_mailbox.rx_mailbox->filter.number = i;
    atomic_store(&conv_mailbox.rx_mailbox->common.in_use,0);
  }

  //set tx mailbox node
  node->canNodeConfig.txConfig.txMode = IfxCan_TxMode_dedicatedBuffers;
  node->canNodeConfig.txConfig.txBufferDataFieldSize= IfxCan_DataFieldSize_8;
  node->canNodeConfig.txConfig.dedicatedTxBuffersNumber= 32;

  if(!IfxCan_Can_initNode(&node->canNode, &node->canNodeConfig))
  {
    IfxCpu_enableInterrupts();
    return -1;
  }

  atomic_store(&node->init_done, 1);
  atomic_store(&node->taken, 0);

  IfxCpu_enableInterrupts();
  return 0;
}

struct CanNode* hardware_init_can_get_ref_node(const enum CAN_MODULES mod)
{
  CanNodeDriver* node = &CAN_NODES[mod];
  if (mod < __NUM_OF_CAN_MODULES__  &&
      atomic_load(&node->init_done) &&
      !atomic_load(&node->taken) )
  {
    atomic_store(&node->taken,1);
    return node;
  }
  return NULL;
}

void hardware_init_can_destroy_ref_node(struct CanNode** restrict self __attribute__((__unused__)))
{
  atomic_store(&(*self)->taken, 0);
  self = NULL;
}

int8_t hardware_read_can(struct CanNode* const restrict self __attribute__((__unused__)),
    CanMessage* const restrict mex __attribute__((__unused__)))
{
  //INFO: not yet implemented because not used yet
  return -1;
}

int8_t hardware_write_can(struct CanNode* const restrict self ,
    const CanMessage* restrict const mex __attribute__((__unused__)) )
{
  CHECK_INIT(self,-1);

  IfxCan_Message tx_message;
  IfxCan_Can_initMessage(&tx_message);

  tx_message.messageId = mex->id;
  tx_message.messageIdLength = IfxCan_MessageIdLength_standard;
  tx_message.frameMode = IfxCan_FrameMode_standard;
  tx_message.dataLengthCode = mex->message_size;

  Ifx_TickTime ticksFor1ms = IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, WAIT_TIME);
  int8_t err= 0;
  int ret=0;

  /* Send the CAN message with the previously defined TX message configuration and content */
  while(IfxCan_Status_ok !=
      (err = IfxCan_Can_sendMessage(&self->canNode, &tx_message, (uint32_t *) &mex->full_word)) &&
      ret++ < 50)
  {
  }

  /* Initialize a time variable for waiting 1 ms */

  /* Adding additional time delay so we are sure that acceptance filtering took place by the node 1 */
  waitTime(ticksFor1ms);
  return ret == 50? -1: 0;
}

struct CanMailbox* hardware_get_mailbox(
    struct CanNode* const restrict self, const enum MAILBOX_TYPE type,
    uint16_t filter_id, const uint16_t filter_mask,
    const uint16_t mex_size)
{
  CHECK_INIT(self, NULL);
  CanMailbox* mailbox_pool = NULL;
  Conv_mailbox_tx_rx conv_mailbox = {0};
  uint32_t mailbox_pool_size = 0;
  uint8_t mailbox_index =0;

  switch (type)
  {
    case FIFO_BUFFER:
      //INFO: not yet implemented
      return NULL;
    case RECV_MAILBOX:
      mailbox_pool = &self->rx_mailbox[0];
      mailbox_pool_size = IfxCan_RxBufferId_63 + 1;
      break;
    case SEND_MAILBOX:
      mailbox_pool = &self->tx_mailbox[0];
      mailbox_pool_size = IfxCan_TxBufferId_31 + 1;
      break;
    default:
      return NULL;
  }

  for (uint8_t i=0; i<mailbox_pool_size; i++)
  {
    conv_mailbox.general_mailbox = &mailbox_pool[i];
    if (!atomic_load(&conv_mailbox.general_mailbox->common.in_use))
    {
      atomic_store(&conv_mailbox.general_mailbox->common.in_use,1);
      conv_mailbox.general_mailbox->common.can_node = self;
      mailbox_index = i;
      break;
    }
  }

  conv_mailbox.general_mailbox = &mailbox_pool[mailbox_index];

  switch (type)
  {
    case FIFO_BUFFER:
      conv_mailbox.general_mailbox->common.type = FIFO_BUFFER;
      conv_mailbox.rx_mailbox->filter.type = IfxCan_FilterType_classic;
      conv_mailbox.rx_mailbox->filter.id1= filter_id;
      conv_mailbox.rx_mailbox->filter.id2= filter_mask;
      conv_mailbox.rx_mailbox->filter.elementConfiguration=
        IfxCan_FilterElementConfiguration_storeInRxFifo0; //HACK: test
      conv_mailbox.rx_mailbox->filter.rxBufferOffset= DO_NOT_CARE_BUFFER_INDEX;

      IfxCan_Can_setStandardFilter(
          &conv_mailbox.general_mailbox->common.can_node->canNode,
          &conv_mailbox.rx_mailbox->filter);
      break;
    case RECV_MAILBOX:
      conv_mailbox.general_mailbox->common.type = RECV_MAILBOX;
      conv_mailbox.rx_mailbox->filter.type = IfxCan_FilterType_classic;
      conv_mailbox.rx_mailbox->filter.id1= filter_id;
      conv_mailbox.rx_mailbox->filter.id2= filter_mask;
      conv_mailbox.rx_mailbox->filter.elementConfiguration= IfxCan_FilterElementConfiguration_storeInRxBuffer;
      conv_mailbox.rx_mailbox->filter.rxBufferOffset= IfxCan_RxBufferId_0 + mailbox_index;

      IfxCan_Can_setStandardFilter(
          &conv_mailbox.general_mailbox->common.can_node->canNode,
          &conv_mailbox.rx_mailbox->filter);
      break;
    case SEND_MAILBOX:
      conv_mailbox.tx_mailbox->tx_message.messageId = filter_id;
      conv_mailbox.tx_mailbox->tx_message.messageIdLength = IfxCan_MessageIdLength_standard;
      conv_mailbox.tx_mailbox->tx_message.frameMode = IfxCan_FrameMode_standard;
      conv_mailbox.tx_mailbox->tx_message.dataLengthCode= IfxCan_DataLengthCode_0 + mex_size;
      conv_mailbox.tx_mailbox->tx_message.bufferNumber = IfxCan_TxBufferId_0 + mailbox_index;
      break;
  }

  return conv_mailbox.general_mailbox;
}

int8_t hardware_mailbox_read(struct CanMailbox* const restrict self,
    CanMessage* const restrict o_mex)
{
  IfxCan_Message message;
  IfxCan_RxBufferId rxBufferId =0;
  uint8_t f=0;

  IfxCan_Can_initMessage(&message);

  switch (self->common.type)
  {
    case FIFO_BUFFER:
      message.readFromRxFifo0 = TRUE;
      //WARN: hardcoding one fifo buffer, be aware there are only two for each node in aurix
      if (!(f=IfxCan_Can_getRxFifo0FillLevel(&self->common.can_node->canNode)))
      {
        return 0;
      }
      break;
    case RECV_MAILBOX:
      rxBufferId = self - self->common.can_node->rx_mailbox;
      message.bufferNumber = rxBufferId;
      if(!IfxCan_Can_isNewDataReceived(&self->common.can_node->canNode, rxBufferId))
      {
        return 0;
      }
      break;
    case SEND_MAILBOX:
      return -1;
      break;
  }
  IfxCan_Can_readMessage(&self->common.can_node->canNode,
      &message,
      &o_mex->words[0]);
  o_mex->message_size = IfxCan_Node_getDataLength(message.dataLengthCode);
  o_mex->id = message.messageId; 
  return o_mex->id;
}

int8_t hardware_mailbox_send(struct CanMailbox* const restrict self ,
    const uint64_t data)
{
  Conv_mailbox_tx_rx general_mailbox = {.general_mailbox = self};

  if (self->common.type != SEND_MAILBOX && !atomic_load(&self->common.in_use))
  {
    return -1;
  }

  return IfxCan_Can_sendMessage(
      &general_mailbox.general_mailbox->common.can_node->canNode,
      &general_mailbox.tx_mailbox->tx_message,
      (uint32_t *)&data);
}

void hardware_free_mailbox_can(struct CanMailbox** restrict self)
{
  atomic_store(&(*self)->common.can_node->taken,0);
  self = NULL;
}
