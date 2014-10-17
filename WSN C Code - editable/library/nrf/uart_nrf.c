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
 * UART functions.
 * @todo Add support for multiple UART baudrates.
 *
 * @file uart_nrf.c
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#include "global.h"
#include "uart.h"


/******************************************************************************\
 * See uart.h for documentation of these functions.
\******************************************************************************/

void uart_init(void)
{
    /*
     * Initialises UART to baud rate of F_CPU/32 bps. 8 data bits, 1 parity bit
     * (space), 1 start bit, and 1 stop bit. No handshaking.
     */
    SM0 = 1;
    PCON |= BIT(SMOD);

    /* Set up pins */
    P0_ALT |= BIT(RXD) | BIT(TXD);
    P0_DIR |= BIT(RXD);

    uart_enable();
}


void uart_disable(void)
{
    REN = 0;

#ifdef UART_USE_CALLBACK
    ES = 0;
#endif
}


void uart_enable(void)
{
    REN = 1;

#ifdef UART_USE_CALLBACK
    ES = 1;
    enableInterrupts();
#endif
}


uint8_t uart_putchar(uint8_t ch)
{
/* Insert carriage return with newline */
#ifndef UART_DO_NOT_INSERT_RETURN
    if (ch == '\n') {
        uart_putchar('\r');
    }
#endif

#ifdef UART_USE_CALLBACK
    ES = 0;         /* Turn off UART interrupt for transmission */
#endif
    SBUF = ch;
    while(!TI) {
        ;           /* Wait until character is sent */
    }
    TI = 0;
#ifdef UART_USE_CALLBACK
    ES = 1;         /* Turn UART interrupt back on */
#endif
    return ch;
}


uint8_t uart_getchar(void)
{
    while(!RI) {
        ;               /* Wait until a character is recieved */
    }
    RI = 0;
    return SBUF;
}


#if defined(UART_USE_CALLBACK) || defined(__DOXYGEN__)
ISR(RI_int)
{
    if (RI == 1) {
        RI = 0;
        uart_callback(SBUF);
    }
}
#endif
