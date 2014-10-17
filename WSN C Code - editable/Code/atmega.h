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
 * Header file for reading and writing to internal memory on ATmega
 * microcontrollers.
 *
 * References:
 *  - Atmel, ATmega128 Datasheet.
 *  - Atmel, ATmega325 Datasheet.
 *  - Atmel, App Note AVR910: In-System Programming.
 *  - Tyndall 25mm USB programming board schematic.
 *
 * @file atmega.h
 * @date 24-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#ifndef ATMEGA_H
#define ATMEGA_H


/**
 * Used to get details of the atmega chip that is present, and initialise SPI
 * connection.
 *
 * @param memory defines what type of memory will be programmed (Flash, EEPROM,
 *     or fuses).
 * @param isSlowClock is set when a slow clock speed is needed.
 * @return pointer to structure containing description of memory, SPI pins and
 *     description in internal FT232R EEPROM.
 **/
spiDeviceInfo_t* atmega_init(memType_t memory, bool isSlowClock);


/**
 * Programs a number of bytes to memory. Flash, EEPROM, or fuses has been
 * selected by atmega_init().
 *
 * @param data pointer to bytes that will be written to memory.
 * @param address where to start writing.
 * @param size How many bytes to write.
 **/
void atmega_write(const uint8_t* data, uint32_t address, uint32_t size);


/**
 * Reads a number of bytes from memory. Flash, EEPROM, or fuses has been
 * selected by atmega_init().
 *
 * @param[out] data where to store data. Must be big enough for memory type.
 * @param address where to start reading.
 * @param size how many bytes to read.
 **/
void atmega_read(uint8_t* data, uint32_t address, uint32_t size);


/** Writes 0xFF to every location in the memory. **/
void atmega_erase(void);


/**
 * When reprogramming support is needed, some data is added to the binary code
 * image.
 * @code
 *      /-------------------\  0x20000 (128kB)
 *      |  Bootloader Code  |
 *      |-------------------|  0x1F800 (127kB)
 *      |       empty       |
 *      |-------------------|  N + 0x10000
 *      |     Application   |
 *      |        Code       |
 *      |-------------------|  0x10000 (63 kB)
 *      |       empty       |
 *      |-------------------|  0x0F808
 *      |     App CRC       |    0x0F806
 *      |     App length    |    0x0F804
 *      |     App CRC       |    0x0F802
 *      |     App length    |
 *      |-------------------|  0x0F800 (63 kB)
 *      |       empty       |
 *      |-------------------|  N
 *      |     Application   |
 *      |        Code       |
 *      \-------------------/  0x00000 (0 kB)
 * @endcode
 *
 * @param[in,out] data program data, must be 128kB is size.
 * @param rawCodeSize size of code being sent to function.
 * @param isSupportReprog if this is zero, nothing is done.
 **/
void atmega_addHeader(uint8_t* data, uint32_t rawCodeSize,
                      bool isSupportReprog);


/** Resets ATmega. **/
void atmega_reset(void);


/**
 * Converts Fuse string into 3byte array. Fuse should be 6 hexadecimal digits:
 * @c "<ext_byte><high_byte><low_byte>"
 *
 * @param fuseString string of fuse values.
 * @param[out] data bytes that will contain fuse values.
 **/
void atmega_parseFuseValues(const char* fuseString, uint8_t data[3]);


/**
 * Prints out information of what the fuse values mean. Only for ATmega128!
 *
 * @param fuses Fuse value as 3 bytes: @c "<ext_byte><high_byte><low_byte>"
 **/
void atmega_printFuseInfo(uint8_t fuses[3]);

#endif
