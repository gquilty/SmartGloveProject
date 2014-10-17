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
 * Header file for reading and writing to the 25320 EEPROM used by the nRF9e5.
 *
 * References:
 *  - Atmel, AT25320 Datasheet.
 *  - Nordic, nRF9e5 Datasheet.
 *  - Tyndall 10mm nRF9E5 revA/B Schematic.
 *  - Tyndall 10mm Interface Board revB/C Schematic.
 *
 * @file nrf9e5.h
 * @date 24-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#ifndef NRF9E5_H
#define NRF9E5_H


#define NRF_DEFAULT_CRYSTAL_FREQUENCY 20


/**
 * Used to get details of the 25320 EEPROM, and initialise connection.
 *
 * @param freq frequency of crystal oscillator in Mhz (4, 8, 12, 16, or 20).
 * @return Pointer to structure containing description of memory, SPI pins and
 *     description in internal FT232R EEPROM.
 **/
spiDeviceInfo_t* nrf_init(uint32_t freq);


/**
 * Programs a number of bytes to the EEPROM. Header should be added before
 * calling this function: @see nrf_addEepromHeader().
 * @a address + @a size must be <= 4093.
 *
 * @param data pointer to bytes to write to memory.
 * @param address where to start writing to.
 * @param size number of bytes to write to the memory, other bytes will be 0.
 **/
void nrf_write(const uint8_t* data, uint32_t address, uint32_t size);


/**
 * Reads a number of bytes from the EEPROM.
 * @a address + @a size must be <= 4093.
 *
 * @param[out] data location to store read data.
 * @param address where to start reading from.
 * @param size how many bytes to read.
 **/
void nrf_read(uint8_t* data, uint32_t address, uint32_t size);


/** Writes 0xFF to every location in the EEPROM. **/
void nrf_erase(void);


/**
 * Adds 3 byte header to EEPROM data that is required by nRF9E5. Also adds CRC
 * and program size information which is needed for in-system programming. If
 * in-system programming is wanted, then this function also copies the
 * lower-half of the memory to the upper half.
 * @code
 *      byte[409-1]
 *         .
 *         .                = Unused
 *         .
 *      byte[(N+1)*256]
 *      byte[((N+1)*256)-1]
 *         .
 *         .                = Program data
 *         .
 *      byte[7]
 *      byte[6]             = High byte of program CRC
 *      byte[5]             = Low byte of program CRC
 *      byte[4]             = High byte of program size
 *      byte[3]             = Low byte of program size
 *      byte[2]             = How many 256 byte blocks are in the program (N)
 *      byte[1]             = Where program starts (7)
 *      byte[0]             = EEPROM configuration
 * @endcode
 *
 * @param[in,out] data program data.
 * @param rawCodeSize size of code being sent to function.
 * @param isSupportReprog if this is non-zero, then the data is copied to upper
 *      half of memory.
 **/
void nrf_addHeader(uint8_t* data, uint32_t rawCodeSize, bool isSupportReprog);


#endif
