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
 * Functions for generating millisecond and second delays. The millisecond
 * delays are generated using Timer0. The second delays are using the RTC timer.
 *
 * This file also contains code for managing sleep modes on the nRF9E5. This is
 * put here as they rely on similar parts of the nRF9E5 hardware.
 *
 * @file delay_nrf.c
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#include "global.h"
#include "delay.h"
#include "sleep.h"
#include "nrf_regx.h"

/**
 * Every time 16bit Timer0 overflows, this value will be loaded into the Timer0
 * registers. This will give a delay of 1 millisecond per overflow, if Timer0 is
 * clocked by @c F_CPU/4.
 * The values added are because the delay was found to be slightly too long when
 * measured with an oscilloscope.
 **/
#define timer0ReloadValH    HIGH_BYTE(UINT16_MAX - (F_CPU / 4000) + 35)
#define timer0ReloadValL    LOW_BYTE(UINT16_MAX - (F_CPU / 4000) + 35)

/**
 * Inital values loaded into the Timer0 registers. This will give a delay of 1
 * millisecond per overflow, if Timer0 is clocked by @c F_CPU/4. The values
 * added are because the delay was found to be slightly too long when measured
 * with an oscilloscope.
 **/
#define timer0LoadValH      HIGH_BYTE(UINT16_MAX - (F_CPU / 4000) + 300)
#define timer0LoadValL      LOW_BYTE(UINT16_MAX - (F_CPU / 4000) + 300)


#ifndef DELAY_USE_CALLBACK_MS
static void waitForTimer0(void);
#endif
static void startTimer0(void);
static void stopTimer0(void);
static void calibrateRtc(void);


/** How many milliseconds in delay. **/
static volatile uint16_t ms_timerPause = 0;

/** How many milliseconds have elapsed since delay began. **/
static volatile uint16_t ms_interruptCount;

/** Is the "second" timer running? **/
static volatile bool s_isTimerRunning = 0;

/** Has the "second" timer interrupt been handled? **/
static volatile __no_init __bit bool s_isInterruptHandled;

#ifndef DELAY_USE_CALLBACK_S
/**
 * Allow ability to change the sleep mode used for "second" delays.
 * @c SLEEP_NRF_STANDBY and @c SLEEP_NRF_DEEP use the internal RC oscillator to
 * control the delay. This is lower power, but less accurate.
 **/

//static uint8_t s_sleepMode = SLEEP_NRF_DEEP;
static uint8_t s_sleepMode = SLEEP_NRF_MODERATE;
#endif


/******************************************************************************\
 * See delay.h for documentation of these functions
\******************************************************************************/


#ifndef DELAY_USE_CALLBACK_MS
void delay_ms(uint16_t pause)
{
    stopTimer0();

    if (pause == 0) {
        return;
    }

    ms_timerPause = pause;       /* Store pause value */
    startTimer0();
    waitForTimer0();
}
#else
void delay_ms_withCallback(uint16_t pause)
{
    stopTimer0();

    if (pause == 0) {
        return;
    }

    ms_timerPause = pause;       /* Store pause value */
    startTimer0();

    enableInterrupts();
}


bool delay_ms_isRunning(void)
{
    return (ms_timerPause != 0);
}
#endif


#ifndef DELAY_USE_CALLBACK_S
void delay_s_setSleepMode(uint8_t sleepMode)
{
    s_sleepMode = sleepMode;
}


void delay_s(uint16_t pause)
{
    /* Stop timer */
    EWDI = 0;
    regx_write(WRTCDIS, 0);

    if (pause == 0) {
        return;
    }

    /*
     * Tick rate is 31.25ms, so pause is multiplied by 32. 2 is subtracted
     * as the timer fires at N + 2
     */
    regx_write(GTIMER, HIGH_BYTE(pause) >> 3);
    regx_write(RTCLAT, pause * 32 - 2);
    
    s_isInterruptHandled = 0;
    EWDI = 1;
    PWDI = 1;    /* High priority */
    enableInterrupts();
    
    /* Go to sleep and wait for wakeup */
    s_isTimerRunning = 1;
    sleep(s_sleepMode);
    s_isTimerRunning = 0;
    EWDI = 0;
    EICON_WDTI = 0;
    regx_write(WRTCDIS, 0);
}
#else
void delay_s_withCallback(uint16_t pause)
{
    /* Stop timer */
    EWDI = 0;
    regx_write(WRTCDIS, 0);
    s_isTimerRunning = 0;

    if (pause == 0) {
        return;
    }

    s_isTimerRunning = 1;
    /*
     * Tick rate is 31.25ms, so pause is multiplied by 32. 2 is subtracted
     * as the timer fires at N + 2
     */
    regx_write(GTIMER, HIGH_BYTE(pause) >> 3);
    regx_write(RTCLAT, pause * 32 - 2);

    s_isInterruptHandled = 0;
    EWDI = 1;
    PWDI = 1;    /* High priority */

    enableInterrupts();
}


