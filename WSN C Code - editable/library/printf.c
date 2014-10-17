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
 * This is a very small and incomplete printf function. Only the following 
 * format specifiers are supported:
 *  - %c = character
 *  - %d = signed integer
 *  - %i = signed integer
 *  - %u = unsigned integer
 *  - %s = string
 *  - %x = hexadecimal word (leading '0's are printed, and uppercase is used)
 *  - %X = hexadecimal word (leading '0's are printed, and uppercase is used)
 *  - %% = '%'
 * There is no support for floating point numbers, format flags, or octal
 * numbers.
 *
 * If @c PRINTF_NOT_USED is defined, then printf simply prints the format 
 * string.
 *
 * @file printf.c
 * @date 17-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#include "global.h"
#include "simpleIo.h"

int printf(const char *format, ...);

#ifndef PRINTF_NOT_USED
#include "stdarg.h"

int printf(const char *format, ...)
{
    va_list argPtrs;
    char ch;
    char* ptr;

    va_start(argPtrs, format);

    while ((ch = *(format++))) {
        if (ch != '%') {
            putchar(ch);
        }
        else {
            ch = *(format++);
            switch (ch) {
            case 0:
                va_end(ap);
                return 0;
            case 'c' :
                putchar((char)(va_arg(argPtrs, int16_t)));
                break;
            case 'd':
            case 'i':
                putNum(va_arg(argPtrs, int16_t));
                break;
            case 'u':
                putUNum(va_arg(argPtrs, uint16_t));
            case 's' :
                ptr = va_arg(argPtrs, char*);
                while ((ch = *ptr++)) {
                    putchar(ch);
                }
                break;
            case 'x':
            case 'X' :
                putHex(va_arg(argPtrs, uint16_t));
                break;
            case '%' :
                putchar('%');
            default:
                break;
            }
        }
    }
    return 0;
}

#else
int printf(const char *format, ...)
{
    puts(format);
    return 0;
}
#endif
