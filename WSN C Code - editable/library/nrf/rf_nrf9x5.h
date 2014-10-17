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
 * Header file for using Nordic nRF905 or nRF9E5 radio.
 * This file should not be included directly. Use @e rf.h.
 *
 * @file rf_nrf9x5.h
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#ifndef RF_NRF9x5_H
#define RF_NRF9x5_H


#include "spi_nrfCommands.h"


/**
 * The channel is a 9bit integer. The frequency is calculated as follows:
 * @code freq = (422.4 + channel / 10) * (1 + HFREQ_PLL) @endcode
 * @c HFREQ_PLL is defined in the board header files e.g. @e nrf905_revB_433.h.
 **/
#define RF_CHANNEL_CENTRE   116     /* 434 MHz or 868 MHz */


/** Values for setting output power (-10dBm). **/
#define RF_PWR_NEG10        (0 << PA_PWR)

/** Values for setting output power (-2dBm). **/
#define RF_PWR_NEG2         (1 << PA_PWR)

/** Values for setting output power (6dBm). **/
#define RF_PWR_6            (2 << PA_PWR)

/** Values for setting output power (10dBm). **/
#define RF_PWR_10           (3 << PA_PWR)

/** Maximum output power (-10dBm). **/
#define RF_PWR_MAX          RF_PWR_10

/** Minimum output power (-10dBm). **/
#define RF_PWR_MIN          RF_PWR_NEG10

#if !defined(RF_MAX_PAYLOAD_SIZE) || defined(__DOXYGEN__)
/** Maximum size of a packet that can be sent in one payload. **/
#define RF_MAX_PAYLOAD_SIZE    30
#endif

/* If MAX_PAYLOAD_SIZE has been defined elsewhere, check it's size. */
#if RF_MAX_PAYLOAD_SIZE > 30
#error "RF_MAX_PAYLOAD_SIZE must not be > 30 for nRF9x5"
#endif


#endif
