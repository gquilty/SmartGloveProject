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
 * Function for entering sleep mode. There are four modes in order of decreasing
 * power consumption (and increasing time to wakeup):
 *  - @c SLEEP_IDLE.
 *  - @c SLEEP_LIGHT.
 *  - @c SLEEP_MODERATE.
 *  - @c SLEEP_DEEP.
 *  - @c SLEEP_POWERDOWN.
 *
 * These correspond to the following AVR sleep modes:
 *  - @c SLEEP_AVR_IDLE.
 *  - @c SLEEP_AVR_EXT_STANDBY.
 *  - @c SLEEP_AVR_STANDBY.
 *  - @c SLEEP_AVR_PWR_SAVE.
 *  - @c SLEEP_AVR_PWR_DOWN.
 *
 * and the following nrf9e5 sleep modes:
 *  - @c SLEEP_NRF_IDLE.
 *  - @c SLEEP_NRF_LIGHT.
 *  - @c SLEEP_NRF_MODERATE.
 *  - @c SLEEP_NRF_STANDBY.
 *  - @c SLEEP_NRF_DEEP.
 *
 * @file sleep.h
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#ifndef SLEEP_H
#define SLEEP_H


#define SLEEP_IDLE                  0
#define SLEEP_LIGHT                 1
#define SLEEP_MODERATE              2
#define SLEEP_DEEP                  3
#define SLEEP_POWERDOWN             4


/* Processor specific definitions. */
#if defined (UC_AVR)
#   define SLEEP_AVR_IDLE           SLEEP_IDLE
#   define SLEEP_AVR_EXT_STANDBY    SLEEP_LIGHT
#   define SLEEP_AVR_STANDBY        SLEEP_MODERATE
#   define SLEEP_AVR_PWR_SAVE       SLEEP_DEEP
#   define SLEEP_AVR_PWR_DOWN       SLEEP_POWERDOWN

#elif defined (UC_8051)
#   define SLEEP_NRF_IDLE           SLEEP_IDLE
#   define SLEEP_NRF_LIGHT          SLEEP_LIGHT
#   define SLEEP_NRF_MODERATE       SLEEP_MODERATE
#   define SLEEP_NRF_STANDBY        SLEEP_DEEP
#   define SLEEP_NRF_DEEP           SLEEP_POWERDOWN

#endif


/**
 * Puts the system into sleep mode.
 *
 * @param mode which sleep mode to use.
 **/
void sleep(uint8_t mode);


#endif
