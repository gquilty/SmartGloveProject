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
 * Defines pins used in revA of Tyndall 25mm Zigbee module.
 *
 * @file 25_2420_revA.h
 * @date 15-Jan-2010
 * @author Seán Harte, Pádraig Curran
 ******************************************************************************/


#ifndef _25_2420_REVA_H
#define _25_2420_REVA_H


/** Definition so compiler can determine what microcontroller is being used. **/
#define UC_AVR

/** Definition so compiler can determine what radio is being used. **/
#define RADIO_2420

/** TOSC1 is not connected to 32.768kHz oscillator. **/
#define BOARD_HAS_NO_TOSC1_CRYSTAL

/** Hardware I2C cannot be used as the pins are connected to the radio. **/
#define BOARD_HAS_NO_HARDWARE_I2C


/*------------------------------------------------------------------------------
 * Pins connected to CC2420.
 */

/** VREG pin to CC2420, PortB, pin5. **/
#define RF_VREG_PORT        PORTB
#define RF_VREG_DDR         DDRB
#define RF_VREG_EN          5

/** RESETN pin to CC2420, PortB, pin6.  **/
#define RF_RESETN_PORT      PORTB
#define RF_RESETN_DDR       DDRB
#define RF_RESETN           6

/** FIFOP pin from CC2420, PortD, pin0.  **/
#define RF_FIFOP_PORT       PORTD
#define RF_FIFOP_DDR        DDRD
#define RF_FIFOP_PIN        PIND
#define RF_FIFOP            0

/** FIFO pin from CC2420, PortD, pin1. **/
#define RF_FIFO_PORT        PORTD
#define RF_FIFO_DDR         DDRD
#define RF_FIFO_PIN         PIND
#define RF_FIFO             1

/** SFD pin from CC2420, PortD, pin4. **/
#define RF_SFD_PORT         PORTD
#define RF_SFD_DDR          DDRD
#define RF_SFD_PIN          PIND
#define RF_SFD              4

/** CCA pin from CC2420, PortD, pin6. **/
#define RF_CCA_PORT         PORTD
#define RF_CCA_DDR          DDRD
#define RF_CCA_PIN          PIND
#define RF_CCA              6


/*------------------------------------------------------------------------------
 * External interrupts from CC2420
 */
 
/** Rising edge trigger for external interrupt 0 (FIFOP). **/
#define rf_initInterrupt() do {       \
    EICRA |= BIT(ISC00) | BIT(ISC01); \
    rf_clearInterrupt();              \
} while (0)

/** Enable radio interrupt (FIFOP). **/
#define rf_enableInterrupt()    (EIMSK |= BIT(INT0))

/** Disable radio interrupt (FIFOP). **/
#define rf_disableInterrupt()   (EIMSK &= ~BIT(INT0))

/** Reset radio interrupt (FIFOP). **/
#define rf_clearInterrupt()     (EIFR = BIT(INTF0))

/** Radio interrupt address. **/
#define RF_INT_VECTOR       INT0_vect


/** Lowest power mode from which the CC2420 can wake up the ATmega128. **/
#define RF_SLEEP_MODE       SLEEP_AVR_PWR_DOWN

/** Lowest power mode from which second (the time unit) timer can wakeup uC. **/
#define DELAY_S_SLEEP_MODE  SLEEP_IDLE

/*------------------------------------------------------------------------------
 * LEDs
 */
 
/** LED pins: PortG, pin 0 and pin 1. **/
#define LED_PORT            PORTG
#define LED_DDR             DDRG
#define LED_PIN             PING
#define LED_1               0
#define LED_0               1       /* LED closest to edge of board */
#define LED_MASK            0x03


/**
 * Initialise pins to a low power state. This should be the first thing done in
 * every application. Depending on sensors attached (if they have active low
 * enable) a customised version of this can be defined by the application.
 *  - PORTB, pin 0 is SPI active low chip enable.
 *  - PORTC, pin 1 is active low sensor power enable.
 *  - PORTD, pins 0 and 1 are I2C pins with pullups.
 *  - PORTE, pin 1 is UART RX.
 *  - PORTG only has 5 pins.
 **/
#define board_init() do { \
    PORTA = 0x00; \
    DDRA  = 0xFF; \
    PORTB = BIT(0); \
    DDRB  = 0xFF; \
    PORTC = BIT(1); \
    DDRC  = 0xFF; \
    PORTD = BIT(0) | BIT(1); \
    DDRD  = ~(BIT(0) | BIT(1)); \
    PORTE = BIT(1); \
    DDRE  = ~BIT(1); \
    PORTF = 0x00; \
    DDRF  = 0xFF; \
    PORTG = 0x00; \
    DDRG  = 0x1F; \
} while (0)


#endif
