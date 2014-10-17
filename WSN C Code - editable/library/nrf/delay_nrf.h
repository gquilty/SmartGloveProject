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
 * Static inline function for generating microsecond delays. This function
 * should be tested with different compilers due to different optimisations that
 * might be done.
 *
 * @file delay_nrf.h
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#ifndef DELAY_NRF_H
#define DELAY_NRF_H


/**
 * Delay function for generating microsecond delays. For this function to
 * generate accurate delays, full optimisation for size MUST be turned on in
 * the IAR compiler. Here is the assembly code generated for calling this
 * function with two different values:
 * @code
 *          Instruction         Instruction cycles
 *
 *  delay_us(0x12);
 *          MOV     R2,#12      2
 *      loop:
 *          MOV     A,R2        1
 *          DEC     R2          1
 *          JNZ     loop        3
 *
 *  delay_us(0x3456);
 *          MOV     R3,#56      2
 *          MOV     R4,#34      2
 *          SJMP    startLoop   3
 *      outerLoop:
 *          NOP                 1
 *          MOV     R2,#0xd4    2
 *      innerLoop:
 *          NOP                 1
 *          DEC     R2          1
 *          MOV     A,R2        1
 *          JNZ     innerLoop   3
 *      startLoop:
 *          MOV     A,R4        1
 *          DEC     R4          1
 *          JNZ     outerLoop   3
 *      endloop:
 *          MOV     A,R3        1
 *          DEC     R3          1
 *          JNZ     endloop     3
 * @endcode
 *
 * @param pause how many microseconds to delay for. This MUST be a
 *     compile-time constant. If @c F_CPU is not 20MHz, then this value is 
 *     rounded to the nearest multiple of 20e6/F_CPU.
 **/
FORCE_INLINE static inline void delay_us(uint16_t pause)
{
    uint8_t b0, b1, i;

    /*
     * Adjust delay cycles for different clock speeds. As pause is a compile-
     * time constant, this calculation is done by the compiler.
     */
    pause = (uint16_t)((float)pause / (20e6 / (float)F_CPU));

    b0 = LOW_BYTE(pause);

    /* Test for 0 delay */
    if (pause == 0) {
        return;
    }

    /* Do loops lasting 256us for the value of the high byte (if necessary) */
    if (pause > 0xFF) {
        b1 = HIGH_BYTE(pause);

        /* Each loop takes 1280 cycles. This is 256us with a 20MHz clock */
        while(b1--) {
            nop();
            /* Each loop takes 6 cycles. 6 * 212 = 1272 cycles in total */
            for (i = 212; i > 0; --i){
                nop();
            }
        }
    }

    /* Do loops for the low byte. This loop takes 5 cycles or 1us */
    while(b0--) {
        ;
    }
}


#endif
