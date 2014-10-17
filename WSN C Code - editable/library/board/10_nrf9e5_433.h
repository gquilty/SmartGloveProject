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
 * Board definitions for 433MHz Tyndall 10mm nRF9E5 module.
 *
 * @file 10_nrf9e5_433.h
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#ifndef _10_NRF9E5_433_H
#define _10_NRF9E5_433_H


/** Definition so compiler can determine what microcontroller is being used. */
#define UC_8051
/** Definition so compiler can determine what radio is being used. */
#define RADIO_NRF9E5

/** LED Pin. **/
#define LED_0                   0

/** Pin connected to voltage regualtor enable for sensors. **/
#define SENSORS_PWR_PIN         6

/** Frequency band: 433MHz **/
#define HFREQ_PLL_VALUE         HFREQ_PLL_433

/** Register value to give 31.25ms tick rate for CKLF. **/
#define TICK_DV_VAL             124

#if (F_CPU == 20000000UL)
/** Register values controlling clock **/
#define CKLFCON_VAL         ((XO_DIRECT_XOF << XO_DIRECT) | (XOF_20MHZ << XOF))
#elif (F_CPU == 4000000UL)
#define CKLFCON_VAL         (XOF_20MHZ << XOF)
#elif (F_CPU == 2000000UL)
#define CKLFCON_VAL         ((UP_CLK_FREQ_2MHZ << UP_CLK_FREQ) | (XOF_20MHZ << XOF))
#elif (F_CPU == 1000000UL)
#define CKLFCON_VAL         ((UP_CLK_FREQ_1MHZ << UP_CLK_FREQ) | (XOF_20MHZ << XOF))
#elif (F_CPU == 500000UL)
#define CKLFCON_VAL         ((UP_CLK_FREQ_500KHZ << UP_CLK_FREQ) | (XOF_20MHZ << XOF))
#else
#error "INVALID F_CPU value"
#endif


/** Turn on voltage regulator that powers sensors. **/
#define sensors_enable()        (P0 |= BIT(SENSORS_PWR_PIN))

/** Turn off voltage regulator that powers sensors. **/
#define sensors_disable()       (P0 &= ~BIT(SENSORS_PWR_PIN))

/**
 * Set pins to initial state (minimum current leakeage).
 * Pin0 = LED, Pins 3 and 5 = I2C.
 **/
#define board_init()            do { \
  P0 = BIT(0) | BIT(3) | BIT(5); \
  P0_DIR = 0; \
  CKCON = BIT(T0M) | BIT(T1M) | BIT(T2M); \
  CKLFCON = CKLFCON_VAL; \
  SPICLK = 0; \
  spi_selectInternal(); \
  spi_enableCsn(); \
  SPI_WRITE_BYTE(W_RF_CONFIG | 9); \
  SPI_WRITE_BYTE(CKLFCON_VAL | BIT(CRC_EN) | (CRC_MODE_16BIT << CRC_MODE)); \
  spi_disableCsn(); \
  TICK_DV = TICK_DV_VAL; \
} while (0)


/** This board does not have hardware support for I2C. **/
#define BOARD_HAS_NO_HARDWARE_I2C

/** Lowest power uC sleep mode that can be woken up by the radio. **/
#define RF_SLEEP_MODE           SLEEP_NRF_IDLE

/** Lowest power uC sleep mode that can be woken up by "second" delays. **/
#define DELAY_S_SLEEP_MODE      SLEEP_NRF_DEEP

/** Lowest power uC sleep mode that can be woken up by "millisecond" delays. **/
#define DELAY_MS_SLEEP_MODE     SLEEP_NRF_IDLE


#endif 
