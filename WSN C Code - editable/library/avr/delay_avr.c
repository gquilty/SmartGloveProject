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
 * Implementation of delay functions for Atmel AVR microcontrollers. This should
 * work for most ATmega devices, except Atmega128.
 *
 * Timer2 is used for second delays, and Timer0 is used for millisecond delays.
 *
 * @file delay_avr.c
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#include "global.h"
#include "delay.h"
#include "sleep.h"

/* Prescalar values for Timer/Counter0. */
#define PRESCALE_T0_NO_CLOCK    0
#define PRESCALE_T0_DIV_1       (BIT(CS00))
#define PRESCALE_T0_DIV_8       (BIT(CS01))
#define PRESCALE_T0_DIV_64      (BIT(CS01) | BIT(CS00))
#define PRESCALE_T0_DIV_256     (BIT(CS02))
#define PRESCALE_T0_DIV_1024    (BIT(CS02) | BIT(CS00))
#define PRESCALE_T0_BITMASK     (BIT(CS02) | BIT(CS01) | BIT(CS00))

/* Prescalar values for Timer/Counter2. */
#define PRESCALE_T2_NO_CLOCK    0
#define PRESCALE_T2_DIV_1       (BIT(CS20))
#define PRESCALE_T2_DIV_8       (BIT(CS21))
#define PRESCALE_T2_DIV_32      (BIT(CS21) | BIT(CS20))
#define PRESCALE_T2_DIV_64      (BIT(CS22))
#define PRESCALE_T2_DIV_128     (BIT(CS22) | BIT(CS20))
#define PRESCALE_T2_DIV_256     (BIT(CS22) | BIT(CS21))
#define PRESCALE_T2_DIV_1024    (BIT(CS22) | BIT(CS21) | BIT(CS20))
#define PRESCALE_T2_BITMASK     (BIT(CS22) | BIT(CS21) | BIT(CS20))


/* Used as index for local variables below. */
#define TIMER_SECOND        0
#define TIMER_MILLISECOND   1


/* Counts how many time interrupt is signalled. */
static volatile uint16_t timerInterruptCount[2];

/* Period of continuous delay. */
static volatile uint16_t timerPause[2];

/* Flags to show if interrupt is executing. */
static volatile bool isTimerInt[2];

static uint8_t s_sleepMode = DELAY_S_SLEEP_MODE;

static void timerDelay(uint16_t pause, uint8_t timer);
static void waitForTimer(uint16_t pause, uint8_t timer, uint8_t sleepMode);
static void startTimer(uint8_t timer);
static void startTimerS(void);
static void startTimerMS(void);
static void stopTimer(uint8_t timer);


/******************************************************************************\
 * See delay.h for documentation of these functions.
\******************************************************************************/

void delay_s_setSleepMode(uint8_t sleepMode)
{
    if (sleepMode > DELAY_S_SLEEP_MODE) {
        sleepMode = DELAY_S_SLEEP_MODE;
    }
    s_sleepMode = sleepMode;
}


#ifndef DELAY_USE_CALLBACK_MS
void delay_ms(uint16_t pause)
{
    timerDelay(pause, TIMER_MILLISECOND);
}
#else
void delay_ms_withCallback(uint16_t pause)
{
    return timerDelay(pause, TIMER_MILLISECOND);
}
bool delay_ms_isRunning(void)
{
    return (timerPause[TIMER_MILLISECOND] != 0);
}
#endif


#ifndef DELAY_USE_CALLBACK_S
void delay_s(uint16_t pause)
{
    timerDelay(pause, TIMER_SECOND);
}
#else
void delay_s_withCallback(uint16_t pause)
{
    return timerDelay(pause, TIMER_SECOND);
}
bool delay_s_isRunning(void)
{
    return (timerPause[TIMER_SECOND] != 0);
}
#endif

/* This is just to generate correct documentation */
#ifdef __DOXYGEN__
void delay_ms_withCallback(uint16_t pause) { }
bool delay_ms_isRunning(void) { }
void delay_s_withCallback(uint16_t pause) { }
bool delay_s_isRunning(void) { }
#endif

/******************************************************************************\
 * Functions used only in this file.
\******************************************************************************/

/**
 * Delay for a number of (milli)seconds. If @c DELAY_USE_CALLBACK_XX is not
 * defined, the function will not return until the delay is over. Otherwise, the
 * function @c delay_callback_xx() will be called with a period of "pause"
 * (milli)seconds.
 *
 * The seconds and milliseconds timer are separate and can run at the same time.
 *
 * @param pause how many (milli)seconds to delay.
 * @param timer @c TIMER_SECOND, or @c TIMER_MILLISECOND.
 **/
static void timerDelay(uint16_t pause, uint8_t timer)
{
    if (isTimerInt[timer] == 1) {
        return;
    }

    if (pause == 0) {
        stopTimer(timer);
        return;
    }

    /* Store variables for access by other functions */
    timerPause[timer] = pause;

    startTimer(timer);

#ifndef DELAY_USE_CALLBACK_S
    if (timer == TIMER_SECOND) {
        waitForTimer(pause, timer, s_sleepMode);
    }
#endif

#ifndef DELAY_USE_CALLBACK_MS
    if (timer == TIMER_MILLISECOND) {
        waitForTimer(pause, timer, SLEEP_AVR_IDLE);
    }
#endif
}


/**
 * Waits for pause to finish.
 *
 * @param pause how long to wait.
 * @param timer @c TIMER_SECOND, or @c TIMER_MILLISECOND.
 * @param sleepMode sleep mode to use while waiting.
 **/
