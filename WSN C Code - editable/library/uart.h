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
 * UART communications. The UART can be accessed by calling the
 * @c uart_getChar() and @c uart_putChar() functions, through @e <stdio.h>
 * functions e.g. @c printf(), or using the functions defined in @e simpleIo.h
 *
 * @todo Software implementation of UART.
 * @todo Buffering of UART (interrupt based).
 *
 * @file uart.h
 * @date 17-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#ifndef UART_H
#define UART_H


/* Possible values for Parity check bit options. */
#define UART_PARITY_NONE    0
#define UART_PARITY_ODD     1
#define UART_PARITY_EVEN    2
#define UART_PARITY_SPACE   3


#ifndef UART_BAUDRATE
/**
 * UART baud rate. Not all baudrates will be possible. If the error is greater
 * than @c UART_BAUD_TOL %, there is a compile time warning.
 **/
#define UART_BAUDRATE       38400
#endif

#ifndef UART_BAUD_TOL
/**  Set the percentage tolerance in baudrates for a compile time warning. **/
#define UART_BAUD_TOL       5           /* percentage */
#endif


#ifndef UART_PARITY
/**
 * Parity options: @c UART_PARITY_NONE, @c UART_PARITY_ODD, @c UART_PARITY_EVEN,
 * of @c UART_PARITY_SPACE.
 **/
#define UART_PARITY UART_PARITY_NONE
#endif

/* Check parity is valid. */
#if (UART_PARITY > 3) || (UART_PARITY < 0)
#error "Invalid value for UART_PARITY"
#endif


/**
 * Initialise hardware UART. Note that no handshaking is used. Also note that,
 * currently, the nRF9E5 always uses a baud rate of F_CPU/32 bps, 8 data bits, 1
 * parity bit (space), 1 start bit, and 1 stop bit.
 **/
void uart_init(void);


/**
 * Disable UART hardware.
 **/
void uart_disable(void);


/**
 * Re-enable UART hardware. @c uart_init() should be called before this function
 * is used, to setup the baudrate.
 **/
void uart_enable(void);


/**
 * Write a single character to the UART. It's better (for compatibliity) to call
 * @c putchar() instead.
 *
 * @param ch character to write.
 *
 * @return character written (for compatibility with standard @c putchar() )
 **/
uint8_t uart_putchar(uint8_t ch);


/**
 * Read a single character from the UART. This function will block until a
 * character is read. It's better (for compatibliity) to call @c getchar()
 * instead.
 *
 * @return character read from UART.
 **/
uint8_t uart_getchar(void);


/**
 * If UART_USE_CALLBACK is defined, then the function below is called, and the
 * application can handle the byte, whenever a byte is received.
 *
 * @param received byte received from UART.
 **/
#if defined(UART_USE_CALLBACK) || defined(__DOXYGEN__)
void uart_callback(uint8_t received);
#endif

#endif
