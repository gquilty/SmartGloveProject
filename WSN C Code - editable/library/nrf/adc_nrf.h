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
 * nRF9E5 specific ADC definitions.
 *
 * @file adc_nrf.h
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#ifndef ADC_NRF_H
#define ADC_NRF_H


/** Internal voltage reference (@a voltageRef parameter of @c adc_init()). **/
#define ADC_REF_INTERNAL        0

/** External voltage reference (@a voltageRef parameter of @c adc_init()). **/
#define ADC_REF_EXTERNAL        1


/** 6-bit conversion (@a bits parameter of @c adc_init(). **/
#define ADC_BITS_6              (0 << RESCTRL)

/** 8-bit conversion (@a bits parameter of @c adc_init(). **/
#define ADC_BITS_8              (1 << RESCTRL)

/** 10-bit conversion (@a bits parameter of @c adc_init(). **/
#define ADC_BITS_10             (2 << RESCTRL)

/** 12-bit conversion (@a bits parameter of @c adc_init(). **/
#define ADC_BITS_12             (3 << RESCTRL)


/** ADC channel 0. **/
#define ADC_CH_ADC0             0x00

/** ADC channel 1. **/
#define ADC_CH_ADC1             0x01

/** ADC channel 2. **/
#define ADC_CH_ADC2             0x02

/** ADC channel 3. **/
#define ADC_CH_ADC3             0x03

/** ADC channel VCC/3. **/
#define ADC_CH_VDD_DIV3         0x0F

/** Maximum valid channel. **/
#define ADC_MAX_CHANNEL         0x0F


#endif
