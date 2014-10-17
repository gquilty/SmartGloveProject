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
 * Header for very simple IO functions. As the 10mm nodes have very limited
 * memory, it is advisable to use these instead of @c printf(). If
 * @c SIMPLEIO_SMALLER is defined, then only unsigned numebrs are supported, and
 * leading 0's of numbers will be printed, i.e. decimal numbers will always have
 * five digits, and hexadecimal will have 4 digits. If @c SIMPLEIO_LOWERCASE is
 * defined then lowercase letters will be used in hexadecimal numbers.
 *
 * @file simpleIo.h
 * @date 17-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#ifndef SIMPLEIO_H
#define SIMPLEIO_H

#include "uart.h"

//#define SIMPLEIO_SMALLER
//#define SIMPLEIO_LOWERCASE


/* putchar() and getchar() will use these function if stdio is not included. */
#if defined (UC_8051)
#   define getchar          uart_getchar
#   define putchar          uart_putchar
#   define puts             simpleIo_puts
#   define gets             simpleIo_gets
#endif


/**
 * Writes a string to the UART. @c "\0" should be in the string! This function
 * should be accessed by calling @c puts().
 *
 * @param string what to write.
 **/
void simpleIo_puts(const char* string);


/**
 * Read a string from the UART. Be aware of buffer overflow risks! This function
 * should be accessed by calling @c gets().
 *
 * @param string where to store characters that are read.
 **/
void simpleIo_gets(char* string);


/**
 * Write an array of bytes to the UART.
 *
 * @param array what to write.
 * @param length maximum number of bytes to output.
 **/
void putArray(const uint8_t* array, uint16_t length);


/**
 * Read an array of bytes from the UART.
 *
 * @param array where to store characters that are read.
 * @param length number of bytes to read.
 **/
void getArray(uint8_t* array, uint16_t length);


/**
 * Writes an signed number to UART in decimal.
 *
 * @param value number to write.
 **/
void putNum(int16_t value);


/**
 * Writes an unsigned number to UART in decimal.
 *
 * @param value number to write.
 **/
void putUNum(uint16_t value);


/**
 * Writes a 16bit number to UART in hex format.
 *
 * @param value number to write.
 **/
void putHex(uint16_t value);
void putHex_JT(uint8_t value);

#endif
