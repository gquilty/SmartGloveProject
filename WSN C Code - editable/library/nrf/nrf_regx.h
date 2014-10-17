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
 * Functions for reading and writing to the registers that are accessed through
 * REGX_CTRL.
 *
 * @file nrf_regx.h
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#ifndef NRF_REGX_H
#define NRF_REGX_h


/*------------------------------------------------------------------------------
 * Addresses used with REGX_CTRL
 */
 /** Watchdog register. **/
#define WD                  0x00

/** GTIMER / RTC value / RTC latch. **/
#define GTIMER              0x01

/** LSB of RTC latch. **/
#define RTCLAT              0x02

/** LB of RTC value. **/
#define RRTC                0x03

/** RTC Disable. **/
#define WRTCDIS             0x03

/** Wakeup status. **/
#define RWSTA0              0x04

/** GPIO configuration for P0.0 to P0.3. **/
#define WWCON0              0x05

/** Wakeup status. **/
#define RWSTA1              0x05

/** GPIO configuration for P0.4 to P0.7. **/
#define WWCON1              0x06


/**
 * Writes to the registers controlled by REGX_CTRL.
 *
 * @param address indirect address to write to.
 * @param value value to write.
 **/
void regx_write(uint8_t address, uint16_t value);


/**
 * Reads from the registers controlled by REGX_CTRL.
 *
 * @param address indirect address to read from.
 * @return value read.
 **/
uint16_t regx_read(uint8_t address);


#endif
