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
 * Provides an interface for inter node and node-PC communication in a tree
 * network. Each node must have a unique 16 bit address. Messages can be sent
 * from any node to the gateway, where they will be transferred to the PC.
 * Messages can also be sent from the PC to any node
 *
 * When using the radio, each packet will look like this:
 * @verbatim
       <length:8><address:16><type:8><data:N>
       
           length = length of data section
           address = source (if message going to PC) or destination address
           type = see definitions below@endverbatim
 *
 * For the serial, the message will look like this:
 * @verbatim
       <STX:8><length:8><address:16><type:8><data:N><ETX:8>@endverbatim
 *
 * There are are three types of nodes. The gateway node is a 25mm node connected
 * to the PC. Nodes where @c MSG_LEAF_NODE is defined have no children. It can 
 * be a Tyndall 10mm node. Other nodes are in between and must be 25mm nodes.
 *
 * @note The @c rf_callback() and @c uart_callback() functions are
 * implemented by this library, and so cannot be implemented in an application.
 *
 * @c UART_DO_NOT_INSERT_RETURN and @c UART_USE_CALLBACK must be defined. The 
 * best place to do that is the application @e makefile.
 *
 * @todo A lot of funtionality can be implemented under this interface:
 *  - Checksum to verify message.
 *  - Packetisation to break up large messages into multiple transmissions.
 *  - ACKs.
 *  - Tranmission frequency and power.
 *  - MAC algorithms.
 *  - Dynamic tree routing.
 *  - Clustering.
 *
 * @file msg.h
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#ifndef MSG_H
#define MSG_H

#include "rf.h"

/** Sensor readings, format: "<type:8><sensorData:VAR>" **/
#define MSG_TYPE_SENSORDATA         0x01

/** Command to move code memory: "<type:8><addrOld:16><addrNew:16><cnt:16>" **/
#define MSG_TYPE_REPROG_MOVE        0x10

/** Command to write to code memory: "<type:8><addr:16><count:8><data:VAR>" **/
#define MSG_TYPE_REPROG_WRITE       0x20

/** Command to restart node with new image: "<type:8>" **/
#define MSG_TYPE_REPROG_RESTART     0x30

/** Command saying code image is invalid: "<type:8>" **/
#define MSG_TYPE_REPROG_INVALID     0x02


/** Address of PC. **/
#define MSG_UART_ADDRESS            0x0000

/** Address of node connected to PC. **/
#define MSG_GATEWAY_ADDRESS         0x0001

/** Bytes needed for message header/footer. **/
#define MSG_OVERHEAD_SIZE           4

/** Maxmum message size that can be sent. **/
#define MSG_MAX_SIZE                RF_MAX_PAYLOAD_SIZE - MSG_OVERHEAD_SIZE


/** Structure representing message. 4 bytes of overhead. **/
typedef volatile struct {
    uint8_t length;                 /**< Length in bytes of data. **/
	uint16_t address;               /**< Source/destination of message. **/
	uint8_t type;                   /**< Type of message. **/
    uint8_t data[MSG_MAX_SIZE];     /**< Data payload. **/
} msgType;


/**
 * Initalises interfaces (radio, and UART) for message transmission.
 *
 * @param rxBuffer where to store incoming messages.
 **/
void msg_init(msgType* rxBuffer);


/**
 * Send a message to address specified in the structure.
 *
 * @param txMsg data and address information for sending the message.
 *
 * @return status of sending.
 **/
status_t msg_send(const msgType* txMsg);


/** Enable message reception. **/
void msg_enableRx(void);


/** Disable message reception. **/
void msg_disableRx(void);


/**
 * This function should be implemented by the application. It is triggered
 * when a message is received. The application should check if the node's
 * address is as specified in the message. If not it should be forwarded on to
 * the correct address.
 *
 * @param rxMsg data and address information of the received message.
 **/
void msg_callback(volatile msgType* rxMsg);


#endif
