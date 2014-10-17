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
 * ATmega128 specific ADC definitions.
 *
 * @file adc_avr.h
 * @date 12-Feb-2010
 * @author Seán Harte
 ******************************************************************************/


#ifndef ADC_AVR_H
#define ADC_AVR_H


/** External voltage reference (@a voltageRef parameter of @c adc_init()). **/
#define ADC_REF_EXTERNAL        0

/** VCC voltage reference (@a voltageRef parameter of @c adc_init()). **/
#define ADC_REF_VCC             1

/**
 * 1.1V Internal voltage reference (@a voltageRef parameter of @c adc_init()).
 * Not available on ATmega128.
 **/
#if defined (_AVR_IOMXX0_1_H_) || defined (__DOXYGEN__)
#   define ADC_REF_INTERNAL_1_1V   2
#endif

/**
 * 2.56V Internal voltage reference (@a voltageRef parameter of @c adc_init()).
 **/
#define ADC_REF_INTERNAL        3


/** Valid options for number of bits in conversion. **/
#define ADC_BITS_10             10


/** ADC channel 0. **/
#define ADC_CH_ADC0             0x00

/** ADC channel 1 **/
#define ADC_CH_ADC1             0x01

/** ADC channel 2. **/
#define ADC_CH_ADC2             0x02

/** ADC channel 3. **/
#define ADC_CH_ADC3             0x03

/** ADC channel 4. **/
#define ADC_CH_ADC4             0x04

/** ADC channel 5. **/
#define ADC_CH_ADC5             0x05

/** ADC channel 6. **/
#define ADC_CH_ADC6             0x06

/** ADC channel 7. **/
#define ADC_CH_ADC7             0x07


/** Differential conversion (pos = ADC1, neg = ADC0, 10x gain). **/
#define ADC_CH_1_0_DIFF10X      0x09

/** Differential conversion (pos = ADC1, neg = ADC0, 200x gain). **/
#define ADC_CH_1_0_DIFF200X     0x0B

/** Differential conversion (pos = ADC3, neg = ADC2, 10x gain). **/
#define ADC_CH_3_2_DIFF10X      0x0D

/** Differential conversion (pos = ADC3, neg = ADC2, 200x gain). **/
#define ADC_CH_3_2_DIFF200X     0x0F

/** Differential conversion (pos = ADC0, neg = ADC1, 1x gain). **/
#define ADC_CH_0_1_DIFF1X       0x10

/** Differential conversion (pos = ADC2, neg = ADC1, 1x gain). **/
#define ADC_CH_2_1_DIFF1X       0x12

/** Differential conversion (pos = ADC3, neg = ADC1, 1x gain). **/
#define ADC_CH_3_1_DIFF1X       0x13

/** Differential conversion (pos = ADC4, neg = ADC1, 1x gain). **/
#define ADC_CH_4_1_DIFF1X       0x14

/** Differential conversion (pos = ADC5, neg = ADC1, 1x gain). **/
#define ADC_CH_5_1_DIFF1X       0x15

/** Differential conversion (pos = ADC6, neg = ADC1, 1x gain). **/
#define ADC_CH_6_1_DIFF1X       0x16

/** Differential conversion (pos = ADC7, neg = ADC1, 1x gain). **/
#define ADC_CH_7_1_DIFF1X       0x17

/** Differential conversion (pos = ADC0, neg = ADC2, 1x gain). **/
#define ADC_CH_0_2_DIFF1X       0x18

/** Differential conversion (pos = ADC1, neg = ADC2, 1x gain). **/
#define ADC_CH_1_2_DIFF1X       0x19

/** Differential conversion (pos = ADC3, neg = ADC2, 1x gain). **/
#define ADC_CH_3_2_DIFF1X       0x1B

/** Differential conversion (pos = ADC4, neg = ADC2, 1x gain). **/
#define ADC_CH_4_2_DIFF1X       0x1C

/** Differential conversion (pos = ADC5, neg = ADC2, 1x gain). **/
#define ADC_CH_5_2_DIFF1X       0x1D


/** Internal bandgap 1.23V reference. **/
#define ADC_CH_1V23             0x1E


/** Read Ground value (should be 0). **/
#define ADC_CH_GND              0x1F


/** Maximum valid channel. **/
#define ADC_MAX_CHANNEL         0x1F


#endif
