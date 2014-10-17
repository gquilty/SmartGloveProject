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
 * Functions for LEDs. LED_PORT etc. should be defined in the board header.
 * @note An easy way to disable LEDs is by defining LED_NOT_USED.
 *
 * @file led.h
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#ifndef LED_H
#define LED_H


#if defined (LED_NOT_USED)
#   define led_init()
#   define led_disable(n)
#   define led_enable(n)
#   define led_toggle(n)


/******************************************************************************\
 * LEDs on 25mm boards turn on when the line goes high.
\******************************************************************************/

#elif defined (UC_AVR)

/** Initialise pins connected to LEDs. **/
FORCE_INLINE
static inline void led_init(void)
{
    LED_DDR |= LED_MASK;
    LED_PORT &= ~LED_MASK;
}


/**
 * Turn off specific LED.
 *
 * @param n which LED to modify.
 **/
FORCE_INLINE
static inline void led_disable(uint8_t n)
{
    LED_PORT &= ~BIT(n);
}


/**
 * Turn on specific LED.
 *
 * @param n which LED to modify.
 **/
FORCE_INLINE
static inline void led_enable(uint8_t n)
{
    LED_PORT |= BIT(n);
}


/**
 * Toggle specific LED.
 *
 * @param n which LED to modify.
 **/
FORCE_INLINE
static inline void led_toggle(uint8_t n)
{
    LED_PORT ^= BIT(n);
}


/******************************************************************************\
 * LED on 10mm boards turn on when the line goes low.
\******************************************************************************/

#else


static inline void led_init(void)
{
    P0 |= BIT(LED_0);
    P0_DIR &= ~BIT(LED_0);
}


static inline void led_disable(uint8_t n)
{
    P0 |= BIT(LED_0);
}


static inline void led_enable(uint8_t n)
{
    P0 &= ~BIT(LED_0);
}


static inline void led_toggle(uint8_t n)
{
    P0 ^= BIT(LED_0);
}


#endif


#endif
