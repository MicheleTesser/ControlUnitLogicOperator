#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "IfxCan_Can.h"
#include "IfxCan.h"
#include "IfxCpu_Irq.h"
#include "IfxPort.h"

#include "../../Libraries/ControlUnitLogicOperator/lib/raceup_board/components/can.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>


#define MAXIMUM_CAN_DATA_PAYLOAD    2                       /* Define maximum classical CAN payload in 4-byte words */
#define CAN_ISR_PROVIDER            IfxSrc_Tos_cpu0                         /* Interrupt provider                   */
#define ISR_PRIORITY_CAN_RX         2                       			    /* Define the CAN RX interrupt priority */
#define MAXIMUM_SEND_ATTEMPTS       4096



struct CanNode{
    struct{
        IfxCan_Can_Config canConfig;                            /* CAN module configuration structure                   */
        IfxCan_Can canModule;                                   /* CAN module handle                                    */
        IfxCan_Can_Node canNode;                                /* CAN node handle data structure                       */
        IfxCan_Can_NodeConfig canNodeConfig;                    /* CAN node configuration structure                     */
        IfxCan_Message txMsg;                                   /* Transmitted CAN message structure                    */
        IfxCan_Message rxMsg;                                   /* Received CAN message structure                       */
        uint32 txData[MAXIMUM_CAN_DATA_PAYLOAD];                /* Transmitted CAN data array                           */
        uint32 rxData[MAXIMUM_CAN_DATA_PAYLOAD];                /* Received CAN data array                              */
    } g_mcmcan;
    boolean canBusBusy;
};

static struct CanNode nodes[3];



static struct CanNode* extract_can_node_from_id(const BoardComponentId id){
    switch (id) {
        case 0:
            return &nodes[0];
        default:
            return NULL;
    }
}

/*
 * typedef struct
 * {
 *     IfxCan_Txd_Out    *txPin;
 *     IfxPort_OutputMode txPinMode;
 *     IfxCan_Rxd_In     *rxPin;
 *     IfxPort_InputMode  rxPinMode;
 *     IfxPort_PadDriver  padDriver;
 * } IfxCan_Can_Pins;
 */

static void setup_pin_from_id(const BoardComponentId id,IfxCan_Can_Pins* pins){
    switch (id) {
        case 0:
            pins->txPinMode = IfxPort_OutputMode_pushPull;
            pins->rxPinMode = IfxPort_InputMode_pullUp;
            pins->padDriver = IfxPort_PadDriver_cmosAutomotiveSpeed1;
            pins->txPin = &IfxCan_TXD00_P20_8_OUT;
            pins->rxPin = &IfxCan_RXD00B_P20_7_IN;
            break;
        case 1:
            break;
        case 2:
            break;
        default:
            break;
    }
}


static void setup_interrupt_from_id(const BoardComponentId id)
{
    struct CanNode* can_node= extract_can_node_from_id(id);
    IfxCan_Can_InterruptConfig* interrupt_config = &can_node->g_mcmcan.canNodeConfig.interruptConfig;


    switch (id) {
        case 0:
            interrupt_config->messageStoredToDedicatedRxBufferEnabled = TRUE;
            interrupt_config->reint.priority = ISR_PRIORITY_CAN_RX;
            interrupt_config->reint.interruptLine = IfxCan_InterruptLine_0;
            interrupt_config->reint.typeOfService = CAN_ISR_PROVIDER;
            break;
        default:
            break;
    
    }
}

//public
int8_t hardware_init_can(const BoardComponentId id,uint32_t baud_rate)
{
    struct CanNode* can_node= extract_can_node_from_id(id);

    IfxCpu_disableInterrupts();
    IfxCan_Can_Pins pins;
    setup_pin_from_id(id, &pins);

    IfxCan_Can_initModuleConfig(&can_node->g_mcmcan.canConfig, pins.rxPin->module);

    IfxCan_Can_initModule(&can_node->g_mcmcan.canModule, &can_node->g_mcmcan.canConfig);
    
    IfxCan_Can_initNodeConfig(&can_node->g_mcmcan.canNodeConfig, &can_node->g_mcmcan.canModule);

    //g_mcmcan.canNodeConfig.busLoopbackEnabled = TRUE;
    can_node->g_mcmcan.canNodeConfig.pins = &pins;
    can_node->g_mcmcan.canNodeConfig.nodeId = pins.rxPin->nodeId;
    can_node->g_mcmcan.canNodeConfig.baudRate.baudrate = baud_rate;

    can_node->g_mcmcan.canNodeConfig.frame.type = IfxCan_FrameType_transmitAndReceive;

    // setup_interrupt_from_id(id);

    IfxCan_Can_initNode(&(can_node->g_mcmcan.canNode), &(can_node->g_mcmcan.canNodeConfig));

    IfxCpu_enableInterrupts();
    return 0;
}

extern int8_t hardware_read_can(const BoardComponentId id, uint32_t* msg_id,
        void* buffer, uint8_t* buffer_size)
{
    struct CanNode* can_node= extract_can_node_from_id(id);
    IfxCan_Can_readMessage(&can_node->g_mcmcan.canNode , &can_node->g_mcmcan.rxMsg , can_node->g_mcmcan.rxData );
    *msg_id = can_node->g_mcmcan.rxMsg.messageId;
    buffer =  &(can_node->g_mcmcan.rxData);
    *buffer_size = 8;


    return 0;
}

extern int8_t hardware_write_can(const BoardComponentId id, const CanMessage* restrict const mex)
{
    struct CanNode* can_node= extract_can_node_from_id(id);
    IfxCan_Can_initMessage(&(can_node->g_mcmcan.txMsg));

    /* Define the content of the data to be transmitted */
    uint8_t mex_size = mex->message_size % 9;
    uint8_t* data_buffer = mex->buffer;
    memcpy(&can_node->g_mcmcan.txData[0], data_buffer, mex->message_size);
    if (mex->message_size > 4) {
        memcpy(&can_node->g_mcmcan.txData[1], &data_buffer[4], mex_size - 4);
    }

    /* Set frame parameters */
    can_node->g_mcmcan.txMsg.messageId = mex->id;
    can_node->g_mcmcan.txMsg.dataLengthCode = mex_size;
    can_node->g_mcmcan.txMsg.errorStateIndicator = FALSE;
    can_node->g_mcmcan.txMsg.remoteTransmitRequest = FALSE;
    can_node->g_mcmcan.txMsg.storeInTxFifoQueue = TRUE;
    can_node->g_mcmcan.txMsg.txEventFifoControl = TRUE;

    /* Send the CAN message with the previously defined TX message content */

    uint16 sendRetry = 0;
    while(IfxCan_Status_notSentBusy == IfxCan_Can_sendMessage(
                &(can_node->g_mcmcan.canNode), 
                &(can_node->g_mcmcan.txMsg), 
                &(can_node->g_mcmcan.txData[0])) && sendRetry < MAXIMUM_SEND_ATTEMPTS){
        sendRetry++;
    }

    return 0;
}
