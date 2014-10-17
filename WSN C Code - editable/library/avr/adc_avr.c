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
 * Implementation of ADC sampling functions for ATmega128.
 *
 * @file adc_avr.c
 * @date 12-Feb-2010
 * @author Seán Harte, Finbarr Quinlan
 ******************************************************************************/
 

#include "global.h"
#include "adc.h"


/* Values for ADC clock prescaler. */
#define PRESCALER_DIV2      0x00
#define PRESCALER_DIV4      0x02
#define PRESCALER_DIV8      0x03
#define PRESCALER_DIV16     0x04
#define PRESCALER_DIV32     0x05
#define PRESCALER_DIV64     0x06
#define PRESCALER_DIV128	0x07


/* ATmega1281 compatibility. */
#ifdef _AVR_IOMXX0_1_H_
#   define ADFR ADATE
#endif


/* The result of the most recent conversion from adc_startContinuous(). */
static volatile uint16_t result_local = 0;
static volatile bool isNewValue = false;


/******************************************************************************\
 * See adc.h for documentation of these functions.
\******************************************************************************/

void adc_init(uint8_t channel, uint8_t bits, uint8_t voltageRef)
{

    uint32_t maxAdcClock = 200000UL;
    uint8_t adcPrescaler;

    /* Check parameters */
    bits = ADC_BITS_10;
    if (channel > ADC_MAX_CHANNEL) {
        channel = ADC_MAX_CHANNEL;  /* GND reference */
    }
    if (voltageRef > ADC_REF_INTERNAL) {
        voltageRef = ADC_REF_VCC;
    }

    /* Calculate clock divider value based on F_CPU. */
    adcPrescaler = F_CPU / maxAdcClock;
    if      (adcPrescaler < 2)  { adcPrescaler = PRESCALER_DIV2; }
    else if (adcPrescaler < 4)  { adcPrescaler = PRESCALER_DIV4; }
    else if (adcPrescaler < 8)  { adcPrescaler = PRESCALER_DIV8; }
    else if (adcPrescaler < 16) { adcPrescaler = PRESCALER_DIV16; }
    else if (adcPrescaler < 32) { adcPrescaler = PRESCALER_DIV32; }
    else if (adcPrescaler < 64) { adcPrescaler = PRESCALER_DIV64; }
    else                        { adcPrescaler = PRESCALER_DIV128; }

    /* Set clock */
    ADCSRA = (ADCSRA & ~0x03) | (adcPrescaler);

    /* Set channel, voltage reference and right justification */
    ADMUX = (voltageRef << REFS0) | channel;

    /* Enable ADC */
    ADCSRA |= BIT(ADEN) | BIT(ADIE);
    isNewValue = false;
    enableInterrupts();
}


void adc_readMultiple(uint8_t* data, uint8_t channels)
{
    uint8_t i;
    uint16_t temp;
    
    /* Loop through each channel */
    for (i = 0; i < 8; ++i) {
        /* Check if this channel should be read */
        if (BIT(i) & channels) {
            /* Set channel */
            ADMUX &= ~ADC_MAX_CHANNEL;
            ADMUX |= i;
            temp = adc_singleConversion();
            data[i * 2] = HIGH_BYTE(temp);
            data[i * 2 +1] = LOW_BYTE(temp);
        }
    }
}


void adc_startContinuous(void)
{
    /* Check that ADC is initialised */
    if ( !(ADCSRA & BIT(ADEN))) {
        return;
    }
    ADCSRA |= BIT(ADFR) | BIT(ADSC);
}


#ifndef ADC_USE_CALLBACK
uint16_t adc_read(void)
{
    uint16_t result;

    /* Check that ADC is enabled */
    if ( !(ADCSRA & BIT(ADEN))) {
        return 0xFFFF;
    }
    
    while (!isNewValue) {
        ;
    }
    disableInterrupts();
    isNewValue = false;
    result = result_local;
    enableInterrupts();
    
    return result;
}
#endif


uint16_t adc_singleConversion(void)
{
    uint16_t result;
        
    /* Check that ADC is enabled */
    if ( !(ADCSRA & BIT(ADEN))) {
        return 0xFFFF;
    }

    /* Start single conversion */
    ADCSRA &= ~BIT(ADFR);
    ADCSRA |= BIT(ADSC);
    
#ifndef ADC_USE_CALLBACK
    /* Wait for conversion to complete */
    while (!isNewValue) {
        ;
    }
    disableInterrupts();
    isNewValue = false;
    result = result_local;
    enableInterrupts();
    return result;
#else
    return 0;
#endif
}


void adc_disable(void)
{
    ADCSRA &= ~(BIT(ADEN) | BIT(ADIE));
}


void adc_enable(void)
{
    ADCSRA |= BIT(ADEN) | BIT(ADIE);
}


/******************************************************************************\
 * Functions used only within this file.
\******************************************************************************/

/**
 * Interrupt service routine that is signalled when an ADC conversion has
 * finished. The resulting value is stored locally, and a flag set to indicate
 * that a new value has been generated.
 **/
ISR(ADC_vect)
{
    uint8_t resultLowByte;

    /* Must read ADCL before ADCH */
    resultLowByte = ADCL;
    result_local = TO_UINT16(ADCH, resultLowByte);
    isNewValue = true;
    
#ifdef ADC_USE_CALLBACK
    adc_callback(result_local);
#endif
}
