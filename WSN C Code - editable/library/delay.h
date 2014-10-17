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
 * Functions for generating delays. Delay functions with units of microseconds,
 * milliseconds, and seconds are defined.
 *
 * For the millisecond and second delays two function types are available. The
 * first type is a blocking function. When this is called, it will not return
 * until the delay is over. The delay function will put the microcontroller into
 * a low power state to save energy.
 *
 * In the callback mode, the function will return immediately. Then, when the
 * delay period is over, the function @c delay_x_callback() will be called
 * periodically.
 *
 * To choose callback mode, the preprocessor definition @c DELAY_USE_CALLBACK_S
 * or @c DELAY_USE_CALLBACK_MS should be defined. Only one type of function can
 * be used at a time, as the same hardware timers are used.
 *
 * The blocking type function can be used to generate exact delays between some
 * events. The callback can be used when doing something with an exact period
 * or duty cycle is needed.
 *
 * File contents:
 * @code
 *     void delay_ms(uint16_t pause);
 *
 *     #ifndef DELAY_USE_CALLBACK_MS
 *         void delay_ms(uint16_t pause);
 *     #else
 *         void delay_ms_withCallback(uint16_t pause);
 *         uint16_t delay_callback_ms(uint16_t pause);
 *         bool delay_ms_isRunning(void);
 *     #endif
 *
 *     #ifndef DELAY_USE_CALLBACK_S
 *         void delay_s(uint16_t pause);
 *     #else
 *         void delay_s_withCallback(uint16_t pause);
 *         void delay_callback_s(void);
 *         bool delay_s_isRunning(void);
 *     #endif
 *
 *     void delay_s_setSleepMode(uint8_t sleepMode);
 * @endcode
 *
 * @file delay.h
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#ifndef DELAY_H
#define DELAY_H


#if defined (UC_AVR)
#   include <util/delay.h>
#   define delay_us _delay_us   /* Just use AVR LibC function */
#elif defined (UC_8051)
#   include "nrf/delay_nrf.h"
#endif


#ifndef DELAY_USE_CALLBACK_MS
void delay_ms(uint16_t pause);
#else
void delay_ms_withCallback(uint16_t pause);
uint16_t delay_callback_ms(uint16_t pause);
bool delay_ms_isRunning(void);
#endif


#ifndef DELAY_USE_CALLBACK_S
void delay_s(uint16_t pause);
#else
void delay_s_withCallback(uint16_t pause);
void delay_callback_s(void);
bool delay_s_isRunning(void);
#endif


/**
 * Sets the sleep mode that will be used for the second delay. Lower power sleep
 * modes may be less accurate.
 *
 * @param sleepMode sleep mode to use (see @e sleep.h).
 **/
void delay_s_setSleepMode(uint8_t sleepMode);


#if defined (__DOXYGEN__)
/**
 * Delay for a number of microseconds. Based on loops. @c F_CPU must be
 * accurately defined (usually done in @e makefile).
 *
 * @param pause how many microseconds to delay. This MUST be a compile-time
 *     constant.
 **/
void delay_us(uint16_t pause);


/**
 * Delay for a number of milliseconds. The function will return when the delay
 * is over.
 *
 * @note if @c DELAY_USE_CALLBACK_MS is defined, this function will not be
 *     available. @c delay_ms_withCallback() will be.
 *
 * @param pause how many milliseconds to delay. A value of 0 will stop a
 *     currently running timer.
 **/
void delay_ms(uint16_t pause);


/**
 * Delay for a number of milliseconds. Based on timer. The function
 * @c delay_callback_ms() will be called  with a period of @c pause
 * milliseconds.
 *
 * @note if @c DELAY_USE_CALLBACK_MS is not defined, this function will not be
 *     available. @c delay_ms() will be.
 *
 * @param pause how many milliseconds to delay. A value of 0 will stop a
 *     currently running timer.
 **/
void delay_ms_withCallback(uint16_t pause);


/**
 * This function is signalled when a a delay is finished. It should be defined
 * by the application. Do not try to start a new millisecond timer from within
 * the callback function. Strange things could happen! Use the return value
 * instead.
 *
 * @note if @c DELAY_USE_CALLBACK_MS is not defined, this function will not be
 *     available.
 *
 * @param pause how long since the function was called, in milliseconds.
 *
 * @return the application can change the delay length. The return value will
 *     be the delay before the next time this function is called. If 0, the
 *     function will not be called again.
 **/
uint16_t delay_callback_ms(uint16_t pause);


/**
 * Returns the state of the millisecond delay.
 *
 * @note if @c DELAY_USE_CALLBACK_MS is not defined, this function will not be
 *     available.
 *
 * @return @c true if the timer is running, otherwise @c false.
 **/
bool delay_ms_isRunning(void);


/**
 * Delay for a number of seconds. The function will return when the delay is
 * over.
 *
 * @note if @c DELAY_USE_CALLBACK_S is defined, this function will not be
 *     available. @c delay_s_withCallback() will be.
 *
 * @param pause how many seconds to delay. A value of 0 will stop a currently
 *     running timer.
 **/
void delay_s(uint16_t pause);

/**
 * Delay for a number of seconds. Based on timer. The function
 * @c delay_callback_s() will be called  with a period of @c pause seconds.
 *
 * @note if @c DELAY_USE_CALLBACK_S is not defined, this function will not be
 *     available. @c delay_s() will be.
 *
 * @param pause how many mseconds to delay. A value of 0 will stop a
 *     currently running timer.
 **/
void delay_s_withCallback(uint16_t pause);


/**
 * This function is signalled when a a delay is finished. It should be defined
 * by the application. Do not try to start a new delay from within the callback
 * function! Also, due to the way the hardware timers are configured it is not
 * possible to change the delay value as with the millisecond timer. If you
 * want a different delay, stop the timer, and start a new one, from the normal
 * execution context.
 *
 * @note if @c DELAY_USE_CALLBACK_S is not defined, this function will not be
 *     available.
 **/
void delay_callback_s(void);


/**
 * Returns the state of the second delay.
 *
 * @note if @c DELAY_USE_CALLBACK_S is not defined, this function will not be
 *     available.
 *
 * @return @c true if the timer is running, otherwise @c false.
 **/
bool delay_s_isRunning(void);
#endif

#endif
