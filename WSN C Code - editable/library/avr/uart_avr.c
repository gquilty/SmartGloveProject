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
 *  Simple use of UART0. Simple polling is used to send and receive bytes. No 
 * buffering of input or output is used, so received bytes may be lost.
 *
 * After the UART is initialised it can be accessed through stdout and stdin 
 * using stdio functions: printf(), scanf(), puts() etc.
 *
 * If UART_USE_CALLBACK is defined, then an interrupt is used to handle received
 * bytes and calls the function uart_callback(), which should be implemented by
 * the application. Doing this may interfere with using the UART as stdin.
 *
 * @file uart_avr.c
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#include "global.h"
#include "uart.h"


#ifdef _STDIO_H_
/*
 * Internal function prototypes for UART functions, conforming to the function
 * types needed by AVR LibC for stdout and stdin.
 */
static int write(char c, FILE *unused);
static int read(FILE *unused);

/* Set up stdio to use uart */
static FILE uart0 = FDEV_SETUP_STREAM(write, read, _FDEV_SETUP_RW);
#endif


/******************************************************************************\
 * See uart.h for documentation of these functions.
\******************************************************************************/


void uart_init(void)
{
/* See avr-libc docs on <util/setbaud.h> for explanation of this code */
#define BAUD UART_BAUDRATE
#define BAUD_TOL UART_BAUD_TOL
#include <util/setbaud.h>
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
#if USE_2X
    UCSR0A |= BIT(U2X0);
#else
    UCSR0A &= ~BIT(U2X0);
#endif

#undef BAUD
#undef BAUD_TOL

    /* Set parity, stop bits (one), and character size (eight) */
    UCSR0C = BIT(UCSZ01) | BIT(UCSZ00);
    if (UART_PARITY == UART_PARITY_EVEN) {
        UCSR0C |= BIT(UPM01);
    }
    else if (UART_PARITY == UART_PARITY_ODD) {
        UCSR0C |= BIT(UPM01) | BIT(UPM00);
    }

    uart_enable();

#ifdef _STDIO_H_
    /* USART0 becomes stdin and stdout with avr-libc */
    stdout = &uart0;
    stdin = &uart0;
    stderr = &uart0;
#endif
}


void uart_disable(void)
{
    /* Disable UART interrupt, and UART hardware */
    UCSR0B &= ~(BIT(RXCIE0) | BIT(RXEN0) | BIT(TXEN0));
}


void uart_enable(void)
{
#ifdef UART_USE_CALLBACK
    /* Enable RX interrupt if application wants it */
    UCSR0B |= BIT(RXCIE0) | BIT(RXEN0) | BIT(TXEN0);
    enableInterrupts();
#else
    UCSR0B |= BIT(RXEN0) | BIT(TXEN0);
#endif
}


uint8_t uart_putchar(uint8_t ch)
{
/* Insert carriage return with newline */
#ifndef UART_DO_NOT_INSERT_RETURN
   //if (ch == '\n')
     //   putchar('\r');
#endif

    UDR0 = ch;
    while ( !(UCSR0A & BIT(UDRE0)) )
        ;
    return ch;
}


uint8_t uart_getchar(void)
{
    while ( !(UCSR0A & BIT(RXC0)) )
        ;
    return UDR0;
}


/******************************************************************************\
 * Local functions.
\******************************************************************************/


#if defined (_STDIO_H_)
/*
 * Internal function to send single char over USART0, and conforms to function
 * type expected by AVR-libc stdio system.
 */
static int write(char ch, FILE __attribute__((unused)) *unused)
{
    return uart_putchar(ch);
}


/*
 * Internal function receives single char over USART0, and returns it, and
 * conforms to function type expected by AVR-libc stdio system.
 */
static int read(FILE __attribute__((unused)) *unused)
{
    return uart_getchar();
}
#endif

/**
 * Interrupt service routine for RXCIE0. Only used if @c UART_USE_CALLBACK
 * is defined.
 **/
#if defined(UART_USE_CALLBACK) || defined (__DOXYGEN__)
ISR(USART0_RX_vect)
{
    uart_callback(UDR0);
}
#endif
