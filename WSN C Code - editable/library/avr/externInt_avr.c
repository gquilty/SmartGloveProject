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
 * Functions to easily enable and use external interrupts on the Atmega.
 *
 * @file externInt_avr.c
 * @date 05-Feb-2010
 * @author Seán Harte
 ******************************************************************************/


#include "global.h"
#include "externInt.h"
#include "avr/interrupt.h"


/******************************************************************************\
 * See externInt.h for documentation of these functions.
\******************************************************************************/

void externInt_init(uint8_t interrupt, uint8_t mode)
{
    /* Make sure parameters are ok */
    mode &= 0x03;
    interrupt &= 0x07;

#if defined (UC_AVR) && defined (_25_2420_REVA_H)
    if (interrupt == 0) return;
#elif defined (UC_AVR)
    if (interrupt == 4) return;
#endif
    
    externInt_disable(interrupt);
    /* The first 4 interrups are configured in EICRA register */
    if (interrupt <= 3) {
        DDRD &= ~BIT(interrupt);
        /* Set both bits to 0, and then set mode */
        EICRA &= ~(3 << (interrupt*2));
        EICRA |= mode << (interrupt*2);
    }
    else {
        DDRE &= ~BIT(interrupt);
        /* Subtract 4 so that same method as above can be used for EICRB */
        interrupt -= 4;
        EICRB &= ~(3 << (interrupt*2));
        EICRB |= mode << (interrupt*2);
        interrupt += 4;
    }
    externInt_clear(interrupt);
    externInt_enable(interrupt);
    enableInterrupts();
}


void externInt_enable(uint8_t interrupt)
{
    /* Make sure interrupt is between 0 and 7 */
    interrupt &= 0x07;

#if defined (UC_AVR) && defined (_25_2420_REVA_H)
    if (interrupt == 0) return;
#elif defined (UC_AVR)
    if (interrupt == 4) return;
#endif

    /* Enable interrupt */
    EIMSK |= BIT(interrupt);
}


void externInt_disable(uint8_t interrupt)
{
    /* Make sure interrupt is between 0 and 7 */
    interrupt &= 0x07;

#if defined (UC_AVR) && defined (_25_2420_REVA_H)
    if (interrupt == 0) return;
#elif defined (UC_AVR)
    if (interrupt == 4) return;
#endif

    /* Enable interrupt */
    EIMSK &= ~BIT(interrupt);
}


void externInt_clear(uint8_t interrupt)
{
#if defined (UC_AVR) && defined (_25_2420_REVA_H)
    if (interrupt == 0) return;
#elif defined (UC_AVR)
    if (interrupt == 4) return;
#endif

    /* Make sure interrupt is between 0 and 7 */
    interrupt &= 0x07;

    /* Enable interrupt */
    EIFR |= BIT(interrupt);
}


#if !defined (_25_2420_REVA_H)
ISR(INT0_vect)
{
    externInt_callback(0);
}
#endif


ISR(INT1_vect)
{
    externInt_callback(1);
}


ISR(INT2_vect)
{
    externInt_callback(2);
}


ISR(INT3_vect)
{
    externInt_callback(3);
}

#if !defined (UC_AVR) || defined (_25_2420_REVA_H)
ISR(INT4_vect)
{
    externInt_callback(4);
}
#endif



ISR(INT5_vect)
{
    externInt_callback(5);
}


ISR(INT6_vect)
{
    externInt_callback(6);
}


ISR(INT7_vect)
{
    externInt_callback(7);
}

