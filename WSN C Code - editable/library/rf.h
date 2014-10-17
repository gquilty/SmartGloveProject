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
 * Common header file for using different radios.
 *
 * See the @e rf_nrf9x5.h and @e rf_2420.h files for information specific to
 * each radio.
 *
 * @c RF_MAX_PAYLOAD_SIZE is defined to the maximum size the radio supports.
 * This can be overridden to a smaller value if it is defined before including 
 * this file. The CC2420 supports 115, and the nRF905/nRF9E5 30 bytes.
 *
 * @file rf.h
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#ifndef RF_H
#define RF_H


#if (defined (RADIO_NRF905)) || (defined (RADIO_NRF9E5))
#   include "nrf/rf_nrf9x5.h"
#elif defined (RADIO_2420)
#   include "avr/rf_2420.h"
#endif


/**
 * If a message is sent to this address, all nodes with the same NETWORK_ID
 * will receive it.
 **/
#define RF_BROADCAST_ADDRESS    0xFFFF


/**
 * Option for @c rf_setMode(). @c rf_init() must be called before using the
 * radio again.
 **/
#define RF_MODE_OFF             0

/**
 * Option for @c rf_setMode(). Radio is in lowest power mode that retains
 * settings.
 **/
#define RF_MODE_SLEEP           1

/**
 * Option for @c rf_setMode(). Radio can start quicker than from @c
 * RF_MODE_SLEEP.
 **/
#define RF_MODE_STANDBY         2

/** Option for @c rf_setMode(). **/
#define RF_MODE_RECEIVING       3


/** Structure for storing information about a received message. **/
typedef struct {
    uint8_t length;             /**< Length in bytes of the message. **/
	uint16_t srcAddress;        /**< Where the message came from. **/
	uint8_t seqNumber;          /**< Sequence number of message. **/
	int8_t rssi;                /**< RSSI value in dB (only for CC2420). **/
    volatile uint8_t* volatile data;     /**< Payload. **/
} rf_msgType;


/**
 * Initialise the radio, but leave it in sleep mode. See @e rf_nrf9x5.h and
 * @e rf_2420.h for valid values for the parameters.
 *
 * @param channel RF Channel.
 * @param power output power.
 **/
void rf_init(uint16_t channel, uint8_t power);


/**
 * Send a number of bytes using the radio. If a current transmission is taking
 * place, this function will block until the previous transmission is finished.
 *
 * @param address address to send data to.
 * @param msg pointer to data that will be sent.
 * @param length number of bytes in data to send.
 **/
void rf_send(uint16_t address, const uint8_t* msg, uint8_t length);


/**
 * Send an unmodulated carrier signal at the current channel and power. This can
 * be useful for RF testing and characterisation.
 **/
void rf_sendCarrier(void);


/**
 * Tell the RF code where to store received messages. Must be done to be able to
 * receive messages.
 *
 * @param receiveBuffer where to store packet and information about packet.
 **/
void rf_setReceiveBuffer(volatile rf_msgType* receiveBuffer);


/**
 * Change the mode of the radio. If set to @c RF_MODE_OFF, then rf_init() must
 * be called before using the radio again. @c RF_MODE_SLEEP will use less
 * current than @c RF_MODE_RECEIVING but it will take longer for the radio to
 * become active.
 *
 * @param mode @c RF_MODE_OFF, @c RF_MODE_SLEEP, @c RF_MODE_RECEIVING, or
 *     @c RF_MODE_STANDBY.
 **/
void rf_setMode(uint8_t mode);


/**
 * Set RF channel to use for transmitting and receiving.
 *
 * @param channel RF Channel.
 **/
void rf_setChannel(uint16_t channel);


/**
 * Set output power to use for transmitting.
 *
 * @param power power to change to.
 **/
void rf_setPower(uint8_t power);


/**
 * Increase the power used to transmit.
 **/
void rf_increasePower(void);


/**
 * Decrease the power used to transmit.
 **/
void rf_decreasePower(void);


/**
 * Allow the radio to receive messages sent to broadcast address (0xFFFF). This
 * is not needed for CC2420.
 **/
void rf_setMulticastAddress(void);


/**
 * Allow the radio to receive messages sent to its own unique address. This
 * is not needed for CC2420.
 **/
void rf_setUnicastAddress(void);


/**
 * This function should be defined by the application. Calling other rf
 * functions from this callback may have strange effects!.
 *
 * @param msg data received from radio, and message information.
 **/
void rf_callback(volatile rf_msgType* msg);


#endif
