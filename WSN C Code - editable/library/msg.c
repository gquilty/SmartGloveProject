/******************************************************************************\
 * Copyright (c) 2010, Tyndall National Institute
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the Tyndall National Institute nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

/***************************************************************************//**
 * Implementation of functions supporting tree network. See @e msg.h
 *
 * @todo check for ACK after sending.
 *
 * @file msg.c
 * @date 16-Apr-2010
 * @author Seán Harte
 ******************************************************************************/


#include "global.h"
#include "rf.h"
#include "msg.h"
#include "uart.h"


#ifndef MSG_LEAF_NODE
/**
 * A 2-dimensional array with 2 columns. The first column is the address of a
 * node. The second column is the address of the parent of that node. This is
 * not present in leaf nodes.
 **/
extern uint16_t nodes[MSG_TOTAL_NODES][2];

/* Function prototype */
static uint16_t getParent(uint16_t address);
#endif


static volatile msgType* msgBuffer;
static volatile rf_msgType rf_msgBuffer;

#ifdef MSG_GATEWAY_NODE
static volatile uint8_t uartBuffer[MSG_MAX_SIZE + MSG_OVERHEAD_SIZE];
static uint8_t uartIndex = 0;
static bool isUartError = false;
#endif




/******************************************************************************\
 * See msg.h for documentation of these functions.
\******************************************************************************/
void msg_init(msgType* rxBuffer)
{
    uart_init();
    rf_init(RF_CHANNEL_CENTRE, RF_PWR_MAX);
    msgBuffer = rxBuffer;
    rf_msgBuffer.data = (volatile uint8_t*)rxBuffer;
    rf_setReceiveBuffer(&rf_msgBuffer);
    rf_setMode(RF_MODE_RECEIVING);
}

status_t msg_send(const msgType* txMsg)
{
    uint16_t rfAddress = RF_PARENT_ADDRESS;

#ifdef MSG_GATEWAY_NODE
    if (txMsg->type == MSG_TYPE_SENSORDATA) {
        putchar(0x02);      /* STX */
        putchar(txMsg->length);
        putchar(HIGH_BYTE(txMsg->address));
        putchar(LOW_BYTE(txMsg->address));
        for (uint8_t i = 0; i < txMsg->length; i++) {
            putchar(txMsg->data[i]);
        }
        putchar(0x03);      /* ETX */
        return STATUS_OK;
    }
#endif

#ifndef MSG_LEAF_NODE
    if (txMsg->type != MSG_TYPE_SENSORDATA) {
        /* Search through parents of the end destination node, until the current 
         * node is found. The actual address this node should send to will then 
         * be one level below this.
         */
        rfAddress = txMsg->address;
        while (RF_LOCAL_ADDRESS != getParent(rfAddress)) {
            rfAddress = getParent(rfAddress);
            if (rfAddress == 0) {
                return STATUS_INVALID_ARG;      /* Something's gone wrong... */
            }
        }
    }
#endif

    rf_send(rfAddress, (uint8_t const*)txMsg, txMsg->length + MSG_OVERHEAD_SIZE);

    return STATUS_OK;
}


void msg_enableRx(void)
{
    uart_enable();
    rf_setMode(RF_MODE_RECEIVING);
}


void msg_disableRx(void)
{
    uart_disable();
    rf_setMode(RF_MODE_SLEEP);
}


/*
 * This is signalled every time an RF packet is received. 
 */
void rf_callback(volatile rf_msgType* msg)
{
        msg_callback(msgBuffer);
}


#if defined(MSG_GATEWAY_NODE) || defined(__DOXYGEN__)
/* This is signalled every time a character is received over the UART. */
void uart_callback(uint8_t received)
{
    /* 0x02 = STX */
    if (received == 0x02) {
        uartIndex = 0;
        isUartError = false;
        return;
    }

    /*
     * If there has been an error (message is too long, or length doesn't match
     * specified length), ignore bytes until next STX is received.
     */
    if (isUartError) {
        return;
    }

    /* 0x03 = ETX */
    if (received == 0x03) {
        if (uartBuffer[0] != uartIndex - MSG_OVERHEAD_SIZE) {
            isUartError = true;
            return;
        }
        msg_callback((msgType*)&uartBuffer);
        return;
    }

    /* Don't let index go over buffer size */
    if (uartIndex + 1 >= RF_MAX_PAYLOAD_SIZE) {
        isUartError = true;
        return;
    }

    uartBuffer[uartIndex++] = received;
}
#endif

#ifndef MSG_LEAF_NODE
/**
 * Looks up the table of nodes to find the parent of any node. Not required for
 * leaf nodes.
 *
 * @param address address of node to find parent of.
 * @return address of the parent node.
 **/
static uint16_t getParent(uint16_t address)
{
    for (uint8_t i = 0; i < MSG_TOTAL_NODES; ++i) {
        if (nodes[i][0] == address) {
            return nodes[i][1];
        }
    }
    return 0;
}
#endif
