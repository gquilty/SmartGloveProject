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
 * SPI master implementation.
 * @todo Software control of SPI clock speed. Currently the maximum is used.
 * @todo Software implementation of SPI (bit-banging) for using SPI on different
 *     pins.
 *
 * @file spi.h
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#ifndef SPI_H
#define SPI_H


#if defined (UC_AVR)
#   include "avr/spi_avr.h"
#elif defined (UC_8051)
#   include "nrf/spi_nrf.h"
#endif


/** Initialise SPI. **/
static inline void spi_init(void);


/** Disable SPI. **/
static inline void spi_disable(void);


/** Enable SPI. **/
static inline void spi_enable(void);


/**
 * Send and receive a single byte using SPI. Note that chip select must be
 * handled elsewhere.
 *
 * @param byte byte to send.
 *
 * @return byte that was read back.
 **/
uint8_t spi_readWriteByte(uint8_t byte);


/**
 * Receive an array of bytes using SPI. Note that chip select must be
 * handled elsewhere.
 *
 * @param[out] readBytes where to store bytes that are read.
 * @param size how many bytes to read.
 **/
void spi_readArray(uint8_t* readBytes, uint16_t size);


/**
 * Send an array of bytes using SPI. Note that chip select must be
 * handled elsewhere.
 *
 * @param writeBytes bytes to write.
 * @param size how many bytes to write.
 **/
void spi_writeArray(const uint8_t* writeBytes, uint16_t size);


/**
 * Send and receive an array of bytes using SPI. Note that chip select must be
 * handled elsewhere. 
 *
 * @param[out] readBytes where to store bytes that are read.
 * @param writeBytes bytes to write.
 * @param size how many bytes to read/write.
 **/
void spi_readWriteArray(uint8_t* readBytes,
                                      const uint8_t* writeBytes, uint16_t size);


#endif
