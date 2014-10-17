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
 * Where to add support for new ATmega devices.
 *
 * This header makes it easy to add support for programming new ATmega devices.
 * To add a new device a new row should be added to @c atmega_infoTable and
 * @c atmega_strings. When the project is recompiled, the new device will be
 * recognised.
 *
 * References:
 *  - Atmel, ATmega128 Datasheet.
 *  - Atmel, ATmega1281 Datasheet.
 *  - Atmel, ATmega325P Datasheet.
 *  - Atmel, ATmega2561 Datasheet.
 *
 * @file atmegaDevices.h
 * @date 24-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#ifndef ATMEGADEVICES_H
#define ATMEGADEVICES_H


#define ATMEGA_ATMEGA128_SIG    0x1E9702
#define ATMEGA_ATMEGA1281_SIG   0x1E9704
#define ATMEGA_ATMEGA2561_SIG   0x1E9802
#define ATMEGA_ATMEGA325P_SIG   0x1E950D


/**
 * A number of different ATmega microcontrollers can be supported, as long as
 * they support SPI memory programming. For example ATmega128, ATmega325, or
 * ATmega2561. Note that this program uses page programming for Flash and slower
 * byte programming for EEPROM as this is supported by most Atmegas. The ATmega
 * is identified by it's signature bytes. The values in the table below show the
 * details about memory sizes for each signature ID.
 * @verbatim
           Signature      | Flash |   Flash   | EEPROM |  EEPROM
             Bytes        | Size  | Page Size |  Size  | Page Size
   @endverbatim
 **/
uint32_t atmega_infoTable[][5] = {
    { ATMEGA_ATMEGA128_SIG, 131072,        256,    4096,          8},
    {ATMEGA_ATMEGA1281_SIG, 131072,        256,    4096,          8},
    {ATMEGA_ATMEGA2561_SIG, 262144,        256,    4096,          8},
    {ATMEGA_ATMEGA325P_SIG,  32768,        128,    1024,          4}
};


/**
 * Strings printed to the screen to identify the chip, MUST be in the same order
 * as the table above.
 **/
const char* atmega_strings[] = {
    "ATmega128",
    "Atmega1281",
    "ATmega2561",
    "Atmega325P"
};

#endif
