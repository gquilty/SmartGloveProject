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
 * Hardware independent interface for Analogue to Digital Converters (ADC).
 *
 * @file adc.h
 * @date 27-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#ifndef ADC_H
#define ADC_H


#if defined (UC_AVR)
#   include "avr/adc_avr.h"
#elif defined (UC_8051)
#   include "nrf/adc_nrf.h"
#endif


/**
 * Configure and enable the ADC. If this is not called before the other
 * functions are used, they will not return correct values! This function is
 * also used for changing the ADC parameters.
 *
 * @param channel which channel to read (see @e adc_avr.h and @e adc_nrf.h).
 * @param bits bit depth of conversion. The ATmega128 only supports 10 bits.
 *     The nRF9E5 supports, 6, 8, 10 or 12.
 * @param voltageRef source of reference voltage: @c ADC_REF_INTERNAL,
 *     @c ADC_REF_EXTERNAL, or @c ADC_REF_VCC.
 **/
void adc_init(uint8_t channel, uint8_t bits, uint8_t voltageRef);


/**
 * Configures the ADC, and can read from multiple channels. @c adc_init() should
 * be used first to set the bit depth and the reference voltage. As the channel
 * is changed between conversions this will be slower than using the other ADC
 * read functions. Note that when using adc_readMultiple(), the @a channel
 * parameter used in @c adc_init() has no effect.
 *
 * @param[out] data where to store the readings. Each reading takes two bytes.
 *     Bit 15 will be 0. Bits 14-12 identify which ADC channel the reading is
 *     from, andthe lower bits are the results of the conversion. Readings are
 *     in order of channels from lowest to highest.
 * @param channels bitmask of which channels to read. Bit 0 is ADC0 etc.
 **/
void adc_readMultiple(uint8_t* data, uint8_t channels);


/**
 * Start the ADC into continuous conversion mode. Conversion results can be got
 * by using @c adc_read() or from @c adc_callback().
 **/
void adc_startContinuous(void);


#ifndef ADC_USE_CALLBACK
/**
 * Read the last value that has been sampled. Each call will return a newly read
 * value. (The result from the same conversion won't be returned twice). This
 * means that the function will block if called too quickly. This function is
 * not availble if @c ADC_USE_CALLBACK is defined.
 *
 * @return last value that has been sampled.
 **/
uint16_t adc_read(void);
#endif


/**
 * Perform a single ADC conversion and return the value. If continuous
 * conversions are happenning, and this is called, they will not continue after
 * this function stops.
 *
 * @return value that has been sampled. If @c ADC_USE_CALLBACK is defined, 0 is
 *     returned here, and the conversion result is given by @c adc_callback().
 **/
uint16_t adc_singleConversion(void);


/** Stop conversions and turn off ADC. **/
void adc_disable(void);


/**
 * Put ADC back to state before @c adc_disable() was called. The adc will not be
 * properly enabled if this is called before @c adc_init().
 **/
void adc_enable(void);


#if defined(ADC_USE_CALLBACK) || defined(__DOXYGEN__)
/**
 * This function is defined by the application. It is signalled when a
 * conversion has been completed. This is only called if @c ADC_USE_CALLBACK has
 * been defined. It will be signalled regularly after @c adc_startContinuous()
 * is called.
 *
 * @param value result of conversion.
 **/
void adc_callback(uint16_t value);
#endif


#endif
