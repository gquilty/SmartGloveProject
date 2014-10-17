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
 * Header for very simple I/O print functions.
 *
 * @file simpleIo.c
 * @date 17-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#include "global.h"
#include "simpleIo.h"


#ifdef SIMPLEIO_LOWERCASE
#   define ALPHA_START ('a' - 10)
#else
#   define ALPHA_START ('A' - 10)
#endif


static void putDigit(uint8_t digit);
static void printNumber(uint16_t value, uint8_t base);
static void printNumber_JT(uint8_t value, uint8_t base);

/******************************************************************************\
 * See simpleIo.h for documentation of these functions
\******************************************************************************/

void simpleIo_puts(const char* string)
{
    while (*string) {
        putchar(*string++);
    }
}


void simpleIo_gets(char* string)
{
    uint8_t ch;
    while ((ch = getchar()) != '\n') {
        *string++ = ch;
    }
}


void putArray(const uint8_t* array, uint16_t length)
{
    for (uint16_t i = 0; i < length; ++i) {
        putchar(*array++);
    }
}


void getArray(uint8_t* array, uint16_t length)
{
    for (uint16_t i = 0; i < length; ++i) {
        *array++ = getchar();
    }
}


void putNum(int16_t value)
{
#ifndef SIMPLEIO_SMALLER
    if (value < 0) {
        value = -value;
        putchar('-');
    }
#endif
    printNumber(value, 10);
}


void putUNum(uint16_t value)
{
    printNumber(value, 10);
}


void putHex(uint16_t value)
{
    printNumber(value, 16);
}

void putHex_JT(uint8_t value)
{
    printNumber_JT(value, 16);
}
/******************************************************************************\
 * Functions used only within this file.
\******************************************************************************/

/**
 * Writes a single digit to UART.
 *
 * @param digit what to write.
 **/
static void putDigit(uint8_t digit)
{
    putchar(digit + ((digit < 10) ? '0' : ALPHA_START));
}

/**
 * Writes a number to UART.
 *
 * @param value value of number to print.
 * @param base 10 = decimal, 16 = hexadecimal. Other values will print decimal.
 **/
static void printNumber_JT(uint8_t value, uint8_t base)
{
    uint16_t divVal = 100;

    if (base == 16) {
        divVal = 0x10;
    }
    
#ifndef SIMPLEIO_SMALLER
    while (divVal > 1 && divVal > value) {
        divVal /= base;
    }
#endif
    do {
        putDigit(value / divVal);
        value %= divVal;
        divVal /= base;
    } while (divVal);
}
static void printNumber(uint16_t value, uint8_t base)
{
    uint16_t divVal = 10000;

    if (base == 16) {
        divVal = 0x1000;
    }
    
#ifndef SIMPLEIO_SMALLER
    while (divVal > 1 && divVal > value) {
        divVal /= base;
    }
#endif
    do {
        putDigit(value / divVal);
        value %= divVal;
        divVal /= base;
    } while (divVal);
}