static void waitForTimer(uint16_t pause, uint8_t timer, uint8_t sleepMode)
{
    uint16_t timerCount;
    
    /* Wait... */
    do {
        sleep(sleepMode);
        disableInterrupts();
        timerCount = timerInterruptCount[timer];
        enableInterrupts();
    } while (timerCount < pause);

    stopTimer(timer);
}


/**
 * Starts a timer.
 *
 * @param timer @c TIMER_SECOND or @c TIMER_MILLISECOND.
 **/
static void startTimer(uint8_t timer)
{
    /* Stop any existing timer */
    stopTimer(timer);
    
    /* Start new timer */
    switch (timer) {
    case TIMER_SECOND:      startTimerS();
                            break;
    case TIMER_MILLISECOND: startTimerMS();
                            break;
    }
    
    /* Make sure interrupts are enabled */
    enableInterrupts();
}


/**
 * Starts Timer2, into asynchronous mode. An output compare match interrupt is
 * generated every 8 second. Resets interrupt counter for Timer2.
 **/
static void startTimerS(void)
{
    /* Use asynchronous 32.768Hz clock */
    ASSR |= BIT(AS2);

    /* Start timer/counter with 32Hz clock (32768/1024) */
    TCCR2B = PRESCALE_T2_DIV_1024;

    /*
     * Set output compare value. The timer will overflow every 8 seconds. To
     * be able to delay in units of 1 second it is neccessary to use output
     * compare match. This takes delay mod 8, and scales it to the right size
     * for a n second delay.
     *
     * Note that for multiples of 8 the minus 1 will give 255.
     */
    OCR2A = ((timerPause[TIMER_SECOND] & 7) * 32) - 1;

    /* Reset counters */
    TCNT2 = 0;
    timerInterruptCount[TIMER_SECOND] = 0;

    /*
     * There are 8 seconds per overflow, so delay counter is divided by 8
     * 1 to 8 = 0
     * 9 to 16 = 1
     * etc.
     */
    timerPause[TIMER_SECOND] -= 1;
    timerPause[TIMER_SECOND] /= 8;

    /* Wait until changes are made */
    while (ASSR & BIT(TCN2UB))
        ;

    /* Enable output compare match interrupt */
    TIMSK2 |= BIT(OCIE2A);
}


/**
 * Starts Timer0. An output compare match interrupt is generated every
 * millisecond. Resets interrupt counter for Timer0.
 **/
static void startTimerMS(void)
{
    /*
     * Timer delay calculations at 8MHz:
     *      (1/8000000) * 64) = 0.008ms per clock tick
     *      8000000 / 64000 = 125 clock ticks before reset
     *      0.008ms * 125 = 1ms between resets
     */

    /* Start timer in Clear Timer on Compare (CTC) mode */
    TCCR0A = BIT(WGM01);
    TCCR0B = PRESCALE_T0_DIV_64;

    /* Set output compare value */
    OCR0A = (F_CPU / 64000) - 1;

    /* Reset counters */
    TCNT0 = 0;
    timerInterruptCount[TIMER_MILLISECOND] = 0;

    /* Enable output compare match interrupt */
    TIMSK0 |= BIT(OCIE0A);
}


/**
 * Stops a timer by disabling its clock source, and interrupt.
 *
 * @param timer @c TIMER_SECOND or @c TIMER_MILLISECOND.
 **/
static void stopTimer(uint8_t timer)
{
    switch (timer) {
    case TIMER_SECOND:      TIMSK2  &= ~BIT(OCIE2A);
                            TCCR2B &= ~PRESCALE_T2_BITMASK;
                            break;
    case TIMER_MILLISECOND: TIMSK0 &= ~BIT(OCIE0A);
                            TCCR0B &= ~PRESCALE_T0_BITMASK;
                            break;
    }
}


/*------------------------------------------------------------------------------
 * Interrupt routines.
 */
/* ISR is signalled every 8 seconds. */
ISR(TIMER2_COMPA_vect)
{
    TCCR2B = PRESCALE_T2_DIV_1024;

    timerInterruptCount[TIMER_SECOND]++;
#ifdef DELAY_USE_CALLBACK_S
    if (timerInterruptCount[TIMER_SECOND] > timerPause[TIMER_SECOND]) {
        isTimerInt[TIMER_SECOND] = 1;
        /* Call upper layer */
        delay_callback_s();
        timerInterruptCount[TIMER_SECOND] = 0;
        TCNT2 = 0;
        isTimerInt[TIMER_SECOND] = 0;
    }
#endif
    /* Use update busy flag to know when interrupt logic is reset */
    while (ASSR & BIT(TCR2AUB)) {
        ;
    }
}

/* ISR is signalled every millisecond. */
ISR(TIMER0_COMPA_vect)
{
    timerInterruptCount[TIMER_MILLISECOND]++;
#ifdef DELAY_USE_CALLBACK_MS
    if (timerInterruptCount[TIMER_MILLISECOND] >= timerPause[TIMER_MILLISECOND]) {
        isTimerInt[TIMER_MILLISECOND] = 1;
        /* Call upper layer */
        timerPause[TIMER_MILLISECOND] = delay_callback_ms(timerPause[TIMER_MILLISECOND]);

        if (timerPause[TIMER_MILLISECOND] == 0) {
            stopTimer(TIMER_MILLISECOND);
        }
        timerInterruptCount[TIMER_MILLISECOND] = 0;
        isTimerInt[TIMER_MILLISECOND] = 0;
    }
#endif
}
