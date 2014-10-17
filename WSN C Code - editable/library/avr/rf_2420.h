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
 * Header file for using Texas Instruments CC2420 / Ember EM2420 radio.
 * This file should not be included directly. Use @e rf.h.
 *
 * @file rf_2420.h
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#ifndef RF_2420_H
#define RF_2420_H


/**
 * Valid channels for this radio are from 11 to 26, inclusive. The frequency of
 * the channel is calculated as follows:
 * @code Freq = 2405 + 5*(channel - 11). @endcode
 **/
#define RF_CHANNEL_CENTRE   20      /* 2.45 Ghz */


/** Value for @a power parameter: 0dBm. **/
#define RF_PWR_0            0xFF

/** Value for @a power parameter: -1dBm. **/
#define RF_PWR_NEG1         0xFB

/** Value for @a power parameter: -3dBm. **/
#define RF_PWR_NEG3         0xF7

/** Value for @a power parameter: -5dBm. **/
#define RF_PWR_NEG5         0xF3

/** Value for @a power parameter: -7dBm. **/
#define RF_PWR_NEG7         0xEF

/** Value for @a power parameter: -10dBm. **/
#define RF_PWR_NEG10        0xEB

/** Value for @a power parameter: -15dBm. **/
#define RF_PWR_NEG15        0xE7

/** Value for @a power parameter: -25dBm. **/
#define RF_PWR_NEG25        0xE3

/** Value for @a power parameter: maximum (0dBm). **/
#define RF_PWR_MAX          0xFF

/** Value for @a power parameter: minimum (-25dBm). **/
#define RF_PWR_MIN          0xE3


#if !defined(RF_MAX_PAYLOAD_SIZE) || defined(__DOXYGEN__)
/** Maximum size of a packet that can be sent in one payload. **/
#define RF_MAX_PAYLOAD_SIZE        115
#endif


/* If MAX_PAYLOAD_SIZE has been defined elsewhere, check it's size. */
#if RF_MAX_PAYLOAD_SIZE > 115
#error "RF_MAX_PAYLOAD_SIZE must not be > 115 for xx2420"
#endif


#endif
