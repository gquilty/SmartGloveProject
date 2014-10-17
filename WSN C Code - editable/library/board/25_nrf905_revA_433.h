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
 * Defines pins used in Tyndall 25mm nRF905 module to connect ATmega128 to
 * nRF905, and also the pins that are connected to LEDs. Also has macros for
 * managing interrupt caused when nRF905 receives a packet. This is for revA
 * of the 433MHz nRF905 board.
 *
 * @file 25_nrf905_revA_433.h
 * @date 15-Jan-2010
 * @author Seán Harte, Pádraig Curran
 ******************************************************************************/


#ifndef _25_NRF905_REVA_433_H
#define _25_NRF905_REVA_433_H


/** Operating in 433MHz band. **/
#define HFREQ_PLL_VALUE 0

/** Definition so compiler can determine what microcontroller is being used. **/
#define UC_AVR

/** Definition so compiler can determine what radio is being used. **/
#define RADIO_NRF905


/*------------------------------------------------------------------------------
 * Pins connected to nRF905.
 */
 
/** TRX_CE to nRF905 (PORTD.4). **/
#define RF_TRX_CE_PORT      PORTD
#define RF_TRX_CE_DDR       DDRD
#define RF_TRX_CE           4

/** PWR_UP to nRF905 (PORTB.5). **/
#define RF_PWR_UP_PORT      PORTB
#define RF_PWR_UP_DDR       DDRB
#define RF_PWR_UP           5

/** TX_EN to nRF905 (PORTD.5). **/
#define RF_TX_EN_PORT       PORTD
#define RF_TX_EN_DDR        DDRD
#define RF_TX_EN            5

/** CD from nRF905 (PORTE.5). **/
#define RF_CD_PORT          PORTE
#define RF_CD_DDR           DDRE
#define RF_CD_PIN           PINE
#define RF_CD               5

/** AM from nRF905 (PORTE.4). **/
#define RF_AM_PORT          PORTE
#define RF_AM_DDR           DDRE
#define RF_AM_PIN           PINE
#define RF_AM               4

/* DR from nRF905 (PORTD.7). **/
#define RF_DR_PORT          PORTD
#define RF_DR_DDR           DDRD
#define RF_DR_PIN           PIND
#define RF_DR               7


/*------------------------------------------------------------------------------
 * External interrupts from nRF905
 */
 
/** Rising edge trigger for external interrupt 4 (AM pin on nRF905) **/
#define rf_initInterrupt() do {       \
    EICRB |= BIT(ISC40) | BIT(ISC41); \
    rf_clearInterrupt();              \
} while (0)

/** Enable radio interrupt (AM). **/
#define rf_enableInterrupt()    (EIMSK |= BIT(INT4))

/** Disable radio interrupt (AM). **/
#define rf_disableInterrupt()   (EIMSK &= ~BIT(INT4))

/** Reset radio interrupt (AM). **/
#define rf_clearInterrupt()     (EIFR = BIT(INTF4))

/** Radio interrupt address. **/
#define RF_INT_VECTOR       INT4_vect

/** Which pin is interrupt connected to. **/
#define RF_INTERRUPT_AM


/** Lowest power mode from which the nRF905 can wake up the ATmega128. **/
#define RF_SLEEP_MODE       SLEEP_IDLE

/** Lowest power mode from which second (the time unit) timer can wakeup uC. **/
#define DELAY_S_SLEEP_MODE  SLEEP_AVR_PWR_SAVE

/*------------------------------------------------------------------------------
 * LEDs.
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
 *  - PORTA.2 is active low sensor power enable on USB programming board.
 *  - PORTB.0 is active low SPI radio enable.
 *  - PORTB.6 is active high battery monitoring circuit enable (Low by default).
 *  - PORTC.1 is active low sensor power enable.
 *  - PORTD.0 and PORTD.1 are I2C pins with pullups.
 *  - PORTE.1 is UART RX.
 *  - PORTG only has 5 pins.
 **/
#define board_init() do { \
    PORTA = BIT(2); \
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
