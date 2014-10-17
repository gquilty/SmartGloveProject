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
 * This file has global definitions. It should be included in every *.c file.
 *
 * @file global.h
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#ifndef GLOBAL_H
#define GLOBAL_H


/** Support for @c bool. **/
#define bool                    _Bool
/** Support for @c false. **/
#define false                   0
/** Support for @c true. **/
#define true                    1


#ifndef NULL
/** Define @c NULL if not already defined. **/
#define NULL                    0
#endif


#ifndef UINT8_MAX
    /** Unsigned 8-bit value. **/
    typedef unsigned char       uint8_t;
    /** Unsigned 16-bit value. **/
    typedef unsigned int        uint16_t;
    /** Unsigned 32-bit value. **/
    typedef unsigned long int   uint32_t;
    /** Signed 8-bit value. **/
    typedef signed char         int8_t;
    /** Signed 16-bit value. **/
    typedef signed int          int16_t;
    /** Signed 32-bit value. **/
    typedef signed long int     int32_t;

    /** Maximum value of unsigned 8-bit integer. **/
#   define UINT8_MAX            255
    /** Maximum value of unsigned 16-bit integer. **/
#   define UINT16_MAX           65535
    /** Maximum value of unsigned 32-bit integer. **/
#   define UINT32_MAX           4294967295UL
    /** Minimum value of signed 8-bit integer. **/
#   define INT8_MIN             -128
    /** Maximum value of signed 8-bit integer. **/
#   define INT8_MAX             127
    /** Minimum value of signed 16-bit integer. **/
#   define INT16_MIN            -32768
    /** Maximum value of signed 16-bit integer. **/
#   define INT16_MAX            32767
    /** Minimum value of signed 32-bit integer. **/
#   define INT32_MIN            -2147483648L
    /** Maximum value of signed 32-bit integer. **/
#   define INT32_MAX            2147483647L
#endif


/** Data-type for indicating status. **/
typedef unsigned char status_t;

/** status = no errors **/
#define STATUS_OK               0
/** status = invalid function arguments **/
#define STATUS_INVALID_ARG      1
/** status = no acknowledgement received **/
#define STATUS_NO_ACK           2
/** status = acknowledgement received**/
#define STATUS_ACK              3
/** status = unspecified communication error **/
#define STATUS_COMM_ERROR       4
/** status = inavlid baud rate selected **/
#define STATUS_INVALID_BAUDRATE 5
/** status = device didn't respond **/
#define STATUS_NO_REPSONSE      6



/* Some useful commands (this section is for doxygen only). */
#if defined(__DOXYGEN__)

/** Enable interrupts **/
#define enableInterrupts()

/** Disable interrupts **/
#define disableInterrupts()

/** Insert no-op instruction code (does nothing, used for delays) **/
#define nop()

/** Used in a function delcaration, to force the function to be inlined **/
#define FORCE_INLINE

/**
 * Declares an interrupt service routing.
 * @param addr Address of interrupt.
 **/
#define ISR(addr)
#elif defined(UC_AVR)
#   define enableInterrupts()   __asm__ volatile ("sei" ::)
#   define disableInterrupts()  __asm__ volatile ("cli" ::)
#   define nop()                __asm__ volatile ("nop" ::)
#   define FORCE_INLINE         __attribute__ ((always_inline))
#elif defined(UC_8051)
#   define enableInterrupts()   __enable_interrupt()
#   define disableInterrupts()  __disable_interrupt()
#   define nop()                __no_operation()
#   define FORCE_INLINE         _Pragma("inline=forced")
    /*
     * Allow interrupts to be defined with ISR(vector). It's done like this to
     * avoid warnings when the --require_prototypes parameter is used in
     * icc8051.exe
     */
#   define _PRAGMA(x)           _Pragma(#x)  /* Create string from x */
#   define _ISR(x,f)            _PRAGMA(vector=x) __interrupt void f(void)
#   define ISR(x)               _ISR(x,x##_isr); _ISR(x,x##_isr)
#endif


/**
 * Use this to remove warnings about unused function parameters. In the body
 * of the function put @code UNUSED(param1); @endcode where @c param1 is the
 * parameter name.
 **/
#define UNUSED(expr) do { (void)(expr); } while (0)


/** Getting a bitmask with a single bit set. **/
#define BIT(n)                  (1 << (n))


/** Get LSB from larger value. **/
#define BYTE_0(x)               ((uint8_t)(x))

/** Get second LSB from larger value. **/
#define BYTE_1(x)               ((uint8_t)((x) >> 8))

/** Get third LSB from larger value. **/
#define BYTE_2(x)               ((uint8_t)((x) >> 16))

/** Get fourth LSB from larger value. **/
#define BYTE_3(x)               ((uint8_t)((x) >> 24))

/** Get low byte from 16-bit value. **/
#define LOW_BYTE(x)             BYTE_0(x)

/** Get high byte from 16-bit value. **/
#define HIGH_BYTE(x)            BYTE_1(x)

/** Get low word from 32-bit value. **/
#define LOW_WORD(x)             ((uint16_t)(x))

/** Get high word from 32-bit value. **/
#define HIGH_WORD(x)            ((uint16_t)((x) >> 16))

/** Combine 2 bytes into a 16-bit value. **/
#define TO_UINT16(b1, b0)       (((uint16_t)(0xFF & b1) << 8) | \
                                ((uint16_t)(0xFF & b0) << 0))

/** Combine 4 bytes into a 32-bit value. **/
#define TO_UINT32(b3,b2,b1,b0)  (((uint32_t)(0xFF & b3) << 24) | \
                                ((uint32_t)(0xFF & b2) << 16) | \
                                ((uint32_t)(0xFF & b1) <<  8) | \
                                ((uint32_t)(0xFF & b0) <<  0))

/** Combine 2 words into a 32-bit value. **/
#define W_TO_UINT32(w1, w0)     (((uint32_t)(0xFFFF & w1) << 16) | \
                                ((uint32_t)(0xFFFF & w0) <<  0))


/* Include definitions for registers, ports, pins etc. and stdio. */
#if defined (UC_AVR)
#   include <avr/io.h>
#   include <avr/interrupt.h>

#   if defined (NO_STDIO)
#       include "simpleIo.h"
#   else
        /* Macro to put strings in printf statements into program memory. */
#       include <stdio.h>
#       include <avr/pgmspace.h>
#       define printf(x, ...)   printf_P(PSTR(x), ## __VA_ARGS__)
#   endif
#elif defined (UC_8051)
#   include "intrinsics.h"
#   include "nrf/nrf9e5_io.h"
#   include "spi.h"
#   if defined (NO_STDIO)
#       include "simpleIo.h"
#   else
#       include <stdio.h>
#   endif
    /* Avoid problems if PSTR() is used in AVR code */
#   define PSTR(x) x
#endif


#endif
