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
 * Functions for using the ADC of the nRF9E5.
 *
 * @file adc_nrf.c
 * @date 27-Jan-2010
 * @author Seán Harte, Finbarr Quinlan
 ******************************************************************************/


#include "global.h"
#include "adc.h"
#include "spi.h"
#include "spi_nrfCommands.h"


/**
 * Store configuration byte locally, so that the current configuration does not
 * have to be read, everytime you want to change it.
 **/
static uint8_t config;

/** The number of bits being used for each conversion. **/
static uint8_t numBits;

/** The most recent conversion result. **/
static volatile uint16_t result_local;

/** Keep track of when a new conversion result is ready. **/
static volatile __no_init __bit bool isNewValue;


static void setConfigByte0(void);


/******************************************************************************\
 * See adc.h for documentation of these functions.
\******************************************************************************/

void adc_init(uint8_t channel, uint8_t bits, uint8_t voltageRef)
{
    config = (channel << CHSEL) | (voltageRef << VFSSEL) | BIT(ADC_PWR_UP);
    numBits = bits;

    /* Send 2 ADC config bytes, right justification is used. */
    disableInterrupts();
    spi_enableCsn();
    spi_readWriteByte(W_ADC_CONFIG);
    spi_readWriteByte(config);
    spi_readWriteByte(bits | (ADC_RL_JUST_RIGHT << ADC_RL_JUST));
    spi_disableCsn();
    enableInterrupts();
    
    /* Enable interrupt */
    EX2 = 1;
    isNewValue = false;
}


void adc_readMultiple(uint8_t* data, uint8_t channels)
{
    uint8_t i;

    /* Loop through each channel */
    for (i = 0; i < 8; ++i) {
        /* Check if this channel should be read */
        if (BIT(i) & channels) {
            /* Set channel, and start conversion. */
            config &= ~(BIT(CSTARTN) | BIT(ADCRUN) | (ADC_MAX_CHANNEL << CHSEL));
            setConfigByte0();
            config |= BIT(CSTARTN) | (i << CHSEL);
            setConfigByte0();

            /* Read channel */
            *(uint16_t*)(data + (i * 2)) = adc_read();
        }
    }
}


void adc_startContinuous(void)
{
    config |= BIT(ADCRUN) | BIT(CSTARTN);
    setConfigByte0();
}


#ifndef ADC_USE_CALLBACK
uint16_t adc_read(void)
{
    while (!isNewValue) {
        ;
    }

    isNewValue = false;
    return result_local;
}
#endif


uint16_t adc_singleConversion(void)
{
    /* Stop any current conversions */
    config &= ~(BIT(CSTARTN) | BIT(ADCRUN));
    setConfigByte0();
    
    /* Start single conversion */
    config |= BIT(CSTARTN);
    setConfigByte0();
#ifndef ADC_USE_CALLBACK
    return adc_read();
#else
    return 0;
#endif
}


void adc_disable(void)
{
    config &= ~(BIT(ADCRUN) | BIT(ADC_PWR_UP) | BIT(CSTARTN));
    setConfigByte0();
}


void adc_enable(void)
{
    ;  /* Call adc_singleConversion() or adc_startContinuous() */
}


/******************************************************************************\
 * Functions used only within this file.
\******************************************************************************/

/**
 * Interrupt service routine that is signalled when an ADC conversion has
 * finished. The resulting value is stored locally, and a flag set to indicate
 * that a new value has been generated.
 **/
ISR(IE2_int)
{
    /*
     * A logical AND is done to set unused bits of the adc data to 0, as this is
     * not guaranteed by the hardware.
     */
    static uint16_t bitmask[4] = {0x003F, 0x00FF, 0x03FF, 0x0FFF};

    disableInterrupts();
    EXIF &= ~BIT(IE2);      /* Clear flag */

    /*
     * SPI functions are not used, as it causes a lot of bloat when calling
     * them from within an interrupt.
     */
    spi_enableCsn();
    SPI_WRITE_BYTE(R_ADC_DATA | 0);
    result_local = SPI_READ_WRITE_BYTE(0);                      /* Low byte */
    result_local |= ((uint16_t)SPI_READ_WRITE_BYTE(0) << 8);    /* High byte */
    result_local &= bitmask[numBits];
    spi_disableCsn();
    
    isNewValue = true;

#ifdef ADC_USE_CALLBACK
    adc_callback(result_local);
#endif
    enableInterrupts();
}


/**
 * Writes a value to byte0 of the ADC configuration register. The byte written
 * is the module variable config.
 **/
static void setConfigByte0(void)
{
    disableInterrupts();
    spi_enableCsn();
    spi_readWriteByte(W_ADC_CONFIG);
    spi_readWriteByte(config);
    spi_disableCsn();
    enableInterrupts();
}
