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
 * Interface for Sensirion SHTxx (SHT71, SHT11 etc.) humidity/temperature
 * sensors.
 *
 * @file sht.h
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#ifndef SHT_H
#define SHT_H


/** Command used when calling @c sht_read(). **/
#define SHT_TEMPERATURE         3

/** Command used when calling @c sht_read(). **/
#define SHT_HUMIDITY            5


/* Pins used to communicated with sensor. */
#if defined (UC_AVR)
#   define SHT_CLOCK_PORT       PORTD
#   define SHT_CLOCK_DDR        DDRD
#   define SHT_CLOCK_PIN        PIND
#   define SHT_CLOCK            0

#   define SHT_DATA_PORT        PORTD
#   define SHT_DATA_DDR         DDRD
#   define SHT_DATA_PIN         PIND
#   define SHT_DATA             1
#else
#   define SHT_CLOCK_PORT       P0
#   define SHT_CLOCK_DDR        P0_DIR
#   define SHT_CLOCK_PIN        P0
#   define SHT_CLOCK            3

#   define SHT_DATA_PORT        P0
#   define SHT_DATA_DDR         P0_DIR
#   define SHT_DATA_PIN         P0
#   define SHT_DATA             5
#endif

/**
 * Choose resolution of ADC on SHT.
 *  - 0: 14bit temperature and 12bit humidity.
 *  - 1: 12bit temperature and 8bit humidity.
 **/
#ifndef SHT_LOW_RES_ADC
#   define SHT_LOW_RES_ADC         1
#endif

/**
 * Set whether to reload calibration data from OTP for each measurement as
 * in rare conditions data may be lost from volatile memory.
 *  - 0: Data is reloaded (safe).
 *  - 1: Data is not reloaded (faster by 8.2ms).
 **/
#define SHT_NO_RELOAD_FROM_OTP  1

/**
 * Set if a CRC check will be done on data read from SHT.
 *  - 0: Check is not done.
 *  - 1: CRC is checked.
 **/
#define SHT_CHECK_CRC           0


/**
 * Control accuracy of algorithm used by sht_convert()
 *  - 0: Quick, less accurate algorithm is used (10mm default).
 *  - 1: Slow, more accurate algorithm is used (25mm default).
 **/
#ifdef UC_AVR
#   define SHT_CONV_ACCURACY_HIGH  1
#else
#   define SHT_CONV_ACCURACY_HIGH  0
#endif


/**
 * Starts an ADC conversion and records the result. Note that this function
 * blocks while waiting for the result, which could take 210ms.
 *
 * @param cmd should be either @c SHT_TEMPERATURE or @c SHT_HUMIDITY.
 * @param value where to store the reading.
 *
 * @return @c STATUS_OK or error value.
 **/
status_t sht_read(uint8_t cmd, uint16_t* value);


/**
 * Converts raw data value to degrees celsius, or relative humidity (%)
 *
 * @param cmd should be either @c SHT_TEMPERATURE or @c SHT_HUMIDITY.
 * @param value raw data value to convert.
 *
 * @return degrees celsius or percentage relative humidity. This is a 32bit
 *     float for high accuracy conversion (25mm default), or 8-bit integer for
 *     low-accuracy conversion (10mm default).
 **/
#if SHT_CONV_ACCURACY_HIGH == 1
float sht_convert(uint8_t cmd, uint16_t value);
#else
int8_t sht_convert(uint8_t cmd, uint16_t value);
#endif


#endif