bool delay_s_isRunning(void)
{
    return s_isTimerRunning;
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
 * See sleep.h for documentation of this function.
\******************************************************************************/

void sleep(uint8_t mode)
{
    if (mode == SLEEP_IDLE) {
        /* Just stops processing, doesn't save much power */
        PCON |= IDLE;
    }
    else {
        /* Calibrate RTC if it is being used for a clock */
        if (mode > SLEEP_NRF_MODERATE && s_isTimerRunning) {
            calibrateRtc();
        }
        else {
            /* Reset RTC to use CKLF */
            TICK_DV = TICK_DV_VAL;
        }

        /* Make sure the watchdog register isn't busy, and then set CK_CTRL */
        while (REGX_CTRL & BIT(WD_REG_BUSY)) {
            ;
        }
        CK_CTRL = mode;     /* Goes to sleep now */

        /*
         * The nRF9E5 does not guarantee that the WDTI interrupt will be
         * signalled when using the internal RC oscillator.
         */
        if (s_isTimerRunning && !s_isInterruptHandled && mode > SLEEP_MODERATE) {
            EICON_WDTI = 0;             /* Clear interrupt flag */
#ifdef DELAY_USE_CALLBACK_S
            disableInterrupts();
            delay_callback_s();
            enableInterrupts();
#endif
        }
    }
}


/******************************************************************************\
 * Functions used only in this file.
\******************************************************************************/

/**
 * Starts Timer0 with a preloaded value that will give 1ms delay until an
 * overflow occurs.
 **/
static void startTimer0(void)
{
    TR0 = 0;                    /* Stop timer0 */
    TF0 = 0;                    /* Clear any pending Timer0 interrupts */

    TMOD &= 0xF0;
    TMOD |= BIT(M00);           /* Set Timer0 to mode 1 (16bit) */

    ms_interruptCount = 0;

    TH0 = timer0LoadValH;
    TL0 = timer0LoadValL;

    ET0 = 1;                    /* Enable Timer0 interrupt */
    TR0 = 1;                    /* Start timer0 */
}


/**
 * Stops Timer0.
 **/
static void stopTimer0(void)
{
    ET0 = 0;                    /* Disable Timer0 interrupt */
    TR0 = 0;                    /* Stop timer0 */
    TF0 = 0;                    /* Clear any pending Timer0 interrupts */
}


#ifndef DELAY_USE_CALLBACK_MS
/**
 * Waits for pause to finish.
 **/
static void waitForTimer0(void)
{
    uint16_t timerCount;

    /* Wait... */
    do {
        sleep(SLEEP_NRF_IDLE);
        disableInterrupts();
        timerCount = ms_interruptCount;
        enableInterrupts();
    } while (timerCount < ms_timerPause);

    stopTimer0();
}
#endif


/**
 * Interrupt for Timer0 overflow (every millisecond).
 **/
ISR(TF0_int)
{
    disableInterrupts();
    TF0 = 0;                    /* Clear interrupt flag */
    TH0 = timer0ReloadValH;
    TL0 = timer0ReloadValL;

    ms_interruptCount++;

#ifdef DELAY_USE_CALLBACK_MS
    if (ms_interruptCount >= ms_timerPause) {
        ms_interruptCount = 0;
        /* Call upper layer */
        if (0 == (ms_timerPause = delay_callback_ms(ms_timerPause))) {
            stopTimer0();
        }
    }
#endif
    enableInterrupts();
}


/**
 * Interrupt for RTC. Note: this interrupt will not be signalled when in the
 * SLEEP_NRF_DEEP sleep mode. However, the processor will wakeup and start
 * executing from the instruction after sleep().
 **/
ISR(WDTI_int)
{
    disableInterrupts();
    s_isInterruptHandled = 1;
    EICON_WDTI = 0;             /* Clear interrupt flag */
#ifdef DELAY_USE_CALLBACK_S
    delay_callback_s();
#endif
    enableInterrupts();
	 
}


/**
 * Calibrates the RTC timer for use when it is clocked by the internal RC
 * resonator. Timer 2 is used to time the period of the resonator, and based on
 * this, the TICK_DV register is changed so that RTC timer will be as accurate
 * as possible.
 *
 * The equation to calculate TICK_DV is derived as follows:
 *
 *   @code T_tick = ( 1 + TICK_DV ) / f_cklf @endcode
 *
 * where f_cklf is the frequency of the low power oscillator.
 * f_cklf can be calculated from the diff value above:
 *
 *   @code f_cklf = F_CPU / (2 * diff) @endcode
 *
 * as Timer2 is F_CPU/4, and t2ex is f_cklf/2.
 * Putting these two equations together and rearranging for TICK_DV gives:
 *
 *   @code TICK_DV = ( ( ( T_tick * F_CPU ) - diff ) / (2 * diff) ) @endcode
 *
 * We want T_tick to be 31.25ms, so
 *
 *   @code TICK_DV = ( ( (F_CPU / 64) - diff ) / diff ) @endcode
 *
 * Normally, integer division will round the result towards 0 e.g. 7/4 = 1.
 * To make the result be rounded to the nearest integer, add half the
 * denominator to the numerator e.g. (7+2)/4 = 2.
 **/
static void calibrateRtc(void)
{
    uint16_t diff, firstVal;
    uint8_t temp;   /* To avoid warning about order of volatile accesses */

    /* Initialise Timer 2 to capture t2ex changes */
    T2CON = BIT(CP_RL2_b) | BIT(TR2_b) | BIT(EXEN2_b);

    /* Wait for capture flag to be signalled */
    while (EXF2 == 0) {
        ;
    }
    EXF2 = 0;       /* reset flag */

    /* Store capture values */
    temp = RCAP2H;
    firstVal = TO_UINT16(temp, RCAP2L);

    /* Wait for capture flag to be signalled again */
    while (EXF2 == 0) {
        ;
    }

    EXF2 = 0;       /* reset flag */
    TR2 = 0;        /* Stop timer2 */

    /* Get difference between two capture events */
    temp = RCAP2H;
    diff = (TO_UINT16(temp, RCAP2L) - firstVal);

#if (F_CPU == 20000000UL)
    /* Numerator and diviser are divided by 8 to avoid 32bit calculations */
    diff = (diff + 4) / 8;
    TICK_DV = ((uint16_t)(F_CPU / (64*8)) - (diff / 2)) / diff;
#else
    TICK_DV = ( (uint16_t)(F_CPU / 64) - ((diff + 2) / 2) ) / diff;
#endif
}
